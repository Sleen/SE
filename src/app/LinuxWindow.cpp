#include "Window.h"

#if defined(LINUX)

SE_BEGIN

int Window::id = 0;

Window::Window(){
	Create();
}

Window::~Window(){}

void Window::Create(){
    display = XOpenDisplay(NULL);
	window = XCreateSimpleWindow(display, RootWindow(display, 0),
		(int)location.X(), (int)location.Y(), (int)size.X(), (int)size.Y(), 1, WhitePixel(display, 0), BlackPixel(display, 0));
	XSelectInput(display, window, ExposureMask|ButtonPressMask);
	XMapWindow(display, window);
	XFlush(display);
	//XSetWindowAttributes swa;
	//XVisualInfo *vi, tmp;
	//XSizeHints sh;
	//int n;

	//cout<<2;
	//vi = XGetVisualInfo(sDisplay, VisualIDMask, &tmp, &n);
	//swa.colormap = XCreateColormap(sDisplay,
	//                               RootWindow(sDisplay, vi->screen),
	//                               vi->visual, AllocNone);
	//sh.flags = PMinSize | PMaxSize;
	//sh.min_width = sh.max_width = (int)size.X();
	//sh.min_height = sh.max_height = (int)size.Y();
	//swa.border_pixel = 0;
	//swa.event_mask = ExposureMask | StructureNotifyMask |
	//                 KeyPressMask | ButtonPressMask | ButtonReleaseMask;
	//sWindow = XCreateWindow(sDisplay, RootWindow(sDisplay, vi->screen),
	//                        (int)location.X(), (int)location.Y(), (int)size.X(), (int)size.Y(),
	 //                       0, vi->depth, InputOutput, vi->visual,
	 //                       CWBorderPixel | CWColormap | CWEventMask,
	 //                       &swa);
	/*XMapWindow(sDisplay, sWindow);
	cout<<3;
	XSetStandardProperties(sDisplay, sWindow, title, title,
	                       None, NULL, NULL, NULL);*/
}

void Window::Run(){
	running = true;
	while (running)
    {
        //struct timeval timeNow;

        while (XPending(display))
        {
            XEvent ev;
            XNextEvent(display, &ev);
            switch (ev.type)
            {
            case KeyPress:
                {
                    unsigned int keycode, keysym;
                    keycode = ((XKeyEvent *)&ev)->keycode;
                    keysym = XKeycodeToKeysym(display, keycode, 0);
                    if (keysym == XK_Return || keysym == XK_Escape)
                        running = false;
                }
                break;
            }
        }

        if (running)
        {
            //gettimeofday(&timeNow, NULL);
            //appRender(timeNow.tv_sec * 1000 + timeNow.tv_usec / 1000,
            //          sWindowWidth, sWindowHeight);
            //checkGLErrors();
            //eglSwapBuffers(sEglDisplay, sEglSurface);
            //checkEGLErrors();
        }
    }
}

SE_END

#endif

