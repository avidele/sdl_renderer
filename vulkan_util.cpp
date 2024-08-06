#include "vulkan_util.hpp"
#include <SDL_vulkan.h>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>
// Add this line to include the necessary header file

namespace vulkanDetails
{
    std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation",
    };
#ifdef NOEBUG
    constexpr bool enable_validation_layers = false;
#else
    constexpr bool enable_validation_layers = true;
#endif
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

        auto extensions                     = getRequiredExtensions(window);
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

        if (VkResult result = vkCreateInstance(&create_info, nullptr, &instance); result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
        if (enable_validation_layers && !checkValidationLayerSupport(validation_layers))
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        if (enable_validation_layers)
        {
            create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
        }
    }

    void VulkanBase::initVulkan(SDL_Window* window)
    {
        createInstance(window);
        setupDebugMessenger();
    }

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

    void VulkanBase::cleanup()
    {

        if (enable_validation_layers)
        {
            destroyDebugUtilsMessengerExt(instance, callback, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }

    bool VulkanBase::checkValidationLayerSupport(std::vector<const char*>& validation_layers)
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
        for (const char* layer_name : validation_layers)
        {
            bool found_layer = false;
            for (const auto& layer_properties : available_layers)
            {
                if (strcmp(layer_name, layer_properties.layerName) == 0)
                {
                    found_layer = true;
                    break;
                }
            }
            if (!found_layer)
            {
                return false;
            }
        }
        return true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBase::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT        messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void*                                       pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void VulkanBase::setupDebugMessenger()
    {
        if (!enable_validation_layers)
        {
            return;
        }
        VkDebugUtilsMessengerCreateInfoEXT create_info {};
        create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debugCallback;
        create_info.pUserData       = nullptr;
        if (createDebugUtilsMessengerEXT(instance, &create_info, nullptr, &callback) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    VkResult VulkanBase::createDebugUtilsMessengerEXT(VkInstance                                instance,
                                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                      const VkAllocationCallbacks*              pAllocator,
                                                      VkDebugUtilsMessengerEXT*                 pDebugMessenger)
    {
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanBase::destroyDebugUtilsMessengerExt(VkInstance                   instance,
                                                   VkDebugUtilsMessengerEXT     debugMessenger,
                                                   const VkAllocationCallbacks* pAllocator)
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    std::vector<const char*> VulkanBase::getRequiredExtensions(SDL_Window* window)
    {
        uint32_t extension_count = 0;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr))
        {
            throw std::runtime_error("Failed to get SDL Vulkan extensions count!");
        }
        std::vector<const char*> extensions(extension_count);
        if (!SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extensions.data()))
        {
            throw std::runtime_error("Failed to get SDL Vulkan extensions!");
        }
        if (enable_validation_layers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }
} // namespace vulkanDetails