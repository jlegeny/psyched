#include "backend_wrapper.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_metal.h"
#include <SDL.h>
#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

static id<MTLDevice> g_Device = nil;
static id<MTLCommandQueue> g_CommandQueue = nil;
static SDL_Window* g_Window = nullptr;
static SDL_MetalView g_MetalView = nullptr;

bool InitHardwareBackend(SDL_Window* window) {
    g_Window = window;
    g_Device = MTLCreateSystemDefaultDevice();
    g_CommandQueue = [g_Device newCommandQueue];

    g_MetalView = SDL_Metal_CreateView(window);

    ImGui_ImplSDL2_InitForOther(window);
    ImGui_ImplMetal_Init(g_Device);
    return true;
}

void ShutdownHardwareBackend() {
    ImGui_ImplMetal_Shutdown();
    if (g_MetalView) {
        SDL_Metal_DestroyView(g_MetalView);
        g_MetalView = nullptr;
    }
    g_CommandQueue = nil;
    g_Device = nil;
    g_Window = nullptr;
}

void NewFrameHardwareBackend() {
    ImGui_ImplMetal_NewFrame(nil);
}

void ResizeHardwareBackend(int width, int height) {
    // Metal handles resize via the drawable automatically usually.
}

void RenderHardwareBackend(ImDrawData* draw_data) {
    @autoreleasepool {
        void* layer = SDL_Metal_GetLayer(g_MetalView);
        CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)layer;
        id<CAMetalDrawable> drawable = [metalLayer nextDrawable];

        if (drawable) {
            id<MTLCommandBuffer> commandBuffer = [g_CommandQueue commandBuffer];
            MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.45, 0.55, 0.60, 1.00);
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

            ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);

            [renderEncoder endEncoding];
            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
        }
    }
}
