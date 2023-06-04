#include "Shader.hpp"

#include <vulkan/vulkan_core.h>

#include "Descriptor.hpp"
#include "Device.hpp"

Shader::Shader(Device *_device, const ShaderDesc &desc) : device(_device) {
    stage = desc.stage;

    Vec<VkDescriptorSetLayout> sets;
    for (auto set : desc.setLayouts) sets.push(set->getVkDescriptorSetLayout());

    for (auto &p : desc.pushConstantRanges) p.stageFlags = desc.stage;

    VkPipelineLayoutCreateInfo layoutCreateInfo{
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = (u32)sets.getSize(),
        .pSetLayouts            = sets.getData(),
        .pushConstantRangeCount = (u32)desc.pushConstantRanges.getSize(),
        .pPushConstantRanges    = desc.pushConstantRanges.getData(),
    };

    vkCreatePipelineLayout(device->getVkDevice(), &layoutCreateInfo, nullptr, &pipelineLayout);

    VkShaderCreateInfoEXT createInfo{
        .sType                  = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .stage                  = desc.stage,
        .nextStage              = desc.nextStage,
        .codeType               = desc.codeType,
        .codeSize               = desc.code.getSize(),
        .pCode                  = desc.code.getData(),
        .pName                  = desc.name,
        .setLayoutCount         = (u32)sets.getSize(),
        .pSetLayouts            = sets.getData(),
        .pushConstantRangeCount = (u32)desc.pushConstantRanges.getSize(),
        .pPushConstantRanges    = desc.pushConstantRanges.getData(),
        .pSpecializationInfo    = nullptr,
    };

    device->vkCreateShadersEXT(device->getVkDevice(), 1, &createInfo, nullptr, &shader);
}

Shader::~Shader() {
    vkDestroyPipelineLayout(device->getVkDevice(), pipelineLayout, nullptr);
    device->vkDestroyShaderEXT(device->getVkDevice(), shader, nullptr);
}

Vec<u8> Shader::getBinaryData() {
    u64 size;
    device->vkGetShaderBinaryDataEXT(device->getVkDevice(), shader, &size, nullptr);
    Vec<u8> data(size);
    device->vkGetShaderBinaryDataEXT(device->getVkDevice(), shader, &size, data.getData());
    return data;
}