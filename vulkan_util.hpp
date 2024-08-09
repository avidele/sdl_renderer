#pragma once
#include "vulkan/vulkan.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL_video.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vulkanDetails
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   present_modes;
    };
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

        void                      pickPhysicalDevice();
        bool                      isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices        findQueueFamilies(VkPhysicalDevice device);
        void                      createLogicalDevice();
        void                      createSurface(SDL_Window* window);
        static bool               checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails   querySwapChainSupport(VkPhysicalDevice device);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
        VkPresentModeKHR          chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
        VkExtent2D                chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void                      createSwapChain();
        void                      createImageViews();
        void                      createGraphicsPipeline();
        VkShaderModule            createShaderModule(const std::vector<char>& code);
        void createRenderPass();

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
        VkSwapchainKHR           swap_chain {};
        std::vector<VkImage>     swap_chain_images;
        VkFormat                 swap_chain_image_format {};
        VkExtent2D               swap_chain_extent {};
        std::vector<VkImageView> swap_chain_image_views;
        VkRenderPass render_pass{};
        VkPipelineLayout pipeline_layout{};
    };
    static std::vector<char> readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }
        size_t            file_size = file.tellg();
        std::vector<char> buffer(file_size);
        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(file_size));
        file.close();
        return buffer;
    }

} // namespace vulkanDetails