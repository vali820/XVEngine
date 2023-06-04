#include "Buffer.hpp"

#include "Device.hpp"

Buffer::Buffer(Device *_device, u64 _size, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags)
    : device(_device), size(_size) {
    VkBufferCreateInfo createInfo{
        .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size        = size,
        .usage       = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    VmaAllocationCreateInfo allocationCreateInfo{
        .flags = allocationFlags,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    vmaCreateBuffer(
        device->getVmaAllocator(), &createInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);
}

Buffer::~Buffer() { vmaDestroyBuffer(device->getVmaAllocator(), buffer, allocation); }

CpuVisibleBuffer::CpuVisibleBuffer(Device *_device, u64 _size, VkBufferUsageFlags usage)
    : Buffer(_device, _size, usage,
             VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT) {}

GpuLocalBuffer::GpuLocalBuffer(Device *_device, u64 _size, VkBufferUsageFlags usage)
    : Buffer(_device, _size, usage) {}