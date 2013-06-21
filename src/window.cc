#include "window.h"

// ----------------------------
// Constructor
// ----------------------------
Window::Window(const WCHAR* className, const WCHAR* windowName) : mClassName (className), mWindowName (windowName)
{
	WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_OWNDC, Window::MsgRouter, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL,IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, mClassName, NULL};

	// Register window class
	RegisterClassEx(&wc);

	// Create application window
	mHwnd = CreateWindow(mClassName, mWindowName, WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, wc.hInstance, this);

	// Define desired pixel format
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,		// Flags
		PFD_TYPE_RGBA,													// Framebuffer type
		32,																// Framebuffer colordepth
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,																// Depthbuffer bits
		8,																// Stencilbuffer bits
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	// Get device context for window
	mHdc = GetDC(mHwnd);

	// Retrieve closest pixel format
	int pf = ChoosePixelFormat(mHdc, &pfd);

	// Set the pixel format
	SetPixelFormat(mHdc, pf, &pfd);

	// Obtain rendering context and make it current
	HGLRC rc = wglCreateContext(mHdc);
	wglMakeCurrent(mHdc, rc);

	// Initialize GLEW
	if(glewInit() != GLEW_OK)
		MessageBox(mHwnd, L"GLEW failed to initialize.", L"Error", MB_OK);

	// Check opengl support
	wchar_t buffer[100];
	swprintf(buffer, 100, L"Max OpenGL version supported: %S", glGetString(GL_VERSION));
	MessageBox(mHwnd, buffer, L"Version check", MB_OK);

	// Build triangle
	/*GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	static const GLfloat g_vertexBufferData[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
	};
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertexBufferData), g_vertexBufferData, GL_STATIC_DRAW);*/

	// Show window
	ShowWindow(mHwnd, SW_SHOWDEFAULT);
	UpdateWindow(mHwnd);

	// Enter message loop
	MSG msg = {0};
	while(msg.message != WM_QUIT) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// No messages waiting, start rendering
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/*glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDisableVertexAttribArray(0);*/

			SwapBuffers(mHdc);
			// End rendering
		}
	}

	UnregisterClass(mClassName, wc.hInstance);
}

// ----------------------------
// Destructor
// ----------------------------
Window::~Window()
{
}

// ----------------------------
// Window message router
// ----------------------------
LRESULT CALLBACK Window::MsgRouter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window* pWindow;

	if(msg == WM_NCCREATE)
	{
		SetWindowLongPtr(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT(lparam))->lpCreateParams));
	}
	
	// retrieve associated Window instance
	pWindow = (Window *)GetWindowLongPtr(hwnd, GWL_USERDATA);

	// call the window message handler
	return pWindow->MsgProc(hwnd, msg, wparam, lparam);
}

// ----------------------------
// Window message handler
// ----------------------------
LRESULT CALLBACK Window::MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_DESTROY:			// Window should be closed (i.e. by clicking on X or similar)
		PostQuitMessage(0);
		break;
	/*case WM_PAINT:				// Window needs repainting
		hdc = BeginPaint(mHwnd, &ps);
		EndPaint(mHwnd, &ps);
		break;*/
	case WM_KEYDOWN:			// A key was pressed
		switch(wparam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		}
	/*case WM_ERASEBKGND:
		break;*/
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}