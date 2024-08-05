#include "vulkan_util.hpp"
#include <SDL_vulkan.h>
#include <vector>
// Add this line to include the necessary header file

namespace vulkan{
    vulkanBase* vulkanBase::getInstance()
    {
        return singleton;
    }

    void vulkanBase::createInstance(SDL_Window* window)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Vulkan";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "NinaEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
    
        uint32_t sdlExtensionCount = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr)) {
            throw std::runtime_error("failed to get Vulkan extensions count!");
        }
        std::vector<const char*> sdlExtensions(sdlExtensionCount);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions.data())) {
            throw std::runtime_error("failed to get Vulkan extensions!");
        }

        createInfo.enabledExtensionCount = sdlExtensionCount;
        createInfo.ppEnabledExtensionNames = sdlExtensions.data();
        createInfo.enabledLayerCount = 0;
        if(VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }
    
    void vulkanBase::initVulkan(SDL_Window* window)
    {
        createInstance(window);
    }
}