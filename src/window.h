// *****************************************************************
//
// C++ Win32 window class.
// 
// *****************************************************************

#ifndef SNOWFLAKE_WINDOW_H_
#define SNOWFLAKE_WINDOW_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include "include/gl/glew.h"
#include "include/gl/wglew.h"
#include "include/gl/glext.h"
#include "include/gl/wglext.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "lib/gl/glew32.lib")

using std::string;

class Window
{
private:
	HDC mHdc;
	HWND mHwnd;
	const WCHAR* mClassName;
	const WCHAR* mWindowName;
	int width_;
	int height_;
public:
	void setHwnd(const HWND hwnd) { mHwnd = hwnd; };
	HWND getHwnd() const { return mHwnd; };
	Window(const WCHAR* className, const WCHAR* windowName);
	~Window();
	LRESULT CALLBACK MsgProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK MsgRouter(HWND, UINT, WPARAM, LPARAM);
};

#endif