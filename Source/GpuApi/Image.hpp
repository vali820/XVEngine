#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"
#include "Core/Core.hpp"
#include "ThirdParty/vma/vk_mem_alloc.h"

class Device;
class CmdBuffer;

class Image {
   private:
    Device* device;
    VkImage image{};
    VmaAllocation allocation{};
    VmaAllocationInfo allocationInfo{};
    VkFormat format;
    const bool owned = true;

   public:
    Image(Device* device, VkFormat format, u32 width, u32 height, VkImageUsageFlags usage,
          VkSampleCountFlagBits samples, VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
          VmaAllocationCreateFlags allocationFlags = 0);
    Image(Device* device, VkImage image, VkFormat format);
    ~Image();

    bool transitionLayoutCmd(CmdBuffer* cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
    void transitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    inline Device* getDevice() { return device; }
    inline VkImage getVkImage() { return image; }
    [[nodiscard]] inline VkFormat getVkFormat() const { return format; }
    VmaAllocation getVmaAllocation();
    [[nodiscard]] void* getData() const;
};

class ImageView {
   private:
    Device* device;
    VkImage image;
    VkImageView view{};

   public:
    explicit ImageView(Image* image, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    ImageView(Device* device, VkImage image, VkFormat format,
              VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    ~ImageView();

    inline Device* getDevice() { return device; }
    inline VkImage getVkImage() { return image; }
    inline VkImageView getVkImageView() { return view; }
};
