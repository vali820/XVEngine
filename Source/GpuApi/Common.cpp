#include "Common.hpp"

#include "Image.hpp"

VkRenderingAttachmentInfo RenderingAttachment::getVkInfo() const {
    VkImageView view = imageView ? imageView->getVkImageView() : nullptr;
    return {
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView   = view,
        .imageLayout = imageLayout,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .loadOp      = loadOp,
        .storeOp     = storeOp,
        .clearValue  = clearValue,
    };
}
