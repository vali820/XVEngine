#include "Common.hpp"

VkRenderingAttachmentInfo RenderingAttachment::getVkInfo() const {
    return {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = imageView,
        .imageLayout = imageLayout,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .loadOp = loadOp,
        .storeOp = storeOp,
        .clearValue = clearValue,
    };
}
