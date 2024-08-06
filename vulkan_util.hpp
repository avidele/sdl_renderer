#pragma once
#include "vulkan/vulkan.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL_video.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vulkanDetails
{
    class VulkanBase
    {
    public:
        static VulkanBase* getInstance();
        void               createInstance(SDL_Window* window);

        void        initVulkan(SDL_Window* window);
        static void printExtensionProperties();
        void        cleanup();

    private:
        VulkanBase() = default;
        static VulkanBase* m_singleton;
        VkInstance         instance;
    };
} // namespace vulkanDetails