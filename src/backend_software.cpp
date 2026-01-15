#include "backend_wrapper.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>

static SDL_Renderer* g_Renderer = nullptr;

bool InitSoftwareBackend(SDL_Window* window) {
    g_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (g_Renderer == nullptr) {
        SDL_Log("Error creating SDL_Renderer: %s", SDL_GetError());
        return false;
    }

    ImGui_ImplSDL2_InitForSDLRenderer(window, g_Renderer);
    ImGui_ImplSDLRenderer2_Init(g_Renderer);
    return true;
}

void ShutdownSoftwareBackend() {
    ImGui_ImplSDLRenderer2_Shutdown();
    if (g_Renderer) {
        SDL_DestroyRenderer(g_Renderer);
        g_Renderer = nullptr;
    }
}

void NewFrameSoftwareBackend() {
    ImGui_ImplSDLRenderer2_NewFrame();
}

void RenderSoftwareBackend(ImDrawData* draw_data) {
    ImGuiIO& io = ImGui::GetIO();
    SDL_RenderSetScale(g_Renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

    // Clear screen
    SDL_SetRenderDrawColor(g_Renderer, (Uint8)(0.45f * 255), (Uint8)(0.55f * 255), (Uint8)(0.60f * 255), (Uint8)(1.0f * 255));
    SDL_RenderClear(g_Renderer);

    ImGui_ImplSDLRenderer2_RenderDrawData(draw_data, g_Renderer);
    SDL_RenderPresent(g_Renderer);
}
