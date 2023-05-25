#include "Image.hpp"

#include "Device.hpp"

Image::Image(Device *_device, const ImageDesc &desc, VmaAllocationCreateFlags allocationFlags)
    : device(_device) {
    VkImageCreateInfo createInfo{
        .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType     = VK_IMAGE_TYPE_2D,
        .format        = desc.format,
        .extent        = {desc.width, desc.height, 1},
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = VK_SAMPLE_COUNT_1_BIT,
        .tiling        = VK_IMAGE_TILING_OPTIMAL,
        .usage         = desc.usage,
        .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = desc.initialLayout,
    };
    VmaAllocationCreateInfo allocationCreateInfo{
        .flags = allocationFlags | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    vmaCreateImage(
        device->getVmaAllocator(), &createInfo, &allocationCreateInfo, &image, &allocation, &allocationInfo);
}

Image::~Image() { vmaDestroyImage(device->getVmaAllocator(), image, allocation); }

ImageView::ImageView(Image *_image, VkFormat format)
    : ImageView(_image->getDevice(), _image->getVkImage(), format) {}

ImageView::ImageView(Device *_device, VkImage _image, VkFormat format) : device(_device), image(_image) {
    VkImageViewCreateInfo createInfo{
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image    = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    vkCreateImageView(device->getVkDevice(), &createInfo, nullptr, &view);
}

ImageView::~ImageView() { vkDestroyImageView(device->getVkDevice(), view, nullptr); }