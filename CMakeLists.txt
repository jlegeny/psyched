cmake_minimum_required(VERSION 3.20)
project(imgui_app LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include directories
include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)
include_directories(src)

# ImGui Source Files
set(IMGUI_SOURCES
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

# Application Sources
set(APP_SOURCES
    src/main.cpp
    src/backend_software.cpp
)

# Platform Specific Configurations
if(APPLE)
    enable_language(OBJCXX)

    # macOS - Metal
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)

    list(APPEND IMGUI_SOURCES third_party/imgui/backends/imgui_impl_metal.mm)
    list(APPEND APP_SOURCES src/backend_metal.mm)

    set(PLATFORM_LIBS ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
    add_compile_definitions(IMGUI_IMPL_METAL_CPP)

elseif(UNIX AND NOT APPLE)
    # Linux - Vulkan
    find_package(Vulkan REQUIRED)
    include_directories(${Vulkan_INCLUDE_DIRS})

    list(APPEND IMGUI_SOURCES third_party/imgui/backends/imgui_impl_vulkan.cpp)
    list(APPEND APP_SOURCES src/backend_vulkan.cpp)

    set(PLATFORM_LIBS ${Vulkan_LIBRARIES})
endif()

# Create Executable
add_executable(imgui_app ${APP_SOURCES} ${IMGUI_SOURCES})

# Link Libraries
target_link_libraries(imgui_app PRIVATE ${SDL2_LIBRARIES} ${PLATFORM_LIBS})

# Standard linking for dl, pthread on Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(imgui_app PRIVATE dl pthread)
endif()
