#include "DescriptorSet.hpp"

#include "Device.hpp"

DescriptorSetLayout::DescriptorSetLayout(Device *_device,
                                         const Vec<VkDescriptorSetLayoutBinding> &bindings)
    : device(_device) {
    VkDescriptorSetLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = (u32)bindings.getSize(),
        .pBindings = bindings.getData(),
    };
    vkCreateDescriptorSetLayout(device->getVkDevice(), &createInfo, nullptr, &layout);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device->getVkDevice(), layout, nullptr);
}