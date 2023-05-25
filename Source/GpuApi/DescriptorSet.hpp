#pragma once

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"

class Device;

class DescriptorSetLayout {
   private:
    Device* device;
    VkDescriptorSetLayout layout{};

   public:
    DescriptorSetLayout(Device* device, const Vec<VkDescriptorSetLayoutBinding>& bindings);

    ~DescriptorSetLayout();

    inline Device* getDevice() { return device; }
    inline VkDescriptorSetLayout getVkDescriptorSetLayout() { return layout; }
};