#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"

class Device;

class DescriptorSetLayout;

struct InputAssemblyDesc {
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    bool primitiveRestartEnable = false;

    [[nodiscard]] VkPipelineInputAssemblyStateCreateInfo getVkInfo() const;
};

struct RasterizationDesc {
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
    f32 lineWidth;

    [[nodiscard]] VkPipelineRasterizationStateCreateInfo getVkInfo() const;
};

struct ColorBlendDesc {
    Vec<VkPipelineColorBlendAttachmentState> blendAttachments;
    f32 blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    bool logicOpEnable = false;
    VkLogicOp logicOp = VK_LOGIC_OP_COPY;

    [[nodiscard]] VkPipelineColorBlendStateCreateInfo getVkInfo() const;
};

class PipelineLayout;

struct GraphicsPipelineDesc {
    Vec<VkDynamicState> dynamicStates;
    VkViewport viewport{};
    VkRect2D scissor{};
    InputAssemblyDesc inputAssemblyDesc;
    RasterizationDesc rasterizationDesc;
    ColorBlendDesc colorBlendDesc;
    PipelineLayout* layout;
};

class PipelineLayout {
   private:
    Device* device;
    VkPipelineLayout layout{};

   public:
    PipelineLayout(Device* _device, const Vec<DescriptorSetLayout*>& descriptorSets,
                   const Vec<VkPushConstantRange>& pushConstantRanges);

    ~PipelineLayout();

    inline Device* getDevice() { return device; }
    inline VkPipelineLayout getVkPipelineLayout() { return layout; }
};

class GraphicsPipeline {
   private:
    Device* device;
    VkPipeline pipeline{};

   public:
    GraphicsPipeline(Device* _device, const GraphicsPipelineDesc& desc);
    ~GraphicsPipeline();

    inline Device* getDevice() { return device; }
    inline VkPipeline getVkPipeline() { return pipeline; }
};
