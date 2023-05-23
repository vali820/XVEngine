#pragma once

#include <vulkan/vulkan.h>

#include "GpuApi/Common.hpp"

class Device;
class Window;
class Semaphore;
class Fence;

class Surface {
   private:
    Device* device;
    Window* window;

    VkSurfaceKHR surface{};
    VkSwapchainKHR swapchain{};

    VkFormat format{};
    VkColorSpaceKHR colorSpace{};
    VkPresentModeKHR presentMode{};
    VkExtent2D extent{};

    Vec<VkSurfaceFormatKHR> supportedFormats;
    Vec<VkPresentModeKHR> supportedPresentModes;
    VkSurfaceCapabilitiesKHR surfaceCaps{};

    u32 imageCount = 0;
    Vec<VkImage> images;
    Vec<VkImageView> imageViews;

   public:
    Surface(Device* _device, Window* _window);

    ~Surface();

    void configure(const SurfaceConfig& config);

    u32 getNextImageIndex(u64 timeout, Semaphore* semaphore, Fence* fence);

    inline const Vec<VkImageView>& getImageViews() { return imageViews; }
    inline const Vec<VkImage>& getImages() { return images; }

    inline Device* getDevice() { return device; }
    inline Window* getWindow() { return window; }
    inline VkSurfaceKHR getVkSurface() { return surface; }
    inline VkSwapchainKHR getVkSwapchain() { return swapchain; }

    [[nodiscard]] inline VkFormat getFormat() const { return format; }
    [[nodiscard]] inline VkColorSpaceKHR getColorSpace() const { return colorSpace; }
    [[nodiscard]] inline VkPresentModeKHR getPresentMode() const { return presentMode; }
    [[nodiscard]] inline const Vec<VkSurfaceFormatKHR>& getSupportedFormats() const {
        return supportedFormats;
    }
    [[nodiscard]] inline const Vec<VkPresentModeKHR>& getSupportedPresentModes() const {
        return supportedPresentModes;
    }
    [[nodiscard]] inline VkSurfaceCapabilitiesKHR getSurfaceCapabilities() const {
        return surfaceCaps;
    }

   private:
    void createSurface();
    void getSurfaceSupport();
    void createSwapchain();
    void destroySwapchain();
    void createImageViews();
};
