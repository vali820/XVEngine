#include "Semaphore.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "Device.hpp"

Semaphore::Semaphore(Device *_device, bool timeline, u64 value)
    : device(_device), _isTimeline(timeline) {
    VkSemaphoreTypeCreateInfo typeCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = value,
    };
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = 0,
    };
    if (timeline) createInfo.pNext = &typeCreateInfo;

    vkCreateSemaphore(device->getVkDevice(), &createInfo, nullptr, &semaphore);
}

Semaphore::~Semaphore() { vkDestroySemaphore(device->getVkDevice(), semaphore, nullptr); }

u64 Semaphore::getValue() {
    u64 value;
    vkGetSemaphoreCounterValue(device->getVkDevice(), semaphore, &value);
    return value;
}

void Semaphore::wait(u64 timeout, u64 value) {
    VkSemaphoreWaitInfo waitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &semaphore,
        .pValues = &value,
    };
    vkWaitSemaphores(device->getVkDevice(), &waitInfo, timeout);
}

void Semaphore::signal(u64 value) {
    VkSemaphoreSignalInfo signalInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
        .semaphore = semaphore,
        .value = value,
    };
    vkSignalSemaphore(device->getVkDevice(), &signalInfo);
}