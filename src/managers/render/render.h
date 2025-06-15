


#if VULKAN
#include "render_vulkan.h"
#endif





struct Renderer
{

	#if VULKAN
	Data data;
	#if IMGUI
	ImGuiData myImgui;
	#endif
	#endif

};
