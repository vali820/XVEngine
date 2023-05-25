#pragma once

#include <vulkan/vulkan.h>

#include "Core/Core.hpp"
#include "Device.hpp"

class CmdBuffer;
class Semaphore;
class Fence;
class Surface;

class Queue {
    friend Device;

   private:
    Device* device;
    VkQueue queue{};
    u32 familyIndex;

    Queue(Device* device, u32 familyIndex);

   public:
    void submit(const Vec<CmdBuffer*>& cmdBuffers, const Vec<Semaphore*>& waitSemaphores,
                const Vec<Semaphore*>& signalSemaphores,
                const Vec<VkPipelineStageFlags>& waitStageMask, Fence* fence);

    VkResult present(const Vec<Semaphore*>& waitSemaphores, Surface* surface, u32 imageIndex);

    inline Device* getDevice() { return device; }
    inline VkQueue getVkQueue() { return queue; }
    [[nodiscard]] inline u32 getFamilyIndex() const { return familyIndex; }
};
