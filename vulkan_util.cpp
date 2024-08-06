#include "vulkan_util.hpp"
#include <SDL_vulkan.h>
#include <vector>
// Add this line to include the necessary header file

namespace vulkanDetails
{

    VulkanBase* VulkanBase::m_singleton = nullptr;
    VulkanBase* VulkanBase::getInstance()
    {
        if (m_singleton == nullptr)
        {
            m_singleton = new VulkanBase();
        }
        return m_singleton;
    }

    void VulkanBase::createInstance(SDL_Window* window)
    {
        VkApplicationInfo app_info {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = "Hello Vulkan";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = "NinaEngine";
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        uint32_t sdl_extension_count = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdl_extension_count, nullptr))
        {
            std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Failed to get Vulkan extensions count!");
        }

        std::vector<const char*> sdl_extensions(sdl_extension_count);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &sdl_extension_count, sdl_extensions.data()))
        {
            std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Failed to get Vulkan extensions!");
        }

        std::cout << "SDL Vulkan extensions Count: " << sdl_extension_count << std::endl;
        for (const auto& extension : sdl_extensions)
        {
            std::cout << "\t" << extension << std::endl;
        }

        create_info.enabledExtensionCount   = sdl_extension_count;
        create_info.ppEnabledExtensionNames = sdl_extensions.data();
        create_info.enabledLayerCount       = 0;
        if (VkResult result = vkCreateInstance(&create_info, nullptr, &instance); result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
        printExtensionProperties();
    }

    void VulkanBase::initVulkan(SDL_Window* window) { createInstance(window); }

    void VulkanBase::printExtensionProperties()
    {
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
        std::cout << "available extensions:\n";
        for (const auto& extension : extensions)
        {
            std::cout << "\t" << extension.extensionName << std::endl;
        }
    }

    void VulkanBase::cleanup() { vkDestroyInstance(instance, nullptr); }
} // namespace vulkanDetails