#define STB_IMAGE_IMPLEMENTATION
#include "vulkan_util.hpp"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_video.h>
#include <SDL_vulkan.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <glm/trigonometric.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

// Add this line to include the necessary header file

namespace vulkanDetails
{
    constexpr uint32_t       WIDTH                = 800;
    constexpr uint32_t       HEIGHT               = 600;
    constexpr int            MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<const char*> validation_layers    = {
        "VK_LAYER_KHRONOS_validation",
    };
    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    // const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    //                                       {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    //                                       {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    //                                       {}};

    // const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //                                       {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    //                                       {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    //                                       {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
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

    void VulkanBase::initWindow()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return;
        }

        window = SDL_CreateWindow("Vulkan",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  WIDTH,
                                  HEIGHT,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!window)
        {
            std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }
    }
    void VulkanBase::createInstance()
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

        auto extensions                     = getRequiredExtensions();
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

    void VulkanBase::initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createTextureSampler();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createFrameBuffer();
        createCommandPool();
        createTextureImage();
        createTextureImageView();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffer();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        createSyncObject();
    }
    void VulkanBase::createTextureSampler()
    {
        VkSamplerCreateInfo sampler_info {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = 16;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;
        if(vkCreateSampler(device, &sampler_info, nullptr, &texture_sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    VkImageView VulkanBase::createImageView(VkImage image, VkFormat format)
    {
        VkImageViewCreateInfo view_info {};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                           = image;
        view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                          = format;
        view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;

        VkImageView image_view;
        if (vkCreateImageView(device, &view_info, nullptr, &image_view) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image view!");
        }

        return image_view;
    }
    void VulkanBase::createTextureImageView()
    {
        texture_image_view = createImageView(texture_image, VK_FORMAT_R8G8B8A8_SRGB);
    }
    VkCommandBuffer VulkanBase::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool        = command_pool;
        alloc_info.commandBufferCount = 1;
        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        return command_buffer;
    }
    void VulkanBase::endSingleTimeCommands(VkCommandBuffer command_buffer)
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info {};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &command_buffer;

        vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphics_queue);

        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
    }
    void VulkanBase::createTextureImage()
    {
        int      tex_width, tex_height, tex_channels;
        stbi_uc* pixels = stbi_load("../textures/texture.jpg", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;
        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }
        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(image_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer,
                     staging_buffer_memory);
        void* data;
        vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(device, staging_buffer_memory);
        stbi_image_free(pixels);

        createImage(tex_width,
                    tex_height,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    texture_image,
                    texture_image_memory);
        transitionImageLayout(
            texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(
            staging_buffer, texture_image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
        transitionImageLayout(texture_image,
                              VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }

    void VulkanBase::createImage(uint32_t              width,
                                 uint32_t              height,
                                 VkFormat              format,
                                 VkImageTiling         tiling,
                                 VkImageUsageFlags     usage,
                                 VkMemoryPropertyFlags properties,
                                 VkImage&              image,
                                 VkDeviceMemory&       image_memory)
    {
        VkImageCreateInfo image_info {};
        image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType     = VK_IMAGE_TYPE_2D;
        image_info.extent.width  = width;
        image_info.extent.height = height;
        image_info.extent.depth  = 1;
        image_info.mipLevels     = 1;
        image_info.arrayLayers   = 1;
        image_info.format        = format;
        image_info.tiling        = tiling;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage         = usage;
        image_info.samples       = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &image_info, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info {};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, image_memory, 0);
    }
    void VulkanBase::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(swap_chain_images.size(), descriptor_set_layout);
        VkDescriptorSetAllocateInfo        alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(swap_chain_images.size());
        alloc_info.pSetLayouts        = layouts.data();

        descriptor_sets.resize(swap_chain_images.size());
        if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_sets[0]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < swap_chain_images.size(); i++)
        {
            VkDescriptorBufferInfo buffer_info {};
            buffer_info.buffer = uniform_buffers[i];
            buffer_info.offset = 0;
            buffer_info.range  = sizeof(UniformBufferObject);
            VkDescriptorImageInfo image_info{};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = texture_image_view;
            image_info.sampler = texture_sampler;

           
            std::array<VkWriteDescriptorSet, 2> descriptor_writes{};

            descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[0].dstSet = descriptor_sets[i];
            descriptor_writes[0].dstBinding = 0;
            descriptor_writes[0].dstArrayElement = 0;
            descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_writes[0].descriptorCount = 1;
            descriptor_writes[0].pBufferInfo = &buffer_info;

            descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_writes[1].dstSet = descriptor_sets[i];
            descriptor_writes[1].dstBinding = 1;
            descriptor_writes[1].dstArrayElement = 0;
            descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_writes[1].descriptorCount = 1;
            descriptor_writes[1].pImageInfo = &image_info;
            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
        }
    }
    void VulkanBase::createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> pool_sizes {};
        pool_sizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        pool_sizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo pool_info;
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes    = pool_sizes.data();
        pool_info.maxSets       = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanBase::createUniformBuffer()
    {
        VkDeviceSize buffer_size = sizeof(UniformBufferObject);
        uniform_buffers.resize(swap_chain_images.size());
        uniform_buffers_memory.resize(swap_chain_images.size());
        for (size_t i = 0; i < swap_chain_images.size(); i++)
        {
            createBuffer(buffer_size,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         uniform_buffers[i],
                         uniform_buffers_memory[i]);
        }
    }

    void VulkanBase::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding {};
        ubo_layout_binding.binding            = 0;
        ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount    = 1;
        ubo_layout_binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_layout_binding.pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = 1;
        layout_info.pBindings    = &ubo_layout_binding;

        VkDescriptorSetLayoutBinding sampler_layout_binding {};
        sampler_layout_binding.binding = 1;
        sampler_layout_binding.descriptorCount = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding, sampler_layout_binding};
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings    = bindings.data();
        if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &descriptor_set_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    void VulkanBase::createIndexBuffer()
    {
        VkDeviceSize   buffer_size = sizeof(indices[0]) * indices.size();
        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer,
                     staging_buffer_memory);

        void* data;
        vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
        vkUnmapMemory(device, staging_buffer_memory);
        createBuffer(buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     index_buffer,
                     index_buffer_memory);
        copyBuffer(staging_buffer, index_buffer, buffer_size);
        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }

    void VulkanBase::createVertexBuffer()
    {
        VkDeviceSize   buffer_size = sizeof(vertices[0]) * vertices.size();
        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        createBuffer(buffer_size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     staging_buffer,
                     staging_buffer_memory);
        void* data;
        vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
        vkUnmapMemory(device, staging_buffer_memory);
        createBuffer(buffer_size,
                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     vertex_buffer,
                     vertex_buffer_memory);
        copyBuffer(staging_buffer, vertex_buffer, buffer_size);
        vkDestroyBuffer(device, staging_buffer, nullptr);
        vkFreeMemory(device, staging_buffer_memory, nullptr);
    }

    uint32_t VulkanBase::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                //  std::cout << "Memory type properties: " << mem_properties.memoryTypes[i].propertyFlags << std::endl;
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    void VulkanBase::createBuffer(VkDeviceSize          size,
                                  VkBufferUsageFlags    usage,
                                  VkMemoryPropertyFlags properties,
                                  VkBuffer&             buffer,
                                  VkDeviceMemory&       buffer_memory)
    {
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = size;
        buffer_info.usage              = usage;
        buffer_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);
        VkMemoryAllocateInfo alloc_info {};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);
        if (vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        vkBindBufferMemory(device, buffer, buffer_memory, 0);
    }

    void VulkanBase::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    {
        VkCommandBuffer command_buffer = beginSingleTimeCommands();
        VkBufferCopy    copy_region {};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
        endSingleTimeCommands(command_buffer);
    }

    void VulkanBase::transitionImageLayout(VkImage image,
                                           VkFormat /*format*/,
                                           VkImageLayout old_layout,
                                           VkImageLayout new_layout)
    {
        VkCommandBuffer      command_buffer = beginSingleTimeCommands();
        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = old_layout;
        barrier.newLayout                       = new_layout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
  
        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        endSingleTimeCommands(command_buffer);
    }

    void VulkanBase::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer command_buffer = beginSingleTimeCommands();

        VkBufferImageCopy region {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = {0, 0, 0};
        region.imageExtent                     = {width, height, 1};

        vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        endSingleTimeCommands(command_buffer);
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

    void VulkanBase::cleanup() const
    {
        cleanupSwapChain();
        vkDestroySampler(device, texture_sampler, nullptr);
        vkDestroyImageView(device, texture_image_view, nullptr);
        vkDestroyImage(device, texture_image, nullptr);
        vkFreeMemory(device, texture_image_memory, nullptr);
        vkDestroyDescriptorPool(device, descriptor_pool, nullptr);
        vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
        for (size_t i = 0; i < swap_chain_images.size(); i++)
        {
            vkDestroyBuffer(device, uniform_buffers[i], nullptr);
            vkFreeMemory(device, uniform_buffers_memory[i], nullptr);
        }
        vkDestroyBuffer(device, index_buffer, nullptr);
        vkFreeMemory(device, index_buffer_memory, nullptr);
        vkDestroyBuffer(device, vertex_buffer, nullptr);
        vkFreeMemory(device, vertex_buffer_memory, nullptr);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, render_finished_semaphores[i], nullptr);
            vkDestroySemaphore(device, image_available_semaphores[i], nullptr);
            vkDestroyFence(device, in_flight_fences[i], nullptr);
        }

        vkDestroyCommandPool(device, command_pool, nullptr);
        if (enable_validation_layers)
        {
            destroyDebugUtilsMessengerExt(instance, callback, nullptr);
        }

        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);

        vkDestroyInstance(instance, nullptr);
        SDL_DestroyWindow(window);
        SDL_Quit();
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

    VKAPI_ATTR VkBool32 VKAPI_CALL
    VulkanBase::debugCallback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                              [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT        messageType,
                              const VkDebugUtilsMessengerCallbackDataEXT*             pCallbackData,
                              [[maybe_unused]] void*                                  pUserData)
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

    bool VulkanBase::isDeviceSuitable(VkPhysicalDevice device)
    {
        bool swap_chain_adequate = false;
        if (enable_validation_layers)
        {
            SwapChainSupportDetails swap_chain_support = querySwapChainSupport(device);
            swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        }
        VkPhysicalDeviceProperties device_properties;

        vkGetPhysicalDeviceProperties(device, &device_properties);
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(device, &device_features);

        QueueFamilyIndices indices              = findQueueFamilies(device);
        bool               extensions_supported = checkDeviceExtensionSupport(device);
        return indices.isComplete() && extensions_supported && swap_chain_adequate;
    }

    QueueFamilyIndices VulkanBase::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        uint32_t           queue_family_count = 0;
        VkBool32           present_support    = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, 0, surface, &present_support);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
        int i = 0;
        for (const auto& queue_family : queue_families)
        {
            if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics_family = i;
            }
            if (queue_family.queueCount > 0 && present_support)
            {
                indices.present_family = i;
            }
            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }

    void VulkanBase::createLogicalDevice()
    {
        QueueFamilyIndices                   indices = findQueueFamilies(physical_device);
        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
        float              queue_priority        = 1.0f;
        for (auto queue_family : unique_queue_families)
        {
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount       = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features {};
        VkDeviceCreateInfo       device_create_info {};
        device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos       = queue_create_infos.data();
        device_create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pEnabledFeatures        = &device_features;
        device_create_info.enabledExtensionCount   = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
        if (enable_validation_layers)
        {
            device_create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
            device_create_info.ppEnabledLayerNames = validation_layers.data();
        }
        else
        {
            device_create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
        vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
    }

    void VulkanBase::createSurface()
    {

        if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
        {
            std::cerr << "failed to create window surface!" << std::endl;
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool VulkanBase::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extension_count;

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
        std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());
        for (const auto& extension : available_extensions)
        {
            required_extensions.erase(extension.extensionName);
        }
        return required_extensions.empty();
    }

    SwapChainSupportDetails VulkanBase::querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
        if (format_count != 0)
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;

        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
        if (present_mode_count != 0)
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device, surface, &present_mode_count, details.present_modes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanBase::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        if (available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }
        for (const auto& available_format : available_formats)
        {
            if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return available_format;
            }
        }
        return available_formats[0];
    }

    VkPresentModeKHR VulkanBase::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
    {
        for (const auto& available_present_mode : available_present_modes)
        {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return available_present_mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanBase::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int height, width;
            SDL_Vulkan_GetDrawableSize(window, &width, &height);
            VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actual_extent.width  = std::max(capabilities.minImageExtent.width,
                                           std::min(capabilities.maxImageExtent.width, actual_extent.width));
            actual_extent.height = std::max(capabilities.minImageExtent.height,
                                            std::min(capabilities.maxImageExtent.height, actual_extent.height));

            return actual_extent;
        }
    }

    void VulkanBase::createSwapChain()
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(physical_device);
        VkSurfaceFormatKHR      surface_format     = chooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR        present_mode       = chooseSwapPresentMode(swap_chain_support.present_modes);
        VkExtent2D              extent             = chooseSwapExtent(swap_chain_support.capabilities);
        std::cout << extent.width << " " << extent.height << std::endl;
        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        // uint32_t                image_count        = 1;
        if (swap_chain_support.capabilities.maxImageCount > 0 &&
            image_count > swap_chain_support.capabilities.maxImageCount)
        {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }
        VkSwapchainCreateInfoKHR create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface          = surface;
        create_info.minImageCount    = image_count;
        create_info.imageColorSpace  = surface_format.colorSpace;
        create_info.imageFormat      = surface_format.format;
        create_info.imageExtent      = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices                = findQueueFamilies(physical_device);
        uint32_t           queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = queue_family_indices;
        }
        else
        {
            create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;
            create_info.pQueueFamilyIndices   = nullptr;
        }
        create_info.preTransform   = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode    = present_mode;
        create_info.clipped        = VK_TRUE;
        create_info.oldSwapchain   = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swap_chain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }
        vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
        swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_chain_images.data());
        swap_chain_image_format = surface_format.format;
        swap_chain_extent       = extent;
    }

    void VulkanBase::createImageViews()
    {
        swap_chain_image_views.resize(swap_chain_images.size());
        for (size_t i = 0; i < swap_chain_images.size(); i++)
        {
            swap_chain_image_views[i] = createImageView(swap_chain_images[i], swap_chain_image_format);
        }
    }

    void VulkanBase::createRenderPass()
    {
        VkAttachmentDescription color_attachment {};
        color_attachment.format         = swap_chain_image_format;
        color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &color_attachment_ref;

        VkSubpassDependency dependency {};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_info {};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &color_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &subpass;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies   = &dependency;

        if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass");
        }
    }

    void VulkanBase::createGraphicsPipeline()
    {
        auto                            vertex_shader_code     = readFile("../shader/texture_vert.spv");
        auto                            fragment_shader_code   = readFile("../shader/texture_frag.spv");
        VkShaderModule                  vertex_shader_module   = createShaderModule(vertex_shader_code);
        VkShaderModule                  fragment_shader_module = createShaderModule(fragment_shader_code);
        VkPipelineShaderStageCreateInfo vertex_shader_stage_info {};
        vertex_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_shader_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_shader_stage_info.module = vertex_shader_module;
        vertex_shader_stage_info.pName  = "main";
        VkPipelineShaderStageCreateInfo fragment_shader_stage_info {};
        fragment_shader_stage_info.sType                = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_shader_stage_info.stage                = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_shader_stage_info.module               = fragment_shader_module;
        fragment_shader_stage_info.pName                = "main";
        VkPipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info};

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        auto                                 binding_description    = Vertex::getBindingDescription();
        auto                                 attribute_descriptions = Vertex::getAttributeDescriptions();
        vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount   = 1;
        vertex_input_info.pVertexBindingDescriptions      = &binding_description;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_info.pVertexAttributeDescriptions    = attribute_descriptions.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly {};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport {};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(swap_chain_extent.width);
        viewport.height   = static_cast<float>(swap_chain_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = swap_chain_extent;
        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports    = &viewport;
        viewport_state.scissorCount  = 1;
        viewport_state.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp          = 0.0f;
        rasterizer.depthBiasSlopeFactor    = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;

        VkPipelineColorBlendAttachmentState color_blend_attachment;
        color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable = VK_FALSE;
        // color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        // color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        // color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
        // color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        // color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        // color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo color_blending {};
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount   = 1;
        color_blending.pAttachments      = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};

        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = 2;
        dynamic_state.pDynamicStates    = dynamic_states;

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 1;
        pipeline_layout_info.pSetLayouts            = &descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges    = nullptr;

        if (vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount          = 2;
        pipeline_info.pStages             = shader_stages;
        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pDepthStencilState  = nullptr;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = nullptr;
        pipeline_info.layout              = pipeline_layout;
        pipeline_info.renderPass          = render_pass;
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;
        pipeline_info.basePipelineIndex   = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline");
        }

        vkDestroyShaderModule(device, fragment_shader_module, nullptr);
        vkDestroyShaderModule(device, vertex_shader_module, nullptr);
    }

    void VulkanBase::createFrameBuffer()
    {
        swap_chain_framebuffers.resize(swap_chain_image_views.size());
        for (size_t i = 0; i < swap_chain_image_views.size(); i++)
        {
            VkImageView attachments[] = {swap_chain_image_views[i]};

            VkFramebufferCreateInfo framebuffer_info {};
            framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass      = render_pass;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments    = attachments;
            framebuffer_info.width           = swap_chain_extent.width;
            framebuffer_info.height          = swap_chain_extent.height;
            framebuffer_info.layers          = 1;

            if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &swap_chain_framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create frame buffer!");
            }
        }
    }

    void VulkanBase::createCommandPool()
    {
        QueueFamilyIndices      queue_family_indices = findQueueFamilies(physical_device);
        VkCommandPoolCreateInfo pool_info {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
        pool_info.flags            = 0;
        if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanBase::createCommandBuffers()
    {
        command_buffers.resize(swap_chain_framebuffers.size());
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool        = command_pool;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
        if (vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }

        for (size_t i = 0; i < command_buffers.size(); i++)
        {
            VkCommandBufferBeginInfo begin_info {};
            begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            begin_info.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(command_buffers[i], &begin_info) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo render_pass_info {};
            render_pass_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass        = render_pass;
            render_pass_info.framebuffer       = swap_chain_framebuffers[i];
            render_pass_info.renderArea.offset = {0, 0};
            render_pass_info.renderArea.extent = swap_chain_extent;

            VkClearValue clear_color         = {0.0f, 0.0f, 0.0f, 1.0f};
            render_pass_info.clearValueCount = 1;
            render_pass_info.pClearValues    = &clear_color;

            vkCmdBeginRenderPass(command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

            VkBuffer     vertex_buffers[] = {vertex_buffer};
            VkDeviceSize offsets          = {0};
            vkCmdBindVertexBuffers(command_buffers[i], 0, 1, vertex_buffers, &offsets);
            vkCmdBindIndexBuffer(command_buffers[i], index_buffer, 0, VK_INDEX_TYPE_UINT16);
            // vkCmdDraw(command_buffers[i], 3, 1, 0, 0);
            vkCmdBindDescriptorSets(command_buffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline_layout,
                                    0,
                                    1,
                                    &descriptor_sets[i],
                                    0,
                                    nullptr);
            vkCmdDrawIndexed(command_buffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
            vkCmdEndRenderPass(command_buffers[i]);

            if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    VkShaderModule VulkanBase::createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo create_info {};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shader_module;

        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shader_module;
    }

    void VulkanBase::createSyncObject()
    {
        image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
        in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphore_info {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create semaphores!");
            }
        }
    }

    void VulkanBase::mainLoop()
    {
        SDL_Event e;
        bool      quit = false;
        while (!quit)
        {
            while (SDL_PollEvent(&e))
            {
                switch (e.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_WINDOWEVENT:
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                        {
                            framebufferResizeCallback();
                        }
                        break;
                    default:
                        break;
                }
            }
            drawFrame();
        }
    }
    void VulkanBase::updateUniformBuffer(uint32_t current_image)
    {
        static auto start_time   = std::chrono::high_resolution_clock::now();
        auto        current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
        UniformBufferObject ubo {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj =
            glm::perspective(glm::radians(45.0f),
                             static_cast<float>(swap_chain_extent.width) / static_cast<float>(swap_chain_extent.height),
                             0.1f,
                             10.0f);
        ubo.proj[1][1] *= -1;
        void* data;
        vkMapMemory(device, uniform_buffers_memory[current_image], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniform_buffers_memory[current_image]);
    }

    void VulkanBase::framebufferResizeCallback() { framebuffer_resized = true; }

    void VulkanBase::drawFrame()
    {
        vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(
            device, swap_chain, UINT64_MAX, image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized)
        {
            framebuffer_resized = false;
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &in_flight_fences[current_frame]);
        updateUniformBuffer(image_index);
        VkSubmitInfo submit_info {};
        submit_info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore          wait_semaphores[] = {image_available_semaphores[current_frame]};
        VkPipelineStageFlags wait_stages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submit_info.waitSemaphoreCount         = 1;
        submit_info.pWaitSemaphores            = wait_semaphores;
        submit_info.pWaitDstStageMask          = wait_stages;
        submit_info.commandBufferCount         = 1;
        submit_info.pCommandBuffers            = &command_buffers[image_index];
        VkSemaphore signal_semaphores[]        = {render_finished_semaphores[current_frame]};
        submit_info.signalSemaphoreCount       = 1;
        submit_info.pSignalSemaphores          = signal_semaphores;
        if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR present_info {};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = signal_semaphores;
        VkSwapchainKHR swap_chains[]    = {swap_chain};
        present_info.swapchainCount     = 1;
        present_info.pSwapchains        = swap_chains;
        present_info.pImageIndices      = &image_index;
        present_info.pResults           = nullptr;
        result                          = vkQueuePresentKHR(present_queue, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized)
        {
            framebuffer_resized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
        current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanBase::pickPhysicalDevice()
    {
        uint32_t device_count = 0;

        vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
        if (device_count == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
        for (const auto& device : devices)
        {
            if (isDeviceSuitable(device))
            {
                physical_device = device;
                VkPhysicalDeviceProperties physical_device_properties;
                vkGetPhysicalDeviceProperties(device, &physical_device_properties);
                std::cout << "GPU: " << physical_device_properties.deviceName << std::endl;
                break;
            }
        }
        if (physical_device == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to fina a suitable GPU!");
        }
    }

    std::vector<const char*> VulkanBase::getRequiredExtensions()
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

    void VulkanBase::recreateSwapChain()
    {
        int height, width = 0;
        while (width == 0 || height == 0)
        {
            SDL_Vulkan_GetDrawableSize(window, &width, &height);
            SDL_PollEvent(nullptr);
        }
        vkDeviceWaitIdle(device);
        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFrameBuffer();
        createCommandBuffers();
    }
    void VulkanBase::cleanupSwapChain() const
    {
        vkDestroyBuffer(device, vertex_buffer, nullptr);
        vkFreeMemory(device, vertex_buffer_memory, nullptr);
        for (const auto& framebuffer : swap_chain_framebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        vkFreeCommandBuffers(
            device, command_pool, static_cast<uint32_t>(command_buffers.size()), command_buffers.data());
        vkDestroyPipeline(device, graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyRenderPass(device, render_pass, nullptr);
        for (const auto& image_view : swap_chain_image_views)
        {
            vkDestroyImageView(device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(device, swap_chain, nullptr);
    }
} // namespace vulkanDetails