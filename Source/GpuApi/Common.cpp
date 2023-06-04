#include "Common.hpp"

#include "Image.hpp"

VkRenderingAttachmentInfo RenderingAttachment::getVkInfo() const {
    VkImageView view        = imageView ? imageView->getVkImageView() : nullptr;
    VkImageView resolveView = resolveImageView ? resolveImageView->getVkImageView() : nullptr;
    return {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView          = view,
        .imageLayout        = imageLayout,
        .resolveMode        = resolveMode,
        .resolveImageView   = resolveView,
        .resolveImageLayout = resolveImageLayout,
        .loadOp             = loadOp,
        .storeOp            = storeOp,
        .clearValue         = clearValue,
    };
}
