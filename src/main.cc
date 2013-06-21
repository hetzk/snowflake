/*

	Main.

*/

#include "xwindow.h"

using std::cout;
using std::exception;

int main(int argc, char *argv[]) {

	// Create the window
	try {
		XWindow window("X OpenGL window", 640, 480);
	} catch(exception const &e) {
		cout << e.what();
	}

	return 0;
}