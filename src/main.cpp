//
//  main.cpp
//  z56
//
//  Created by Adam Socki on 6/1/25.
//

#include <iostream>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <set>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>



#include "zayn.cpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    
    
    Zayn zaynData = {};
    
    if (!glfwInit())
    {
        return -1;
    }
    
    InitZayn(&zaynData);
    
    while (!glfwWindowShouldClose(zaynData.windowManager.glfwWindow)) {
        UpdateZayn(&zaynData);
        //     UpdateEngine(&engine);
    }
    
    ShutdownZayn(&zaynData);
    glfwTerminate();
    
    return 0;
}
