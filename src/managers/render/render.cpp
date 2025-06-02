

#if VULKAN
#define MAX_FRAMES_IN_FLIGHT 2
#include "render_vulkan.cpp"


#elif  OPENGL

#include "render_opengl.cpp"


#elif  METAL

#include "render_metal.cpp"


#endif





void InitRender(Zayn* zaynMem)
{

	#ifdef VULKAN
	InitRender_Vulkan(&zaynMem->renderer, &zaynMem->windowManager);
	#elif  OPENGL
	InitRender_OpenGL();
	#elif  METAL
	InitRender_Metal();
	#endif

	std::cout << "init render()" << std::endl;
	
}
