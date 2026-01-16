#pragma once
#include <SDL.h>
#include "imgui.h"

enum class BackendType {
    Software,
    Hardware // Vulkan on Linux, Metal on macOS
};

// Software Backend (SDL_Renderer) - Available on all platforms
bool InitSoftwareBackend(SDL_Window* window);
void ShutdownSoftwareBackend();
void NewFrameSoftwareBackend();
void RenderSoftwareBackend(ImDrawData* draw_data);

// Hardware Backend (Vulkan on Linux, Metal on macOS)
bool InitHardwareBackend(SDL_Window* window);
void ShutdownHardwareBackend();
void NewFrameHardwareBackend();
void RenderHardwareBackend(ImDrawData* draw_data);
void ResizeHardwareBackend(int width, int height);
