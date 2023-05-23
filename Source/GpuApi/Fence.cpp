#include "Fence.hpp"

#include "Device.hpp"

Fence::Fence(Device *_device, bool createSignaled) : device(_device) {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };
    if (createSignaled) createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(device->getVkDevice(), &createInfo, nullptr, &fence);
}

Fence::~Fence() { vkDestroyFence(device->getVkDevice(), fence, nullptr); }

bool Fence::isSignaled() {
    VkResult result = vkGetFenceStatus(device->getVkDevice(), fence);
    if (result == VK_SUCCESS)
        return true;
    else if (result == VK_NOT_READY)
        return false;
    else
        throw std::runtime_error("Failed to get fence status");
}

void Fence::reset() { vkResetFences(device->getVkDevice(), 1, &fence); }

void Fence::waitFor(u64 timeout) {
    vkWaitForFences(device->getVkDevice(), 1, &fence, VK_FALSE, timeout);
}