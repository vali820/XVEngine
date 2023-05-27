#include "CmdBuffer.hpp"

#include "Buffer.hpp"
#include "Common.hpp"
#include "Device.hpp"
#include "Shader.hpp"

CmdPool::CmdPool(Device *_device, VkCommandPool _pool, bool _owned)
    : device(_device), pool(_pool), owned(_owned) {}

CmdPool::~CmdPool() {
    if (owned) {
        vkDestroyCommandPool(device->getVkDevice(), pool, nullptr);
    }
}

CmdBuffer::CmdBuffer(CmdPool *_cmdPool, CmdBufferLevel _level)
    : device(_cmdPool->getDevice()), cmdPool(_cmdPool), level(_level) {
    VkCommandBufferAllocateInfo allocateInfo{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = cmdPool->getVkCommandPool(),
        .level              = (VkCommandBufferLevel)level,
        .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(device->getVkDevice(), &allocateInfo, &cmdBuffer);
}

CmdBuffer::~CmdBuffer() {
    vkFreeCommandBuffers(device->getVkDevice(), cmdPool->getVkCommandPool(), 1, &cmdBuffer);
}

void CmdBuffer::reset() { vkResetCommandBuffer(cmdBuffer, 0); }

void CmdBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    vkBeginCommandBuffer(cmdBuffer, &beginInfo);
}

void CmdBuffer::end() { vkEndCommandBuffer(cmdBuffer); }

void CmdBuffer::beginRendering(const RenderingInfo &info) {
    Vec<VkRenderingAttachmentInfo> colorAttachments;
    for (const auto &a : info.colorAttachments) colorAttachments.push(a.getVkInfo());
    VkRenderingAttachmentInfo depthAttachment   = info.depthAttachment.getVkInfo();
    VkRenderingAttachmentInfo stencilAttachment = info.stencilAttachment.getVkInfo();
    VkRenderingInfo renderingInfo{
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea           = info.renderArea,
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = (u32)colorAttachments.getSize(),
        .pColorAttachments    = colorAttachments.getData(),
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = &stencilAttachment,
    };

    vkCmdBeginRendering(cmdBuffer, &renderingInfo);
}

void CmdBuffer::endRendering() { vkCmdEndRendering(cmdBuffer); }

void CmdBuffer::draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) {
    vkCmdDraw(cmdBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CmdBuffer::drawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset,
                            u32 firstInstance) {
    vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CmdBuffer::copyBuffer(Buffer *src, Buffer *dst, u64 size, u64 srcOffset, u64 dstOffset) {
    VkBufferCopy copy{
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size      = size,
    };
    vkCmdCopyBuffer(cmdBuffer, src->getVkBuffer(), dst->getVkBuffer(), 1, &copy);
}

void CmdBuffer::pushConstant(Shader *shader, u32 offset, u32 size, void *data) {
    vkCmdPushConstants(cmdBuffer, shader->getVkPipelineLayout(), shader->getStage(), offset, size, data);
}

void CmdBuffer::bindShader(VkShaderStageFlagBits stage, Shader *shader) {
    VkShaderEXT s = shader->getVkShader();
    device->vkCmdBindShadersEXT(cmdBuffer, 1, &stage, &s);
}

void CmdBuffer::bindVertexBuffer(Buffer *buffer, u32 bindingIndex) {
    VkBuffer buf = buffer->getVkBuffer();
    u64 offset   = 0;
    vkCmdBindVertexBuffers(cmdBuffer, bindingIndex, 1, &buf, &offset);
}

void CmdBuffer::bindIndexBuffer(Buffer *buffer, VkIndexType indexType) {
    vkCmdBindIndexBuffer(cmdBuffer, buffer->getVkBuffer(), 0, indexType);
}

void CmdBuffer::bindDescriptorBuffers(const Vec<DescriptorBufferBindingInfo> &bindingInfos) {
    Vec<VkDescriptorBufferBindingInfoEXT> vkBindingInfos;
    for (const auto &info : bindingInfos) {
        vkBindingInfos.push({
            .sType   = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
            .address = info.address,
            .usage   = info.usage,
        });
    }
    device->vkCmdBindDescriptorBuffersEXT(cmdBuffer, vkBindingInfos.getSize(), vkBindingInfos.getData());
}

void CmdBuffer::barrier(const Vec<VkImageMemoryBarrier2> &imageBarriers) {
    VkDependencyInfo info{
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .memoryBarrierCount       = 0,
        .bufferMemoryBarrierCount = 0,
        .imageMemoryBarrierCount  = (u32)imageBarriers.getSize(),
        .pImageMemoryBarriers     = imageBarriers.getData(),
    };
    vkCmdPipelineBarrier2(cmdBuffer, &info);
}

void CmdBuffer::setViewport(VkViewport viewport) { vkCmdSetViewportWithCount(cmdBuffer, 1, &viewport); }

void CmdBuffer::setScissor(VkRect2D scissor) { vkCmdSetScissorWithCount(cmdBuffer, 1, &scissor); }

void CmdBuffer::setRasterizerDiscardEnable(bool enable) {
    vkCmdSetRasterizerDiscardEnable(cmdBuffer, enable);
}

void CmdBuffer::setVertexInput(const Vec<VertexBinding> &bindings, const Vec<VertexAttribute> &attributes) {
    Vec<VkVertexInputBindingDescription2EXT> vkBindings;
    Vec<VkVertexInputAttributeDescription2EXT> vkAttributes;

    for (const auto &b : bindings) {
        VkVertexInputBindingDescription2EXT binding{
            .sType     = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
            .binding   = b.binding,
            .stride    = b.stride,
            .inputRate = b.inputRate,
            .divisor   = 1,
        };
        vkBindings.push(binding);
    }
    for (const auto &a : attributes) {
        VkVertexInputAttributeDescription2EXT attribute{
            .sType    = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .location = a.location,
            .binding  = a.binding,
            .format   = a.format,
            .offset   = a.offset,
        };
        vkAttributes.push(attribute);
    }

    device->vkCmdSetVertexInputEXT(cmdBuffer,
                                   vkBindings.getSize(),
                                   vkBindings.getData(),
                                   vkAttributes.getSize(),
                                   vkAttributes.getData());
}

void CmdBuffer::setPrimitiveTopology(VkPrimitiveTopology topology) {
    vkCmdSetPrimitiveTopology(cmdBuffer, topology);
}

void CmdBuffer::setPrimitiveRestartEnable(bool enable) { vkCmdSetPrimitiveRestartEnable(cmdBuffer, enable); }

void CmdBuffer::setRasterizationSamples(VkSampleCountFlagBits samples) {
    device->vkCmdSetRasterizationSamplesEXT(cmdBuffer, samples);
}

void CmdBuffer::setSampleMask(VkSampleCountFlagBits samples, const Vec<u32> &masks) {
    device->vkCmdSetSampleMaskEXT(cmdBuffer, samples, masks.getData());
}

void CmdBuffer::setAlphaToCoverageEnable(bool enable) {
    device->vkCmdSetAlphaToCoverageEnableEXT(cmdBuffer, enable);
}

void CmdBuffer::setPolygonMode(VkPolygonMode polygonMode) {
    device->vkCmdSetPolygonModeEXT(cmdBuffer, polygonMode);
}

void CmdBuffer::setLineWidth(f32 width) { vkCmdSetLineWidth(cmdBuffer, width); }

void CmdBuffer::setCullMode(VkCullModeFlagBits cullMode) { vkCmdSetCullMode(cmdBuffer, cullMode); }

void CmdBuffer::setFrontFace(VkFrontFace frontFace) { vkCmdSetFrontFace(cmdBuffer, frontFace); }

void CmdBuffer::setDepthTestEnable(bool enable) { vkCmdSetDepthTestEnable(cmdBuffer, enable); }

void CmdBuffer::setDepthWriteEnable(bool enable) { vkCmdSetDepthWriteEnable(cmdBuffer, enable); }

void CmdBuffer::setDepthBiasEnable(bool enable) { vkCmdSetDepthBiasEnable(cmdBuffer, enable); }

void CmdBuffer::setStencilTestEnable(bool enable) { vkCmdSetStencilTestEnable(cmdBuffer, enable); }

void CmdBuffer::setColorBlendEnable(u32 firstAttachment, const Vec<bool> &enables) {
    Vec<VkBool32> enablesVk;
    for (bool b : enables) enablesVk.push(b);
    device->vkCmdSetColorBlendEnableEXT(cmdBuffer, firstAttachment, enablesVk.getSize(), enablesVk.getData());
}

void CmdBuffer::setColorBlendEquation(u32 firstAttachment, const Vec<VkColorBlendEquationEXT> &equations) {
    device->vkCmdSetColorBlendEquationEXT(
        cmdBuffer, firstAttachment, equations.getSize(), equations.getData());
}

void CmdBuffer::setColorWriteMask(u32 firstAttachment, const Vec<VkColorComponentFlags> &masks) {
    device->vkCmdSetColorWriteMaskEXT(cmdBuffer, firstAttachment, masks.getSize(), masks.getData());
}
