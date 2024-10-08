#pragma once
#include "vulkan/vulkan.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>
namespace vulkanDetails
{
    struct UniformBufferObject{
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct Vertex{
        glm::vec2 pos;
        glm::vec3 color;
        glm::vec2 tex_coord;

        static VkVertexInputBindingDescription getBindingDescription(){
            VkVertexInputBindingDescription binding_description{};
            binding_description.binding = 0;
            binding_description.stride = sizeof(Vertex);
            binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return binding_description;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array <VkVertexInputAttributeDescription, 3> attribute_descriptions{};
            attribute_descriptions[0].binding = 0;
            attribute_descriptions[0].location = 0;
            attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attribute_descriptions[0].offset = offsetof(Vertex, pos);
            attribute_descriptions[1].binding = 0;
            attribute_descriptions[1].location = 1;
            attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attribute_descriptions[1].offset = offsetof(Vertex, color);
            attribute_descriptions[2].binding = 0;
            attribute_descriptions[2].location = 2;
            attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);
            return attribute_descriptions;
        }
    };

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
        void    initWindow();
        static VulkanBase* getInstance();
        void               createInstance();

        void                                  initVulkan();
        static void                           printExtensionProperties();
        void                                  cleanup() const;
        static bool                           checkValidationLayerSupport(std::vector<const char*>& validation_layers);
        std::vector<const char*>       getRequiredExtensions();
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
        void                      createSurface();
        static bool               checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails   querySwapChainSupport(VkPhysicalDevice device);
        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
        static VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
        VkExtent2D                chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        void                      createSwapChain();
        void                      createImageViews();
        void                      createGraphicsPipeline();
        VkShaderModule            createShaderModule(const std::vector<char>& code);
        void                      createRenderPass();
        void                      createFrameBuffer();
        void                      createCommandPool();
        void                      createCommandBuffers();
        void                      drawFrame();
        void                      mainLoop();
        void                      createSyncObject();
        void                      recreateSwapChain();
        void                      cleanupSwapChain() const;
        void framebufferResizeCallback();
        void createVertexBuffer();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                          VkDeviceMemory& buffer_memory);
        void copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
        void createIndexBuffer();
        void createDescriptorSetLayout();      
        void createUniformBuffer();
        void updateUniformBuffer(uint32_t current_image);
        void createDescriptorPool();
        void createDescriptorSets();
        void createTextureImage();
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer command_buffer);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void createTextureImageView();
        VkImageView createImageView(VkImage image, VkFormat format);
        void createTextureSampler();
    private:
        SDL_Window* window{};
        VulkanBase() = default;
        static VulkanBase*           m_singleton;
        VkInstance                   instance {};
        VkDebugUtilsMessengerEXT     callback {};
        VkPhysicalDevice             physical_device = VK_NULL_HANDLE;
        VkDevice                     device {};
        VkQueue                      graphics_queue {};
        VkQueue                      present_queue {};
        VkSurfaceKHR                 surface {};
        VkSwapchainKHR               swap_chain {};
        std::vector<VkImage>         swap_chain_images;
        VkFormat                     swap_chain_image_format {};
        VkExtent2D                   swap_chain_extent {};
        std::vector<VkImageView>     swap_chain_image_views;
        VkRenderPass                 render_pass {};
        VkPipelineLayout             pipeline_layout {};
        VkPipeline                   graphics_pipeline {};
        std::vector<VkFramebuffer>   swap_chain_framebuffers;
        VkCommandPool                command_pool {};
        std::vector<VkCommandBuffer> command_buffers;
        std::vector<VkSemaphore>     image_available_semaphores;
        std::vector<VkSemaphore>     render_finished_semaphores;
        std::vector<VkFence>         in_flight_fences;
        uint32_t                     current_frame = 0;
        bool                         framebuffer_resized = false;
        VkBuffer    vertex_buffer{};
        VkDeviceMemory vertex_buffer_memory{};
        VkBuffer    index_buffer{};
        VkDeviceMemory index_buffer_memory{};
        std::vector<VkBuffer> uniform_buffers;
        std::vector<VkDeviceMemory> uniform_buffers_memory;
        VkDescriptorSetLayout descriptor_set_layout{};
        VkDescriptorPool descriptor_pool{};
        std::vector<VkDescriptorSet> descriptor_sets;
        VkImage texture_image{};
        VkDeviceMemory texture_image_memory{};
        VkImageView texture_image_view{};
        VkSampler texture_sampler{};
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