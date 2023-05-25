#pragma once

class Device;

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"
#include "ThirdParty/vma/vk_mem_alloc.h"

class Buffer {
   private:
    Device* device;
    VkBuffer buffer{};
    VmaAllocation allocation{};
    VmaAllocationInfo allocationInfo{};

    u64 size;

   public:
    Buffer(Device* device, u64 size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags);

    ~Buffer();

    inline Device* getDevice() { return device; }
    inline VkBuffer getVkBuffer() { return buffer; }
    inline VmaAllocation getVmaAllocation() { return allocation; }
    [[nodiscard]] inline u64 getSize() const { return size; }
    [[nodiscard]] inline void* getData() const { return allocationInfo.pMappedData; }
};
