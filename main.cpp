#include <SDL_config.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <vulkan/vulkan_core.h>
#define SDL_MAIN_HANDLED
#include "vulkan_util.hpp"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

using namespace vulkanDetails;

constexpr uint32_t WIDTH  = 800;
constexpr uint32_t HEIGHT = 600;

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Vulkan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    VulkanBase* singleton = VulkanBase::getInstance();
    singleton->initVulkan(window);
    SDL_Event e;
    bool      quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        // singleton->drawFrame();
    }
    singleton->cleanup();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
