#include "Descriptor.hpp"

#include "Device.hpp"

DescriptorSetLayout::DescriptorSetLayout(Device *_device, const Vec<VkDescriptorSetLayoutBinding> &bindings)
    : device(_device) {
    VkDescriptorSetLayoutCreateInfo createInfo{
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = (u32)bindings.getSize(),
        .pBindings    = bindings.getData(),
    };
    vkCreateDescriptorSetLayout(device->getVkDevice(), &createInfo, nullptr, &layout);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device->getVkDevice(), layout, nullptr);
}

u64 DescriptorSetLayout::getSize() {
    u64 size;
    device->vkGetDescriptorSetLayoutSizeEXT(device->getVkDevice(), layout, &size);
    return size;
}

u64 DescriptorSetLayout::getBindingOffset(u32 binding) {
    u64 offset;
    device->vkGetDescriptorSetLayoutBindingOffsetEXT(device->getVkDevice(), layout, binding, &offset);
    return offset;
}