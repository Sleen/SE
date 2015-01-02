#include "MyTime.h"

#if defined(LINUX) || defined(ANDROID)
extern "C"{
#	define __lint__
#	include <sys/time.h>
}
#endif

SE_BEGIN

int64_t GetMicroSecond(){
#ifdef WINDOWS
	static int64_t freq = 0;
	if(freq == 0){
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		freq = f.QuadPart;
	}
	LARGE_INTEGER perf;
	QueryPerformanceCounter(&perf);
	return perf.QuadPart * 1000000 / freq;
#elif defined(LINUX) || defined(ANDROID)
	struct timeval tv={0};
    gettimeofday(&tv, NULL);
	return tv.tv_sec*1000000 + tv.tv_usec;
#endif
}

SE_END
