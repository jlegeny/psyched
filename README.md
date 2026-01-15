# ImGui Application

This project initializes a Dear ImGui window using C++20 and CMake. It supports Linux (Vulkan) and macOS (Metal), with a software fallback (SDL_Renderer) for environments without hardware acceleration (e.g., VMs).

## Prerequisites

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git libsdl2-dev libvulkan-dev vulkan-headers glslang-tools
```

### macOS
Install dependencies via Homebrew:
```bash
brew install cmake sdl2
```
*Note: Xcode command line tools are also required.*

## Building

1.  Clone the repository and submodules:
    ```bash
    git clone --recursive <repository-url>
    cd <repository-folder>
    ```

2.  Create a build directory and configure with CMake:
    ```bash
    cmake -S . -B build
    ```

3.  Build the project:
    ```bash
    cmake --build build
    ```

## Running

Run the executable found in the `build` directory.

### Standard (Hardware Acceleration)
*   **Linux**: Uses Vulkan.
*   **macOS**: Uses Metal.

```bash
./build/imgui_app
```

### Fallback (Software / OpenGL)
If hardware acceleration is not available or fails, use the fallback mode:

```bash
./build/imgui_app --fallback
```
or
```bash
./build/imgui_app --software-renderer
```
