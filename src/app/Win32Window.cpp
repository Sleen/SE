#include "Window.h"
#include "Color.h"
#include "Bitmap.h"
#include "System.h"

#ifdef WINDOWS

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

SE_BEGIN

void Unit::InitDeviceInfo(){
	HDC hScreenDC = GetDC(NULL);
	widthInch = GetDeviceCaps(hScreenDC, HORZSIZE) / 25.4f;
	heightInch = GetDeviceCaps(hScreenDC, VERTSIZE) / 25.4f;
	widthPixel = GetDeviceCaps(hScreenDC, HORZRES);
	heightPixel = GetDeviceCaps(hScreenDC, VERTRES);
	screenDpi = sqrtf(widthPixel*heightPixel / (widthInch*heightInch)) * 1.3f;
}

Window* Window::FindWindowByHwnd(HWND hwnd){
	for (Window* w : Window::windows)
	if (w->hwnd == hwnd)
		return w;
	return NULL;
}

LRESULT CALLBACK WndProc_Static(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
	if (hwnd){
		Window* w = Window::FindWindowByHwnd(hwnd);
		if (w)
			return w->WndProc(hwnd, msg, wp, lp);
	}
	
	return DefWindowProc(hwnd, msg, wp, lp);
}

Window::Window(){
	visibled = false;
	//Create();
}

bool Window::CreateImpl(){
	//MSG msg;
	WNDCLASSEX wc;
	char cn[128];
	sprintf(cn, "%s - %d", title.c_str(), ++id);
	
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc_Static;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = cn;

	int titleHeight = GetSystemMetrics(SM_CYCAPTION);
	int borderWidth = GetSystemMetrics(SM_CXFRAME);
	int borderHeight = GetSystemMetrics(SM_CYFRAME);

	if (System::Major >= 7){
		borderWidth *= 2;
		borderHeight *= 2;
	}

	SetBorder(Padding(borderWidth, titleHeight + borderHeight, borderWidth, borderHeight));
	SetBorderColor(Color::Transparent);

	ASSERT(RegisterClassEx(&wc), "register class failure");

	ASSERT((hwnd = CreateWindow(cn, title.c_str(),
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		(int)bounds.X(), (int)bounds.Y(), (int)bounds.GetWidth(), (int)bounds.GetHeight(),
		NULL, NULL, wc.hInstance, NULL)), "Can not create window");

	static PIXELFORMATDESCRIPTOR pfdWnd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Structure size.
		1,                             // Structure version number.
		PFD_DRAW_TO_WINDOW |           // Property flags.
		PFD_SUPPORT_OPENGL |
		//PFD_SUPPORT_COMPOSITION |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,                            // 24-bit color.
		0, 0, 0, 0, 0, 0,              // Not concerned with these.
		0, 0, 0, 0, 0, 0, 0,           // No alpha or accum buffer.
		32,                            // 32-bit depth buffer.
		32, 0,                          // No stencil or aux buffer.
		PFD_MAIN_PLANE,                // Main layer type.
		0,                             // Reserved.
		0, 0, 0                        // Unsupported.
	};
	hdc = GetDC(hwnd);
	
	int pixelformat = ChoosePixelFormat(hdc, &pfdWnd);
	SetPixelFormat(hdc, pixelformat, &pfdWnd);
	rc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, rc);
	//SE::Unit::Init();
	return true;
}

Window::~Window(){ }

void Window::Run(){
	Create();

	ShowWindow(hwnd, SW_SHOW);

	OnLoad();

	UpdateWindow(hwnd);

	visibled = true;

	MSG         msg;
	
	running = true;
	while (running)
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			running = false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else{
		//SendMessage(hwnd, WM_PAINT, 0, 0);
	}

	OnUnload();
}

void Window::SwapBuffers(){
	::SwapBuffers(hdc);
}

void Window::Show(){
	ShowWindow(hwnd, SW_SHOW);
}

void Window::Hide(){
	ShowWindow(hwnd, SW_HIDE);
}

void Window::Close(){
	PostQuitMessage(0);
}

void Window::SetLocation(const Vector2& loc){
	if (GetLocation() != loc){
		Control::SetLocation(loc);
		if (hwnd)
			SetWindowPos(hwnd, 0, (int)bounds.X(), (int)bounds.Y(), (int)bounds.GetWidth(), (int)bounds.GetHeight(), 0);
	}
}

void Window::SetSize(const Vector2& size){
	if (GetSize() != size){
		Control::SetSize(size);
		if (hwnd)
			SetWindowPos(hwnd, 0, (int)bounds.X(), (int)bounds.Y(), (int)bounds.GetWidth(), (int)bounds.GetHeight(), 0);
	}
}

void Window::SetTitle(const string& title){
	this->title = title;
	if (hwnd)
		SetWindowText(hwnd, title.c_str());
}

MouseButton GetMouseButton(int w){
	if (w & MK_LBUTTON)
		return MouseButton::Left;
	if (w & MK_RBUTTON)
		return MouseButton::Right;
	if (w & MK_MBUTTON)
		return MouseButton::Middle;

	return MouseButton::None;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp){
	switch (msg)
	{
	case WM_DESTROY:
		cout << GetTitle() << " destroyed.." << endl;
		wglMakeCurrent(hdc, NULL);  // 释放与m_hDC 对应的 RC
		wglDeleteContext(rc);		// 删除 RC

		if (this == Window::startupWindow){
			//OnStop();
			PostQuitMessage(0);
		}
		//UnregisterClass(className.Data(), GetModuleHandle(NULL));
		//OnClose();
		//this->~Window();

		break;
	case WM_CREATE:
		SendMessage(hwnd, WM_PAINT, 0, 0);
		break;
	case WM_PAINT:
		if (IsVisibled()){
			wglMakeCurrent(hdc, rc);
			UpdateFrame();
		}
		break;
    case WM_SIZE:
		//wglMakeCurrent(hdc, rc);
        OnResize(Vector2(LOWORD(lp), HIWORD(lp)));
        break;
    case WM_SIZING:
    {
        auto rect = (LPRECT)lp;
        int width = rect->right - rect->left - border.GetWidth();
        int height = rect->bottom - rect->top - border.GetHeight();
        cout << "Resizing: " << width << ", " << height << endl;
        OnResize(Vector2(width, height));
        break;
    }
	case WM_ERASEBKGND:
		//wprintf(L"WM_ERASEBKGND\n");
		break;
		//case WM_SYSCOMMAND:
		//	DefWindowProc(hwnd, iMsg, wp, lp);
		//	break;
	case WM_KEYDOWN:
		OnKeyEvent(KeyEventArgs(wp, KeyAction::Down));
		break;
	case WM_KEYUP:
		OnKeyEvent(KeyEventArgs(wp, KeyAction::Up));
		break;
	case WM_CHAR:
		OnKeyEvent(KeyEventArgs(wp, KeyAction::Press));
		break;
	case WM_MOUSEMOVE:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), GetMouseButton(wp), MouseAction::Move));
		if (!trackingMouseLeave){		// 追踪鼠标离开事件
			TRACKMOUSEEVENT csTME;
			csTME.cbSize = sizeof (csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = hwnd;
			TrackMouseEvent(&csTME);
			trackingMouseLeave = true;
		}
		break;
	case WM_MOUSELEAVE:
		trackingMouseLeave = false;
		OnMouseLeave();
		break;
	case WM_LBUTTONDOWN:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Left, MouseAction::Down));
		break;
	case WM_RBUTTONDOWN:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Right, MouseAction::Down));
		break;
	case WM_MBUTTONDOWN:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Middle, MouseAction::Down));
		break;
	case WM_LBUTTONUP:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Left, MouseAction::Up));
		break;
	case WM_RBUTTONUP:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Right, MouseAction::Up));
		break;
	case WM_MBUTTONUP:
		OnMouseEvent(MouseEventArgs((short)LOWORD(lp),
			(short)HIWORD(lp), MouseButton::Middle, MouseAction::Up));
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

int Window::GetScreenWidth(){
	return GetSystemMetrics(SM_CXSCREEN);
}

int Window::GetScreenHeight(){
	return GetSystemMetrics(SM_CYSCREEN);
}

SE_END

#endif
