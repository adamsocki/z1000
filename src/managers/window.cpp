//
// Created by Adam Socki on 5/30/25.
//


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //auto zaynMem = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    //zaynMem->vulkan.vkFramebufferResized = true;

}


void InitWindow(WindowManager* windowManager, InputManager* inputManager, vec2 windowSize, const char* windowName) {

#ifdef VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

#if OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    windowManager->glfwWindow = glfwCreateWindow(windowSize.x, windowSize.y, windowName, NULL, NULL);


#ifdef OPENGL
    glfwMakeContextCurrent(windowManager->glfwWindow);
#endif

    glfwSetWindowUserPointer(windowManager->glfwWindow, inputManager);
    glfwSetFramebufferSizeCallback(windowManager->glfwWindow, framebuffer_size_callback);

}


