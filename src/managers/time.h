



#ifdef WIN32
#include <windows.h>


#elif defined(__APPLE__)
// Define LARGE_INTEGER equivalent for Apple

#include <mach/mach_time.h>
#include <CoreServices/CoreServices.h>
#include <unistd.h>
typedef struct {
    uint64_t QuadPart;
} LARGE_INTEGER;

#endif


struct MachineTime
{
    LARGE_INTEGER startSystemTime;
    LARGE_INTEGER systemTime;
    LARGE_INTEGER systemFrequency;
    LARGE_INTEGER prevSystemTime;

#ifdef __APPLE__
    double conversionFactor;
#endif

};


struct Time
{
	MachineTime mTime;
	
	real32 startTime;
	real32 systemTime;
	real32 prevSystemTime;
	
	real32 engineTime;
	real32 deltaTime;
	real32 totalTime;
	
	int32 frameCount;
	real32 fpsTimer;
};