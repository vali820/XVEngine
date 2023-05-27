#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "Core/Core.hpp"

class DescriptorSetLayout;
class ImageView;

const u32 COLOR_COMPONENTS_ALL =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

struct SurfaceConfig {
    VkFormat format;
    VkColorSpaceKHR colorSpace;
    VkPresentModeKHR presentMode;
    u32 width, height;
};

enum class CmdBufferLevel {
    Primary   = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
};

struct ShaderDesc {
    VkShaderStageFlagBits stage;
    VkShaderStageFlags nextStage;
    VkShaderCodeTypeEXT codeType;
    Vec<u8> code;
    const char* name;
    Vec<DescriptorSetLayout*> setLayouts;
    Vec<VkPushConstantRange> pushConstantRanges;
};

struct RenderingAttachment {
    ImageView* imageView;
    VkImageLayout imageLayout;
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkClearValue clearValue;

    [[nodiscard]] VkRenderingAttachmentInfo getVkInfo() const;
};

struct RenderingInfo {
    VkRect2D renderArea;
    Vec<RenderingAttachment> colorAttachments;
    RenderingAttachment depthAttachment;
    RenderingAttachment stencilAttachment;
};

struct VertexBinding {
    u32 binding;
    u32 stride;
    VkVertexInputRate inputRate;
};

struct VertexAttribute {
    u32 location;
    u32 binding;
    u32 offset;
    VkFormat format;
};

struct ImageDesc {
    VkFormat format;
    u32 width, height;
    VkImageUsageFlags usage;
    VkImageLayout initialLayout;
};

struct DescriptorBufferBindingInfo {
    u64 address;
    VkBufferUsageFlags usage;
};