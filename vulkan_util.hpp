#pragma once
#include "vulkan/vulkan.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL_video.h>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <set>

namespace vulkanDetails
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        [[nodiscard]] bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
    };

    class VulkanBase
    {
    public:
        static VulkanBase* getInstance();
        void               createInstance(SDL_Window* window);

        void                                  initVulkan(SDL_Window* window);
        static void                           printExtensionProperties();
        void                                  cleanup() const;
        static bool                           checkValidationLayerSupport(std::vector<const char*>& validation_layers);
        static std::vector<const char*>       getRequiredExtensions(SDL_Window* window);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void*                                       pUserData);
        void                                  setupDebugMessenger();
        static VkResult                       createDebugUtilsMessengerEXT(VkInstance                                instance,
                                                                           const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                           const VkAllocationCallbacks*              pAllocator,
                                                                           VkDebugUtilsMessengerEXT*                 pDebugMessenger);

        static void destroyDebugUtilsMessengerExt(VkInstance                   instance,
                                                  VkDebugUtilsMessengerEXT     debugMessenger,
                                                  const VkAllocationCallbacks* pAllocator);

        void               pickPhysicalDevice();
        bool               isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void               createLogicalDevice();
        void               createSurface(SDL_Window* window);

    private:
        VulkanBase() = default;
        static VulkanBase*       m_singleton;
        VkInstance               instance {};
        VkDebugUtilsMessengerEXT callback {};
        VkPhysicalDevice         physical_device = VK_NULL_HANDLE;
        VkDevice                 device {};
        VkQueue                  graphics_queue {};
        VkQueue                  present_queue {};
        VkSurfaceKHR             surface {};
    };
} // namespace vulkanDetails