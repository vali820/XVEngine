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

CmdBuffer::CmdBuffer(CmdPool *_cmdPool, VkCommandBufferLevel _level)
    : device(_cmdPool->getDevice()), cmdPool(_cmdPool), level(_level) {
    VkCommandBufferAllocateInfo allocateInfo{
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = cmdPool->getVkCommandPool(),
        .level              = level,
        .commandBufferCount = 1,
    };
    vkAllocateCommandBuffers(device->getVkDevice(), &allocateInfo, &cmdBuffer);
}

CmdBuffer::~CmdBuffer() {
    vkFreeCommandBuffers(device->getVkDevice(), cmdPool->getVkCommandPool(), 1, &cmdBuffer);
}

void CmdBuffer::reset() { vkResetCommandBuffer(cmdBuffer, 0); }

void CmdBuffer::begin(bool oneTimeSubmit) {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    if (oneTimeSubmit) beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuffer, &beginInfo);
}

void CmdBuffer::defaultState() {
    setRasterizerDiscardEnable(false);
    setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    setPrimitiveRestartEnable(false);
    setRasterizationSamples(VK_SAMPLE_COUNT_1_BIT);
    setAlphaToCoverageEnable(false);
    setPolygonMode(VK_POLYGON_MODE_FILL);
    setCullMode(VK_CULL_MODE_BACK_BIT);
    setFrontFace(VK_FRONT_FACE_CLOCKWISE);
    setDepthTestEnable(false);
    setDepthWriteEnable(false);
    setDepthBiasEnable(false);
    setStencilTestEnable(false);
    setLineWidth(1.0);
    setLineRasterizationMode(VK_LINE_RASTERIZATION_MODE_DEFAULT_EXT);
    setColorBlendEnable(0, false);
    setColorWriteMask(0, {COLOR_COMPONENTS_ALL});
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
    VkBuffer buf = buffer ? buffer->getVkBuffer() : nullptr;
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

void CmdBuffer::imageMemoryBarrier(VkImageMemoryBarrier2 barrier) {
    VkDependencyInfo info{
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .memoryBarrierCount       = 0,
        .bufferMemoryBarrierCount = 0,
        .imageMemoryBarrierCount  = 1,
        .pImageMemoryBarriers     = &barrier,
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
    u32 masks[2] = {UINT32_MAX, UINT32_MAX};
    device->vkCmdSetSampleMaskEXT(cmdBuffer, samples, masks);
}

void CmdBuffer::setAlphaToCoverageEnable(bool enable) {
    device->vkCmdSetAlphaToCoverageEnableEXT(cmdBuffer, enable);
}

void CmdBuffer::setPolygonMode(VkPolygonMode polygonMode) {
    device->vkCmdSetPolygonModeEXT(cmdBuffer, polygonMode);
}

void CmdBuffer::setLineWidth(f32 width) { vkCmdSetLineWidth(cmdBuffer, width); }

void CmdBuffer::setLineRasterizationMode(VkLineRasterizationModeEXT mode) {
    device->vkCmdSetLineRasterizationModeEXT(cmdBuffer, mode);
}

void CmdBuffer::setLineStippleEnable(bool enable) { device->vkCmdSetLineStippleEnableEXT(cmdBuffer, enable); }

void CmdBuffer::setLineStipple(u32 factor, u16 pattern) {
    device->vkCmdSetLineStippleEXT(cmdBuffer, factor, pattern);
}

void CmdBuffer::setCullMode(VkCullModeFlagBits cullMode) { vkCmdSetCullMode(cmdBuffer, cullMode); }

void CmdBuffer::setFrontFace(VkFrontFace frontFace) { vkCmdSetFrontFace(cmdBuffer, frontFace); }

void CmdBuffer::setDepthTestEnable(bool enable) { vkCmdSetDepthTestEnable(cmdBuffer, enable); }

void CmdBuffer::setDepthWriteEnable(bool enable) { vkCmdSetDepthWriteEnable(cmdBuffer, enable); }

void CmdBuffer::setDepthCompareOp(VkCompareOp op) { vkCmdSetDepthCompareOp(cmdBuffer, op); }

void CmdBuffer::setDepthBiasEnable(bool enable) { vkCmdSetDepthBiasEnable(cmdBuffer, enable); }

void CmdBuffer::setDepthBias(f32 constantFactor, f32 clamp, f32 slopeFactor) {
    vkCmdSetDepthBias(cmdBuffer, constantFactor, clamp, slopeFactor);
}

void CmdBuffer::setStencilTestEnable(bool enable) { vkCmdSetStencilTestEnable(cmdBuffer, enable); }

void CmdBuffer::setStencilOp(VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp,
                             VkStencilOp depthFailOp, VkCompareOp compareOp) {
    vkCmdSetStencilOp(cmdBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

void CmdBuffer::setColorBlendEnable(u32 attachment, bool enable) {
    VkBool32 enabled = enable;
    device->vkCmdSetColorBlendEnableEXT(cmdBuffer, attachment, 1, &enabled);
}

void CmdBuffer::setColorBlendEquation(u32 attachment, VkColorBlendEquationEXT equation) {
    device->vkCmdSetColorBlendEquationEXT(cmdBuffer, attachment, 1, &equation);
}

void CmdBuffer::setColorWriteMask(u32 firstAttachment, const Vec<VkColorComponentFlags> &masks) {
    device->vkCmdSetColorWriteMaskEXT(cmdBuffer, firstAttachment, masks.getSize(), masks.getData());
}
