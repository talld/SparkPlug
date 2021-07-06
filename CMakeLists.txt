cmake_minimum_required(VERSION 3.19)
project(SparkPlug)

add_executable(SparkPlug
        main.cpp
        Engine/Utilities/FileUtils.cpp
        Engine/Renderer/Renderer.cpp
        Engine/Renderer/Allocator/Allocator.cpp
        Engine/Renderer/Window/Window.cpp
        Engine/Renderer/Instance/Instance.cpp
        Engine/Renderer/Surface/Surface.cpp
        Engine/Renderer/PhysicalDevice/PhysicalDevice.cpp
        Engine/Renderer/LogicalDevice/LogicalDevice.cpp
        Engine/Renderer/Swapchain/Swapchain.cpp
        Engine/Renderer/ImageView/ImageView.cpp
        )

set(CMAKE_CXX_STANDARD 20)


find_package(glm REQUIRED)
#target_link_libraries(SparkPlug glm)

find_package(glfw3 REQUIRED)
target_link_libraries(SparkPlug glfw)
target_include_directories (SparkPlug PUBLIC ${GLFW_INCLUDE_DIRS})

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
target_include_directories (SparkPlug PUBLIC ${VULKAN_INCLUDE_DIRS})
