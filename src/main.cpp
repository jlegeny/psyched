#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "backend_wrapper.h"

// Main code
int main(int argc, char** argv)
{
    bool use_software_renderer = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--fallback" || arg == "--software-renderer") {
            use_software_renderer = true;
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

#if defined(__linux__) && !defined(IMGUI_APP_NO_VULKAN)
    if (!use_software_renderer) {
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_VULKAN);
    }
#elif defined(__APPLE__) && !defined(IMGUI_APP_NO_METAL)
    if (!use_software_renderer) {
        window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_METAL);
    }
#endif

    SDL_Window* window = SDL_CreateWindow("Dear ImGui Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    // Init Backend
    bool backend_success = false;
    BackendType active_backend = BackendType::Software; // Default to software if hardware fails?

    if (use_software_renderer) {
        backend_success = InitSoftwareBackend(window);
        active_backend = BackendType::Software;
    } else {
#if (defined(__linux__) && !defined(IMGUI_APP_NO_VULKAN)) || (defined(__APPLE__) && !defined(IMGUI_APP_NO_METAL))
        // Try Hardware first
        if (InitHardwareBackend(window)) {
            backend_success = true;
            active_backend = BackendType::Hardware;
        } else {
            printf("Hardware backend failed, falling back to Software/SDL_Renderer.\n");
            // If hardware failed, we might need to recreate the window if flags were wrong?
            // SDL_WINDOW_VULKAN/METAL are usually just hints for surface creation,
            // but for SDL_Renderer we might need to remove them or just proceed.
            // SDL_CreateRenderer usually works even on Vulkan/Metal windows if they support OpenGL/Software.
            // But let's try.
            backend_success = InitSoftwareBackend(window);
            active_backend = BackendType::Software;
        }
#else
        // No hardware backend compiled in
        backend_success = InitSoftwareBackend(window);
        active_backend = BackendType::Software;
#endif
    }

    if (!backend_success) {
        printf("Failed to initialize any backend.\n");
        return 1;
    }

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Handle Resize
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        if (active_backend == BackendType::Hardware) {
             ResizeHardwareBackend(width, height);
        }

        // New Frame
        if (active_backend == BackendType::Software) {
            NewFrameSoftwareBackend();
        } else {
            NewFrameHardwareBackend();
        }

        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // UI
        {
            ImGui::Begin("ImGui Application");
            ImGui::Text("Backend: %s", (active_backend == BackendType::Hardware) ? "Hardware (Vulkan/Metal)" : "Software (SDL_Renderer)");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();

        if (active_backend == BackendType::Software) {
            RenderSoftwareBackend(ImGui::GetDrawData());
        } else {
            RenderHardwareBackend(ImGui::GetDrawData());
        }
    }

    if (active_backend == BackendType::Software) {
        ShutdownSoftwareBackend();
    } else {
        ShutdownHardwareBackend();
    }

    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
