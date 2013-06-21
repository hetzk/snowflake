/*

	X window class with OpenGL.

*/

#ifndef SNOWFLAKE_XWINDOW_H_
#define SNOWFLAKE_XWINDOW_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <iostream>
#include <exception>
#include "ros.h"

class XWindow {
private:
	Display *display_;
	Window root_window_;
	XVisualInfo *visual_info_;
	Colormap colormap_;
	XSetWindowAttributes set_window_attributes_;
	Window window_;
	GLXContext context_;
	XWindowAttributes window_attributes_;
	XEvent event_;
	KeySym key_;
	char key_text_[16];
	bool active_;
	Ros ros_;
	int height_;
	int width_;
public:
	XWindow(const char* windowName, int width, int height);
	~XWindow() {};
};

class XServException : public std::exception {
	virtual const char *what() const throw() {
		return "Bad: Cannot connect to X server";
	};
};


#endif