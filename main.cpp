#include <SDL_config.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <vulkan/vulkan_core.h>
#define SDL_MAIN_HANDLED
#include "vulkan_util.hpp"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <vulkan/vulkan.h>

using namespace vulkanDetails;

constexpr uint32_t WIDTH  = 800;
constexpr uint32_t HEIGHT = 600;

int main()
{

    VulkanBase* singleton = VulkanBase::getInstance();
    singleton->initWindow();
    singleton->initVulkan();
    singleton->mainLoop();
    singleton->cleanup();

    return 0;
}
