#pragma once

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"
#include "ThirdParty/vma/vk_mem_alloc.h"

class Device;

class Buffer {
   protected:
    Device* device;
    VkBuffer buffer{};
    VmaAllocation allocation{};
    VmaAllocationInfo allocationInfo{};

    u64 size;

   public:
    Buffer(Device* device, u64 size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags = 0);

    ~Buffer();

    inline Device* getDevice() { return device; }
    inline VkBuffer getVkBuffer() { return buffer; }
    inline VmaAllocation getVmaAllocation() { return allocation; }
    [[nodiscard]] inline u64 getSize() const { return size; }
};

class CpuVisibleBuffer : public Buffer {
   public:
    CpuVisibleBuffer(Device* device, u64 size, VkBufferUsageFlags usage);

    [[nodiscard]] inline void* getData() const { return allocationInfo.pMappedData; }
};

class GpuLocalBuffer : public Buffer {
   public:
    GpuLocalBuffer(Device* device, u64 size, VkBufferUsageFlags usage);
};