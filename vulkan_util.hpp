#include "vulkan/vulkan.h"
#include <SDL_video.h>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <SDL2/SDL_vulkan.h> 

namespace vulkan{
    class vulkanBase{
        public:
            static vulkanBase* getInstance();
            void createInstance(SDL_Window* window);
            vulkanBase() = delete;
            void initVulkan(SDL_Window* window);
        private:
            static vulkanBase* singleton;
            VkInstance instance;
    };
}