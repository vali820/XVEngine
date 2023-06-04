#include "Image.hpp"

#include "CmdBuffer.hpp"
#include "Device.hpp"
#include "Queue.hpp"

Image::Image(Device *_device, VkFormat _format, u32 width, u32 height, VkImageUsageFlags usage,
             VkSampleCountFlagBits samples, VkImageLayout initialLayout,
             VmaAllocationCreateFlags allocationFlags)
    : device(_device), format(_format) {
    VkImageCreateInfo createInfo{
        .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType     = VK_IMAGE_TYPE_2D,
        .format        = format,
        .extent        = {width, height, 1},
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = samples,
        .tiling        = VK_IMAGE_TILING_OPTIMAL,
        .usage         = usage,
        .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = initialLayout,
    };
    VmaAllocationCreateInfo allocationCreateInfo{
        .flags = allocationFlags,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    vmaCreateImage(
        device->getVmaAllocator(), &createInfo, &allocationCreateInfo, &image, &allocation, &allocationInfo);
}

Image::Image(Device *_device, VkImage _image, VkFormat _format)
    : device(_device), image(_image), format(_format), owned(false) {}

Image::~Image() {
    if (owned) vmaDestroyImage(device->getVmaAllocator(), image, allocation);
}

VmaAllocation Image::getVmaAllocation() {
    if (!owned) throw std::runtime_error("Image not owned");
    return allocation;
}

void *Image::getData() const {
    if (!owned) throw std::runtime_error("Image not owned");
    if (!allocationInfo.pMappedData)
        throw std::runtime_error("Data is nullptr, image is probably bot mapped");
    return allocationInfo.pMappedData;
}

bool Image::transitionLayoutCmd(CmdBuffer *cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkImageMemoryBarrier2 barrier{
        .sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .image     = image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    // Prepare attachment for present
    if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL and
        newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        barrier.srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        barrier.dstStageMask  = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;
    }
    // Prepare attachment for render
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED and
             newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.srcStageMask  = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
        barrier.dstStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    }
    // Invalid transition
    else
        return false;

    cmdBuffer->imageMemoryBarrier(barrier);
    return true;
}

void Image::transitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
    auto cmd = new CmdBuffer(device->getGraphicsCmdPool());
    cmd->begin(true);
    if (!transitionLayoutCmd(cmd, oldLayout, newLayout))
        throw std::runtime_error("Invalid or unimplemented layout transition");
    cmd->end();
    device->getGraphicsQueue()->submit({cmd});
    device->getGraphicsQueue()->waitIdle();
    delete cmd;
}

ImageView::ImageView(Image *_image, VkImageAspectFlags aspect)
    : ImageView(_image->getDevice(), _image->getVkImage(), _image->getVkFormat(), aspect) {}

ImageView::ImageView(Device *_device, VkImage _image, VkFormat format, VkImageAspectFlags aspect)
    : device(_device), image(_image) {
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
                .aspectMask     = aspect,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    vkCreateImageView(device->getVkDevice(), &createInfo, nullptr, &view);
}

ImageView::~ImageView() { vkDestroyImageView(device->getVkDevice(), view, nullptr); }