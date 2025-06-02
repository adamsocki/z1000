


void InitTime(Time* time)
{
	// Time* time = &zaynMem->time;
#ifdef WIN32
	QueryPerformanceFrequency(&time->mTime.systemFrequency);
	QueryPerformanceCounter(&time->mTime.systemTime);

	time->mTime.startSystemTime = time->mTime.systemTime;
	//SeedRand(time->mTime.startSystemTime.QuadPart);

	time->systemTime = (real32)time->mTime.systemTime.QuadPart;
#elif __APPLE__

	mach_timebase_info_data_t timebaseInfo;
	mach_timebase_info(&timebaseInfo);

	time->mTime.conversionFactor = (double)timebaseInfo.numer / (double)timebaseInfo.denom / 1000000000.0;

	time->mTime.systemTime.QuadPart = mach_absolute_time();
	time->mTime.startSystemTime.QuadPart = time->mTime.systemTime.QuadPart;

	time->mTime.systemFrequency.QuadPart = 1;

	time->systemTime = (real32)time->mTime.systemTime.QuadPart;

#endif

	time->startTime = 0.0f;
	std::cout << "Time Init'd" << std::endl;
}



void UpdateTime(Zayn* zaynMem) {

	Time* time = &zaynMem->time;
	time->mTime.prevSystemTime = time->mTime.systemTime;
	//LARGE_INTEGER
#ifdef WIN32
	int32 error = QueryPerformanceCounter(&time->mTime.systemTime);

	time->deltaTime = ((real64)time->mTime.systemTime.QuadPart - (real64)time->mTime.prevSystemTime.QuadPart) / (real64)time->mTime.systemFrequency.QuadPart;
#elif __APPLE__
	time->mTime.systemTime.QuadPart = mach_absolute_time();
	time->deltaTime = ((real64)time->mTime.systemTime.QuadPart - (real64)time->mTime.prevSystemTime.QuadPart) * time->mTime.conversionFactor;
#endif
	time->totalTime += time->deltaTime;
	time->frameCount = time->frameCount + 1;
	//std::cout << "framecnf: " << time->frameCount << std::endl;
	//        for (int i =0; i < 123445; i++)
	//        {
	//            int32 a = 0;
	//            a++;
	//            a--;
	//        }

	time->fpsTimer += time->deltaTime;


	//        if (time->fpsTimer > 0.1f)
	//        {
	//
	//            real64 avgFPS = time->frameCount / time->fpsTimer;
	//            std::cout << avgFPS << std::endl;
	//
	//            time->fpsTimer = 0.0f;
	//            time->frameCount = 0;
	//
	//        }

}