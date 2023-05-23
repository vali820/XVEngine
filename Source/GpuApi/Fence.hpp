#pragma once

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"

class Device;

class Fence {
   private:
    Device* device;
    VkFence fence{};

   public:
    explicit Fence(Device* _device, bool createSignaled = false);
    ~Fence();

    bool isSignaled();

    void reset();

    void waitFor(u64 timeout);

    inline Device* getDevice() { return device; }
    inline VkFence getVkFence() { return fence; }
};
