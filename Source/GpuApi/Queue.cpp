#include "Queue.hpp"

#include "CmdBuffer.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "Surface.hpp"

Queue::Queue(Device *_device, u32 _familyIndex) : device(_device), familyIndex(_familyIndex) {
    vkGetDeviceQueue(device->getVkDevice(), familyIndex, 0, &queue);
}

void Queue::submit(const Vec<CmdBuffer *> &_cmdBuffers, const Vec<Semaphore *> &_waitSemaphores,
                   const Vec<Semaphore *> &_signalSemaphores, const Vec<VkPipelineStageFlags> &waitStageMask,
                   Fence *fence) {
    Vec<VkCommandBuffer> cmdBuffers;
    Vec<VkSemaphore> waitSemaphores;
    Vec<VkSemaphore> signalSemaphores;

    for (CmdBuffer *c : _cmdBuffers) cmdBuffers.push(c->getVkCommandBuffer());
    for (Semaphore *s : _waitSemaphores) waitSemaphores.push(s->getVkSemaphore());
    for (Semaphore *s : _signalSemaphores) signalSemaphores.push(s->getVkSemaphore());

    VkSubmitInfo submitInfo{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = (u32)waitSemaphores.getSize(),
        .pWaitSemaphores      = waitSemaphores.getData(),
        .pWaitDstStageMask    = waitStageMask.getData(),
        .commandBufferCount   = (u32)cmdBuffers.getSize(),
        .pCommandBuffers      = cmdBuffers.getData(),
        .signalSemaphoreCount = (u32)signalSemaphores.getSize(),
        .pSignalSemaphores    = signalSemaphores.getData(),
    };

    VkFence f = nullptr;
    if (fence) f = fence->getVkFence();
    vkQueueSubmit(queue, 1, &submitInfo, f);
}

VkResult Queue::present(const Vec<Semaphore *> &_waitSemaphores, Surface *surface, u32 imageIndex) {
    Vec<VkSemaphore> waitSemaphores;
    for (Semaphore *s : _waitSemaphores) waitSemaphores.push(s->getVkSemaphore());

    VkSwapchainKHR swapchain = surface->getVkSwapchain();
    VkResult result;
    VkPresentInfoKHR presentInfo{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = (u32)waitSemaphores.getSize(),
        .pWaitSemaphores    = waitSemaphores.getData(),
        .swapchainCount     = 1,
        .pSwapchains        = &swapchain,
        .pImageIndices      = &imageIndex,
        .pResults           = &result,
    };

    vkQueuePresentKHR(queue, &presentInfo);
    return result;
}

void Queue::waitIdle() { vkQueueWaitIdle(queue); }