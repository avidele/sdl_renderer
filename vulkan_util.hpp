#pragma once
#include "vulkan/vulkan.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL_video.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
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
        static bool checkValidationLayerSupport(std::vector<const char*>& validation_layers);
        static std::vector<const char*> getRequiredExtensions(SDL_Window* window);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void*                                       pUserData);
        void setupDebugMessenger();
        VkResult createDebugUtilsMessengerEXT(VkInstance                                instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks*              pAllocator,
                                              VkDebugUtilsMessengerEXT*                 pDebugMessenger);
    
        static void destroyDebugUtilsMessengerExt(VkInstance                   instance,
                                                  VkDebugUtilsMessengerEXT     debugMessenger,
                                                  const VkAllocationCallbacks* pAllocator);
    private:
        VulkanBase() = default;
        static VulkanBase* m_singleton;
        VkInstance         instance;
        VkDebugUtilsMessengerEXT callback;
    };
} // namespace vulkanDetails