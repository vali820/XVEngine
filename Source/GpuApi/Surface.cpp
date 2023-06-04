#include "Surface.hpp"

#include <vulkan/vulkan.h>

#include "App/Window/Window.hpp"
#include "Device.hpp"
#include "Image.hpp"

// clang-format off
#ifdef __linux__
#include "App/Window/X11Window.hpp"
#include <vulkan/vulkan_xcb.h>

#include "App/Window/WlWindow.hpp"
#include <vulkan/vulkan_wayland.h>
#endif
// clang-format on

#include "Fence.hpp"
#include "Semaphore.hpp"

Surface::Surface(Device* _device, Window* _window) : device(_device), window(_window) {
    createSurface();
    getSurfaceSupport();
}

Surface::~Surface() {
    destroySwapchain();
    vkDestroySurfaceKHR(device->getVkInstance(), surface, nullptr);
}

void Surface::configure(const SurfaceConfig& config) {
    getSurfaceSupport();

    // Format and color space
    bool formatOk = false;
    for (const auto& f : supportedFormats) {
        if (f.format == config.format and f.colorSpace == config.colorSpace) {
            format     = config.format;
            colorSpace = config.colorSpace;
            formatOk   = true;
            break;
        }
    }
    if (!formatOk) throw std::runtime_error("Format or color space not supported");

    // Present mode
    bool presentModeOk = false;
    for (const auto& pm : supportedPresentModes) {
        if (pm == config.presentMode) {
            presentMode   = config.presentMode;
            presentModeOk = true;
            break;
        }
    }
    if (!presentModeOk) throw std::runtime_error("Present mode not supported");

    // Extent
    if (isX11Window) {
        extent = surfaceCaps.currentExtent;
    } else if (config.width >= surfaceCaps.minImageExtent.width and
               config.width <= surfaceCaps.maxImageExtent.width and
               config.height >= surfaceCaps.minImageExtent.height and
               config.height <= surfaceCaps.maxImageExtent.height) {
        extent = {config.width, config.height};
    } else
        throw std::runtime_error("Extent not supported");

    createSwapchain();
}

void Surface::createSurface() {
#ifdef __linux__
    if (auto x11Window = dynamic_cast<X11Window*>(window)) {
        isX11Window = true;
        VkXcbSurfaceCreateInfoKHR createInfo{
            .sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
            .connection = x11Window->getXcbConnection(),
            .window     = x11Window->getXcbWindow(),
        };
        vkCreateXcbSurfaceKHR(device->getVkInstance(), &createInfo, nullptr, &surface);
    } else if (auto waylandWindow = dynamic_cast<WlWindow*>(window)) {
        VkWaylandSurfaceCreateInfoKHR createInfo{
            .sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .display = waylandWindow->getWlDisplay(),
            .surface = waylandWindow->getWlSurface(),
        };
        vkCreateWaylandSurfaceKHR(device->getVkInstance(), &createInfo, nullptr, &surface);
    } else
        throw std::runtime_error("Invalid window for surface creation");
#endif
}

void Surface::getSurfaceSupport() {
    // Get supported surface formats
    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->getVkPhysicalDevice(), surface, &formatCount, nullptr);
    supportedFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device->getVkPhysicalDevice(), surface, &formatCount, supportedFormats.getData());

    // Get supported present modes
    u32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->getVkPhysicalDevice(), surface, &presentModeCount, nullptr);
    supportedPresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device->getVkPhysicalDevice(), surface, &presentModeCount, supportedPresentModes.getData());

    // Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->getVkPhysicalDevice(), surface, &surfaceCaps);
}

void Surface::createSwapchain() {
    u32 minImageCount = surfaceCaps.minImageCount + 1;
    if (surfaceCaps.maxImageCount > 0 and minImageCount > surfaceCaps.maxImageCount)
        minImageCount = surfaceCaps.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{
        .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface          = surface,
        .minImageCount    = minImageCount,
        .imageFormat      = format,
        .imageColorSpace  = colorSpace,
        .imageExtent      = extent,
        .imageArrayLayers = 1,
        .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform     = surfaceCaps.currentTransform,
        .compositeAlpha   = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .presentMode      = presentMode,
        .clipped          = VK_TRUE,
        .oldSwapchain     = swapchain,
    };

    VkSwapchainKHR newSwapchain;
    vkCreateSwapchainKHR(device->getVkDevice(), &createInfo, nullptr, &newSwapchain);

    destroySwapchain();
    swapchain = newSwapchain;

    // Get images
    vkGetSwapchainImagesKHR(device->getVkDevice(), swapchain, &imageCount, nullptr);
    Vec<VkImage> vkImages(imageCount);
    vkGetSwapchainImagesKHR(device->getVkDevice(), swapchain, &imageCount, vkImages.getData());

    for (VkImage img : vkImages) {
        images.push(new Image(device, img, format));
    }

    // Create image views
    for (Image* img : images) {
        imageViews.push(new ImageView(img));
    }
}

void Surface::destroySwapchain() {
    if (swapchain) {
        vkDestroySwapchainKHR(device->getVkDevice(), swapchain, nullptr);
        for (auto view : imageViews) delete view;
        for (auto img : images) delete img;
        images.clear();
        imageViews.clear();
    }
}

VkResult Surface::getNextImageIndex(u64 timeout, Semaphore* semaphore, Fence* fence, u32& idx) {
    VkSemaphore vkSemaphore = semaphore ? semaphore->getVkSemaphore() : nullptr;
    VkFence vkFence         = fence ? fence->getVkFence() : nullptr;
    return vkAcquireNextImageKHR(device->getVkDevice(), swapchain, timeout, vkSemaphore, vkFence, &idx);
}
