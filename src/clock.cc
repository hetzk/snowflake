#ifdef __gnu_linux__

#include "clock.h"

Clock::Clock() : delta_(0) {
	last_slice_ = GetTime();
	Slice();
}

// Returns the current time in milliseconds from system start
unsigned long Clock::GetTime() {
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts_);
	return ts_.tv_sec * 1000 + ts_.tv_nsec / 1000000;
}

#elif defined _WIN32

Clock::Clock() : delta_(0) {
	timeBeginPeriod(1);
	last_slice_ = GetTime();
	Slice();
}

Clock::~Clock() {
	timeEndPeriod();
}

unsigned long Clock::GetTime() {
	return timeGetTime();
}

#endif

// Reset time delta.
void Clock::Slice() {
	unsigned long now = GetTime();
	if(last_slice_ != 0)
		delta_ = now - last_slice_;
	last_slice_ = now;
}

// Get the time (in ms) from last use of Slice().
unsigned long Clock::GetDelta() {
	return delta_;
}