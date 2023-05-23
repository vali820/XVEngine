#include "Shader.hpp"

#include <vulkan/vulkan_core.h>

#include "DescriptorSet.hpp"
#include "Device.hpp"

Shader::Shader(Device *_device, const ShaderDesc &desc) : device(_device) {
    Vec<VkDescriptorSetLayout> sets;
    for (auto set : desc.setLayouts) sets.push(set->getVkDescriptorSetLayout());

    VkShaderCreateInfoEXT createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT,
        .stage = desc.stage,
        .nextStage = desc.nextStage,
        .codeType = desc.codeType,
        .codeSize = desc.code.getSize(),
        .pCode = desc.code.getData(),
        .pName = desc.name,
        .setLayoutCount = (u32)sets.getSize(),
        .pSetLayouts = sets.getData(),
        .pushConstantRangeCount = (u32)desc.pushConstantRanges.getSize(),
        .pPushConstantRanges = desc.pushConstantRanges.getData(),
        .pSpecializationInfo = nullptr,
    };

    device->vkCreateShadersEXT(device->getVkDevice(), 1, &createInfo, nullptr, &shader);
}

Shader::~Shader() { device->vkDestroyShaderEXT(device->getVkDevice(), shader, nullptr); }

Vec<u8> Shader::getBinaryData() {
    u64 size;
    device->vkGetShaderBinaryDataEXT(device->getVkDevice(), shader, &size, nullptr);
    Vec<u8> data(size);
    device->vkGetShaderBinaryDataEXT(device->getVkDevice(), shader, &size, data.getData());
    return data;
}