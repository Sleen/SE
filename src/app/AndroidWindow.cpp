
#include "Window.h"
#include "MyTime.h"

#if PLATFORM == PLATFORM_ANDROID

#include <jni.h>

SE_BEGIN


MyGLContext::MyGLContext() : _display(EGL_NO_DISPLAY),
_surface(EGL_NO_SURFACE),
_context(EGL_NO_CONTEXT),
_iWidth( 0 ),
_iHeight( 0 ),
_bES3Support( false ),
_bEGLContextInitialized( false ),
_bGLESInitialized( false )
{
}

void MyGLContext::initGLES()
{
    if( _bGLESInitialized )
        return;
    //
    //Initialize OpenGL ES 3 if available
    //
    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")
        && gl3stubInit())
    {
        _bES3Support = true;
        _fGLVersion = 3.0f;
    }
    else
    {
        _fGLVersion = 2.0f;
    }

    _bGLESInitialized = true;
}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
MyGLContext::~MyGLContext()
{
    terminate();
}

bool MyGLContext::init( ANativeWindow* window )
{
    if( _bEGLContextInitialized )
        return true;

    //
    //Initialize EGL
    //
    _window = window;
    initEGLSurface();
    initEGLContext();
    initGLES();

    _bEGLContextInitialized = true;

    return true;
}

bool MyGLContext::initEGLSurface()
{
    _display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    eglInitialize( _display, 0, 0 );

    /*
    * Here specify the attributes of the desired configuration.
    * Below, we select an EGLConfig with at least 8 bits per color
    * component compatible with on-screen windows
    */
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,    //Request opengl ES2.0
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    _iColorSize = 8;
    _iDepthSize = 24;

    EGLint numConfigs;
    eglChooseConfig( _display, attribs, &_config, 1, &numConfigs );

    if( !numConfigs )
    {
        //Fall back to 16bit depth buffer
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,    //Request opengl ES2.0
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
        };
        eglChooseConfig( _display, attribs, &_config, 1, &numConfigs );
        _iDepthSize = 16;
    }

    if ( !numConfigs )
    {
        LOGW("Unable to retrieve EGL config");
        return false;
    }

    _surface = eglCreateWindowSurface( _display, _config, _window, NULL );
    eglQuerySurface(_display, _surface, EGL_WIDTH, &_iWidth);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &_iHeight);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    * As soon as we picked a EGLConfig, we can safely reconfigure the
    * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    EGLint format;
    eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry( _window, 0, 0, format);

    return true;
}

bool MyGLContext::initEGLContext()
{
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,  //Request opengl ES2.0
        EGL_NONE
    };
    _context = eglCreateContext( _display, _config, NULL, contextAttribs );

    if( eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE )
    {
        LOGW("Unable to eglMakeCurrent");
        return false;
    }

    _bContextValid = true;
    return true;
}

EGLint MyGLContext::swap()
{
    bool b = eglSwapBuffers( _display, _surface);
    if( !b )
    {
        EGLint err = eglGetError();
        if( err == EGL_BAD_SURFACE )
        {
            //Recreate surface
            initEGLSurface();
            return EGL_SUCCESS; //Still consider glContext is valid
        }
        else if( err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT )
        {
            //Context has been lost!!
            _bContextValid = false;
            terminate();
            initEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

void MyGLContext::terminate()
{
    if( _display != EGL_NO_DISPLAY )
    {
        eglMakeCurrent( _display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
        if ( _context != EGL_NO_CONTEXT )
        {
            eglDestroyContext( _display, _context );
        }

        if( _surface != EGL_NO_SURFACE )
        {
            eglDestroySurface( _display, _surface );
        }
        eglTerminate( _display );
    }

    _display = EGL_NO_DISPLAY;
    _context = EGL_NO_CONTEXT;
    _surface = EGL_NO_SURFACE;
    _bContextValid = false;

}

EGLint MyGLContext::resume(ANativeWindow* window)
{
    if( _bEGLContextInitialized == false )
    {
        init( window );
        return EGL_SUCCESS;
    }

    int32_t iOriginalWidth = _iWidth;
    int32_t iOriginalHeight = _iHeight;

    //Create surface
    _window = window;
    _surface = eglCreateWindowSurface( _display, _config, _window, NULL );
    eglQuerySurface(_display, _surface, EGL_WIDTH, &_iWidth);
    eglQuerySurface(_display, _surface, EGL_HEIGHT, &_iHeight);

    if( _iWidth != iOriginalWidth || _iHeight != iOriginalHeight )
    {
        //Screen resized
        LOGI("Screen resized");
    }

    if( eglMakeCurrent(_display, _surface, _surface, _context) == EGL_TRUE )
        return EGL_SUCCESS;

    EGLint err = eglGetError();
    LOGW("Unable to eglMakeCurrent %d", err);

    if( err == EGL_CONTEXT_LOST )
    {
        //Recreate context
        LOGI("Re-creating egl context");
        initEGLContext();
    }
    else
    {
        //Recreate surface
        terminate();
        initEGLSurface();
        initEGLContext();
    }

    return err;

}

void MyGLContext::suspend()
{
    if( _surface != EGL_NO_SURFACE )
    {
        eglDestroySurface( _display, _surface );
        _surface = EGL_NO_SURFACE;
    }
}

bool MyGLContext::invalidate()
{
    terminate();

    _bEGLContextInitialized = false;
    return true;
}


bool MyGLContext::checkExtension( const char* extension )
{
    if( extension == NULL )
        return false;

    std::string extensions = std::string( (char*)glGetString(GL_EXTENSIONS) );
    std::string str = std::string( extension );
    str.append( " " );

    size_t pos = 0;
    if( extensions.find( extension, pos ) != std::string::npos )
    {
        return true;
    }

    return false;
}


struct android_app* Window::state = NULL;

#define CLASS_NAME "android/app/NativeActivity"
#define APPLICATION_CLASS_NAME "com/sample/helper/NDKHelper"

jclass retrieve_class(JNIEnv *jni, ANativeActivity* activity,
	const char* className)
{
	jclass activityClass = jni->FindClass(CLASS_NAME);
	jmethodID getClassLoader = jni->GetMethodID(activityClass, "getClassLoader",
		"()Ljava/lang/ClassLoader;");
	jobject cls = jni->CallObjectMethod(activity->clazz, getClassLoader);
	jclass classLoader = jni->FindClass("java/lang/ClassLoader");
	jmethodID findClass = jni->GetMethodID(classLoader, "loadClass",
		"(Ljava/lang/String;)Ljava/lang/Class;");

	jstring strClassName = jni->NewStringUTF(className);
	jclass classRetrieved = (jclass) jni->CallObjectMethod(cls, findClass,
		strClassName);
	jni->DeleteLocalRef(strClassName);
	return classRetrieved;
}

void Unit::InitDeviceInfo(){
	JNIEnv *env;
	jmethodID mid;

	//pthread_mutex_lock(&_mutex);
	Window::state->activity->vm->AttachCurrentThread(&env, NULL);
	jclass _clsJNIHelper = (jclass)env->NewGlobalRef(retrieve_class(env, Window::state->activity, APPLICATION_CLASS_NAME));
	jmethodID constructor = env->GetMethodID(_clsJNIHelper, "<init>", "()V");
	jobject _objJNIHelper = env->NewObject(_clsJNIHelper, constructor);
	_objJNIHelper = env->NewGlobalRef(_objJNIHelper);
	mid = env->GetMethodID(_clsJNIHelper, "GetDpi", "()F");
	screenDpi = env->CallFloatMethod(_objJNIHelper, mid);
	Window::state->activity->vm->DetachCurrentThread();
	//pthread_mutex_unlock(&_mutex);
}

Window::Window() : 
	app( NULL ),
	sizeChanged( false ),
	created( false ),
	sensorManager( NULL ),
	accelerometerSensor( NULL ),
	sensorEventQueue( NULL )
{
	glContext = MyGLContext::getInstance();
}

void jniInit(){
	JNIHelper::getInstance()->init(Window::state->activity);

	JNIEnv *env;
	jmethodID mid;

	Window::state->activity->vm->AttachCurrentThread(&env, NULL);
	jclass _clsJNIHelper = (jclass)env->NewGlobalRef(retrieve_class(env, Window::state->activity, APPLICATION_CLASS_NAME));
	mid = env->GetStaticMethodID(_clsJNIHelper, "setContext", "(Landroid/content/Context;)V");
	env->CallStaticVoidMethod(_clsJNIHelper, mid, Window::state->activity->clazz);
	Window::state->activity->vm->DetachCurrentThread();

	SE::Unit::Init();
}

bool Window::CreateImpl(){
	if( !resourcesLoaded )
	{
		glContext->init( app->window );
		OnResize(Vector2(ANativeWindow_getWidth(app->window), ANativeWindow_getHeight(app->window)));
		jniInit();
		OnLoad();
		resourcesLoaded = true;
	}
	else
	{
		if( EGL_SUCCESS != glContext->resume( app->window ) )
		{
			LOGW( "failed to resume" );
            OnResize(Vector2(ANativeWindow_getWidth(app->window), ANativeWindow_getHeight(app->window)));
			OnUnload();
			OnLoad();
		}
	}

	created = visibled = running = true;
	return true;
}

Window::~Window(){ }

void Window::engine_draw_frame(){
	if (!IsVisibled())
		return;
	
	UpdateFrame();
}

void Window::SwapBuffers(){
	if( EGL_SUCCESS != glContext->swap() )
	{
		DEBUG_LOG("warning: failed to swap buffers. android window will be reload.");
		OnUnload();
		OnLoad();
	}
}

void Window::engine_term_display(){
	created = false;
	visibled = false;
	running = false;
	glContext->suspend();
}

int32_t Window::engine_handle_input(android_app* app, AInputEvent* event) {
	Window* w = (Window*)app->userData;
	int32_t act;
	switch (AInputEvent_getType(event))
	{
	case AINPUT_EVENT_TYPE_MOTION:
		act = AMotionEvent_getAction(event);
		w->OnMouseEvent(MouseEventArgs(AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0), MouseButton::Left,
			act == AMOTION_EVENT_ACTION_DOWN ? MouseAction::Down : act == AMOTION_EVENT_ACTION_UP ? MouseAction::Up
			: act == AMOTION_EVENT_ACTION_MOVE ? MouseAction::Move : MouseAction::None));
		break;
	case AINPUT_EVENT_TYPE_KEY:
		act = AKeyEvent_getAction(event);
		w->OnKeyEvent(KeyEventArgs(AKeyEvent_getKeyCode(event), act == AKEY_EVENT_ACTION_DOWN ? KeyAction::Down : KeyAction::Up));
		break;
	default:
		return 0;
	}
	return 0;
}

void Window::engine_handle_cmd(android_app* app, int32_t cmd) {
	Window* w = (Window*)app->userData;
	if(!w) return;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		LOGI("APP_CMD_SAVE_STATE");
		break;
	case APP_CMD_CONFIG_CHANGED:
		LOGI("APP_CMD_CONFIG_CHANGED");
		{
			w->timeSizeChanged = GetMilliSecond();
			w->sizeChanged = true;
			break;
		}
		//case APP_CMD_CONTENT_RECT_CHANGED:
		//{
		//	int ww = ANativeWindow_getWidth(w->app->window);
		//	int h = ANativeWindow_getHeight(w->app->window);
		//	LOGI("CONTENT_RECT_CHANGED :  %d, %d", ww, h);
		//	w->OnGLResize(ww, h);
		//	break;
		//}
	case APP_CMD_INIT_WINDOW:
		LOGI("APP_CMD_INIT_WINDOW");
		if (w->app->window != NULL) {
			w->Create();
			w->engine_draw_frame();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		LOGI("APP_CMD_TERM_WINDOW");
		w->running = false;
		w->visibled = false;
		w->engine_term_display();
		break;
	case APP_CMD_STOP:
		LOGI( "APP_CMD_STOP" );
		break;
	case APP_CMD_GAINED_FOCUS:
		LOGI("APP_CMD_GAINED_FOCUS");
		if (w->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(w->sensorEventQueue,
				w->accelerometerSensor);

			ASensorEventQueue_setEventRate(w->sensorEventQueue,
				w->accelerometerSensor, (1000L / 60) * 1000);
		}
		if(w->created){
			w->running = true;
			w->visibled = true;
		}
		break;
	case APP_CMD_LOST_FOCUS:
		w->running = false;
		w->visibled = false;
		if (w->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(w->sensorEventQueue,
				w->accelerometerSensor);
		}
		w->engine_draw_frame();
		break;
	}
}

void onResume(ANativeActivity* activity){
	ANativeActivity_setWindowFlags(activity, AWINDOW_FLAG_FULLSCREEN, 0);
}

void Window::Run(){
	app_dummy();

	app = state;
	state->userData = this;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;

	sensorManager = ASensorManager_getInstance();
	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	sensorEventQueue = ASensorManager_createEventQueue(sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	state->activity->callbacks->onResume = onResume;

	while (true) {
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(IsVisibled()?0:-1, NULL, &events,
			(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(sensorEventQueue,
						&event, 1) > 0) {
						//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
					}
				}
			}

			// Check if we are exiting.
			if (app->destroyRequested != 0) {
				LOGI("FINISH");
				engine_term_display();
				return;
			}
		}

		if(IsVisibled() && running && created)
			engine_draw_frame();

		if(sizeChanged && GetMilliSecond()-timeSizeChanged > 50){
			sizeChanged = false;
			int w = ANativeWindow_getWidth(app->window);
			int h = ANativeWindow_getHeight(app->window);
			OnResize(Vector2(w, h));
		}
	}
}

void Window::Show(){
	
}

void Window::Hide(){
	
}

void Window::Close(){
	LOGI("Close()");
	exit(0);
}

void Window::SetLocation(const Vector2& loc){
	
}

void Window::SetSize(const Vector2& size){
	
}

void Window::SetTitle(const string& title){
	this->title = title;
}

int Window::GetScreenWidth(){
	return windows.front()->glContext->getScreenWidth();
}

int Window::GetScreenHeight(){
	return windows.front()->glContext->getScreenHeight();
}

SE_END

#endif
