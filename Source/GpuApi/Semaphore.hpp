#pragma once

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"

class Device;

class Semaphore {
   private:
    Device* device;
    VkSemaphore semaphore{};
    const bool _isTimeline;

   public:
    explicit Semaphore(Device* device, bool timeline = false, u64 value = 0);
    ~Semaphore();

    u64 getValue();

    void wait(u64 timeout, u64 value = 0);

    void signal(u64 value = 0);

    inline Device* getDevice() { return device; }
    inline VkSemaphore getVkSemaphore() { return semaphore; }
    [[nodiscard]] inline bool isTimeline() const { return _isTimeline; }
};
