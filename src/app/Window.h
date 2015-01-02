#pragma once

#include "Macro.h"
#include "Control.h"
#include "Transform.h"
#include "MyTime.h"
#include "Model.h"
#include "Font.h"

SE_BEGIN

#if PLATFORM == PLATFORM_ANDROID
class MyGLContext
{
private:
    //ELG configurations
    ANativeWindow* _window;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
    EGLConfig _config;

    //Screen parameters
    int32_t _iWidth;
    int32_t _iHeight;
    int32_t _iColorSize;
    int32_t _iDepthSize;

    //Flags
    bool _bGLESInitialized;
    bool _bEGLContextInitialized;
    bool _bES3Support;
    float _fGLVersion;
    bool _bContextValid;

    void initGLES();
    void terminate();
    bool initEGLSurface();
    bool initEGLContext();
public:
    static MyGLContext* getInstance()
    {
        //Singleton
        static MyGLContext instance;

        return &instance;
    }

    MyGLContext( MyGLContext const& );
    void operator=( MyGLContext const& );

    MyGLContext();
    virtual ~MyGLContext();

    bool init( ANativeWindow* window );
    EGLint swap();
    bool invalidate();

    void suspend();
    EGLint resume(ANativeWindow* window);

    int32_t getScreenWidth() { return _iWidth; }
    int32_t getScreenHeight() { return _iHeight; }

    int32_t getBufferColorSize() { return _iColorSize; }
    int32_t getBufferDepthSize() { return _iDepthSize; }
    float getGLVersion() { return _fGLVersion; }
    bool checkExtension( const char* extension );
};
#endif

class Window : public Container{
protected:
#if PLATFORM == PLATFORM_WINDOWS
	HWND hwnd = 0;
	HDC hdc;
	HGLRC rc;
	bool trackingMouseLeave = false;
#elif PLATFORM == PLATFORM_LINUX
	Display *display = NULL;
	::Window window;
#elif PLATFORM == PLATFORM_ANDROID
	struct android_app* app = NULL;
	ASensorManager* sensorManager = NULL;
	const ASensor* accelerometerSensor = NULL;
	ASensorEventQueue* sensorEventQueue = NULL;
	MyGLContext* glContext;
	
	bool sizeChanged = false;
	bool created = false;
	int64_t timeSizeChanged;
	
public:
	static struct android_app* state;
	AAssetManager* GetAssetManager(){
		return app->activity->assetManager;
	}
	//friend void android_main(struct android_app* state);
private:
	static int32_t engine_handle_input(android_app* app, AInputEvent* event);
	static void engine_handle_cmd(android_app* app, int32_t cmd);
	void engine_draw_frame();
	void engine_term_display();
#endif

	bool resourcesLoaded = false;
	bool running = false;
	string title = "SE Window";
	static int id;
	int64_t last = 0;
	int fs = 0;
	float dt = 0, ts = 0, fps = 0, et = 0;
	
	static list<Window*> windows;
	static Window* startupWindow;

	bool CreateImpl();
	void SwapBuffers();
	
	void UpdateFrame();
	

protected:
	bool Create();

	void UpdateTime(){
		int64_t t = GetMicroSecond();
		if (last != 0){
			dt = (t - last) / 1000000.0f;
			fs++;
			if ((ts += dt) > 0.2f){
				fps = fs / ts;
				ts = 0.0f;
				fs = 0;
			}
		}
		last = t;
		et += dt;
	}


	virtual void OnResize(const Vector2& size) override;
	virtual void OnMouseEvent(MouseEventArgs e);
	virtual void OnKeyEvent(KeyEventArgs e);
	virtual void OnLoad();
	virtual void OnUnload();
public:
	Window();
	~Window();
	void Run();
	void Show();
	void Hide();
	void Close();
	void SetTitle(const string&);
	const string& GetTitle(){ return title; }
	void SetLocation(const Vector2&) override;
	void SetSize(const Vector2&) override;
	//const Vector2& GetLocation() const { return location; }
	//const Vector2& GetSize() const { return size; }

	static int GetScreenWidth();
	static int GetScreenHeight();

	virtual void OnDraw(DrawEventArgs e)
	{
		//glClearColor(RGBAF(backColor));
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		PushMatrix();
		Container::OnDraw(e);
		PopMatrix();
	}
	virtual void OnUpdate(float dt) { Container::OnUpdate(dt); }
	
	list<Window*> GetWindows(){ return windows; }
#ifdef WINDOWS
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	static Window* FindWindowByHwnd(HWND hwnd);
	HWND GetHandle() const { return hwnd; }
#endif


	float GetDeltaTime(){
		return dt;
	}

	float GetElapsedTime(){
		return et;
	}

	float GetFPS(){
		return fps;
	}
};

SE_END
