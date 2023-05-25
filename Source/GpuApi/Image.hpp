#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"
#include "Core/Core.hpp"
#include "ThirdParty/vma/vk_mem_alloc.h"

class Device;

class Image {
   private:
    Device* device;
    VkImage image{};
    VmaAllocation allocation{};
    VmaAllocationInfo allocationInfo{};

   public:
    Image(Device* device, const ImageDesc& desc, VmaAllocationCreateFlags allocationFlags);
    ~Image();

    inline Device* getDevice() { return device; }
    inline VkImage getVkImage() { return image; }
    inline VmaAllocation getVmaAllocation() { return allocation; }
    [[nodiscard]] inline void* getData() const { return allocationInfo.pMappedData; }
};

class ImageView {
   private:
    Device* device;
    VkImage image;
    VkImageView view{};

   public:
    ImageView(Image* image, VkFormat format);
    ImageView(Device* device, VkImage image, VkFormat format);
    ~ImageView();

    inline Device* getDevice() { return device; }
    inline VkImage getVkImage() { return image; }
    inline VkImageView getVkImageView() { return view; }
};
