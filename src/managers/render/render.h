


struct InstancedData {
	mat4 modelMatrix;
};



#if VULKAN
#include "render_vulkan.h"
#endif





struct Renderer
{

	#if VULKAN
	Data data;
	#endif

};