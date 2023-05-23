#include "Pipeline.hpp"

#include "Device.hpp"

PipelineLayout::PipelineLayout(Device *_device, const Vec<DescriptorSetLayout *> &descriptorSets,
                               const Vec<VkPushConstantRange> &pushConstantRanges)
    : device(_device) {
    Vec<VkDescriptorSetLayout> sets;
    for (auto set : descriptorSets) sets.push(set->getVkDescriptorSetLayout());
    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (u32)sets.getSize(),
        .pSetLayouts = sets.getData(),
        .pushConstantRangeCount = (u32)pushConstantRanges.getSize(),
        .pPushConstantRanges = pushConstantRanges.getData(),
    };
    vkCreatePipelineLayout(device->getVkDevice(), &createInfo, nullptr, &layout);
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device->getVkDevice(), layout, nullptr);
}

VkPipelineInputAssemblyStateCreateInfo InputAssemblyDesc::getVkInfo() const {
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
        .primitiveRestartEnable = primitiveRestartEnable,
    };
}

VkPipelineRasterizationStateCreateInfo RasterizationDesc::getVkInfo() const {
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = polygonMode,
        .cullMode = cullMode,
        .depthBiasEnable = false,
        .lineWidth = lineWidth,
    };
}

VkPipelineColorBlendStateCreateInfo ColorBlendDesc::getVkInfo() const {
    return {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = logicOpEnable,
        .logicOp = logicOp,
        .attachmentCount = (u32)blendAttachments.getSize(),
        .pAttachments = blendAttachments.getData(),
        .blendConstants =
            {
                blendConstants[0],
                blendConstants[1],
                blendConstants[2],
                blendConstants[3],
            },
    };
}

GraphicsPipeline::GraphicsPipeline(Device *_device, const GraphicsPipelineDesc &desc)
    : device(_device) {
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (u32)desc.dynamicStates.getSize(),
        .pDynamicStates = desc.dynamicStates.getData(),
    };

    VkPipelineViewportStateCreateInfo viewportInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &desc.viewport,
        .scissorCount = 1,
        .pScissors = &desc.scissor,
    };

    auto rasterizationInfo = desc.rasterizationDesc.getVkInfo();
    auto inputAssemblyInfo = desc.inputAssemblyDesc.getVkInfo();
    auto colorBlendInfo = desc.colorBlendDesc.getVkInfo();

    // TODO: Create proper struct for these
    VkPipelineVertexInputStateCreateInfo vertexInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0,
    };
    VkPipelineMultisampleStateCreateInfo multisampleInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = false,
    };

    VkGraphicsPipelineCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pVertexInputState = &vertexInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = desc.layout->getVkPipelineLayout(),
        .renderPass = nullptr,
    };

    vkCreateGraphicsPipelines(device->getVkDevice(), nullptr, 1, &createInfo, nullptr, &pipeline);
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(device->getVkDevice(), pipeline, nullptr);
}