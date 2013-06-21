#include "xwindow.h"

using std::cout;
using std::endl;

XWindow::XWindow(const char* window_name, int width, int height) : active_(true) {
	width_ = width;
	height_ = height;

	display_ = XOpenDisplay(NULL);

	if(display_ == NULL) {
		throw XServException();
	}

	root_window_ = DefaultRootWindow(display_);

	GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	visual_info_ = glXChooseVisual(display_, 0, attributes);

	colormap_ = XCreateColormap(display_, root_window_, visual_info_->visual, AllocNone);

	set_window_attributes_.colormap = colormap_;
	set_window_attributes_.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

	window_ = XCreateWindow(display_, root_window_, 0, 0, width_, height_, 0, visual_info_->depth, InputOutput, visual_info_->visual, CWColormap | CWEventMask, &set_window_attributes_);

	XMapWindow(display_, window_);
	XStoreName(display_, window_, window_name);

	context_ = glXCreateContext(display_, visual_info_, NULL, GL_TRUE);
	glXMakeCurrent(display_, window_, context_);

	XGetWindowAttributes(display_, window_, &window_attributes_);
	glViewport(0, 0, window_attributes_.width, window_attributes_.height);

	ros_.Init();
	ros_.LoadModel("../extra/bone_anim_2.rime");
bool ff = false;
	// Start the event loop
	while(active_) {
		if(XCheckWindowEvent(display_, window_, KeyPressMask, &event_)) {
			if(XLookupString(&event_.xkey, key_text_, 16, &key_, 0) > 0) {

				// Process keypress
				ros_.ProcessInput((const int)key_);

				switch(key_) {
					case XK_Escape:
						glXMakeCurrent(display_, None, NULL);
						glXDestroyContext(display_, context_);
						XDestroyWindow(display_, window_);
						XCloseDisplay(display_);
						active_ = false;
						break;
				}
			}
		} else if(XCheckWindowEvent(display_, window_, ExposureMask, &event_)) {
			// Hide cursor
			Cursor no_ptr;
			Pixmap bm_no;
			XColor black, dummy;
			Colormap colormap;
			static char no_data[] = {0, 0, 0, 0, 0, 0, 0 ,0};

			colormap = DefaultColormap(display_, DefaultScreen(display_));
			XAllocNamedColor(display_, colormap, "black", &black, &dummy);
			bm_no = XCreateBitmapFromData(display_, window_, no_data, 8, 8);
			no_ptr = XCreatePixmapCursor(display_, bm_no, bm_no, &black, &black, 0, 0);

			XDefineCursor(display_, window_, no_ptr);
			XFreeCursor(display_, no_ptr);

			// Position cursor
			XWarpPointer(display_, None, window_, 0, 0, 0, 0, width_/2, height_/2);
			XFlush(display_);

			// Render
			ros_.Render();
			glXSwapBuffers(display_, window_);
			ff = true;
		} else if(XCheckWindowEvent(display_, window_, ButtonPressMask, &event_)) {
			switch(event_.xbutton.button) {
				case 1:
					cout << "btn click " << event_.xbutton.button << endl;
					break;
			}
		} else {
			// Calculate camera movement
			// x = theta, y = phi
			int x = 0, y = 0;
			Window root, child;
			int x1 = width_/2, y1 = height_/2;
			unsigned int mask_return;
			XQueryPointer(display_, window_, &root, &child, &x1, &y1, &x, &y, &mask_return);
			
			int moved_x = x - width_/2;
			int moved_y = y - height_/2;
			XWarpPointer(display_, None, window_, 0, 0, 0, 0, width_/2, height_/2);
			XFlush(display_);

			// Update camera
			if(moved_x != 0 || moved_y != 0)
				if(ff) ros_.UpdateCamera(moved_x, moved_y);
			else cout << "!" << moved_x << " " << moved_y<< endl;

			ros_.Render();

			glXSwapBuffers(display_, window_);
		}
	}
}