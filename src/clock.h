/*

	Timekeeping.
	Multi.

*/

#ifdef __gnu_linux__

#include <time.h>

class Clock {
public:
	Clock();
	unsigned long GetTime();
	void Slice();
	unsigned long GetDelta();
private:
	timespec ts_;
	unsigned long last_slice_;
	unsigned long delta_;
};

#elif defined _WIN32

#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

class Clock {
public:
	Clock();
	~Clock();
	unsigned long GetTime();
	void Slice();
	unsigned long GetDelta();
private:
	unsigned long last_slice_;
	unsigned long delta_;
};

#endif