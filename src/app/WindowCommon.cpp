#include "Window.h"

SE_BEGIN

int Window::id = 0;
list<Window*> Window::windows = list<Window*>();
Window* Window::startupWindow = NULL;

bool Window::Create(){
	if (CreateImpl()){
#ifndef GLES
		
		GLenum err = glewInit();
		if (err != GLEW_OK){
			printf("\nError: %s\n", glewGetErrorString(err));
		}
		wglSwapIntervalEXT(1);
		// 启用线段样式
		glEnable(GL_LINE_STIPPLE);
#endif
		// 启用多重采样
		glEnable(GL_MULTISAMPLE);
		// 启用Alpha混合
		glEnable(GL_BLEND);
		// 混合模式
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		/*
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		*/
		// 清除色
		glClearColor(RGBAF(0xffffffff));

		if (windows.empty())
			startupWindow = this;
		windows.push_back(this);

		return true;
	}
	return false;
}

void Window::OnMouseEvent(MouseEventArgs e){
	if (e.Action == MouseAction::Move)
		OnMouseMove(e);
	else if (e.Action == MouseAction::Down){
		OnMouseDown(e);
#ifdef WINDOWS
		SetCapture(hwnd);
#endif
	}
	else if (e.Action == MouseAction::Up){
		OnMouseUp(e);
#ifdef WINDOWS
		ReleaseCapture();
#endif
	}
}

void Window::OnKeyEvent(KeyEventArgs e){
	if (e.Action == KeyAction::Down)
		OnKeyDown(e);
	else if (e.Action == KeyAction::Up)
		OnKeyUp(e);
	else if (e.Action == KeyAction::Press)
		OnKeyPress(e);
}

void Window::OnResize(const Vector2& size){
	Control::OnResize(size);

	int w = size.X();
	int h = size.Y();

	DEBUG_LOG("OnResize()  %d, %d\n", w, h);

	//SetClientSize((float)w, (float)h);
	bounds.SetSize(w + border.GetWidth(), h + border.GetHeight());
	//float ratio = (float)h / w;
	Viewport(0, 0, w, h);

	MatrixMode(PROJECTION_MATRIX);
	LoadIdentity();
	Ortho(0, (float)w, (float)h, 0, -1, 1);

	MatrixMode(MODELVIEW_MATRIX);
	LoadIdentity();

	//glTranslatef(0, 0, -3);
}

void Window::OnLoad(){
	Graphics::Init();
}

void Window::OnUnload(){
	for (auto c : controls)
		delete c;
	controls.clear();
	downControl = moveControl = NULL;

	delete Model::shader;
	Model::shader = NULL;

	//delete Control::defaultFont;
	//Control::defaultFont = NULL;
	Font::Free();

	Graphics::Free();
}

void Window::UpdateFrame(){
	UpdateTime();
	OnUpdate(GetDeltaTime());
	int64_t t = GetMicroSecond();
	PushMatrix();
	OnDraw(DrawEventArgs(this));
	PopMatrix();
	int64_t t2 = GetMicroSecond();

	GLenum errorCode;
	//const GLubyte *errorString;
	while ((errorCode = glGetError()) != GL_NO_ERROR){
		//errorString = gluErrorString(errorCode);
		DEBUG_LOG("GL Error: %d\n", errorCode);
	}

	SwapBuffers();
	int64_t t3 = GetMicroSecond();
	//DEBUG_LOG("draw: %.3f, swap: %.3f, %d, %d, %d\n", (t2 - t) / 1000.0f, (t3 - t2) / 1000.0f, int(t % 100000), int(t2 % 100000), int(t3 % 100000));
}

SE_END
