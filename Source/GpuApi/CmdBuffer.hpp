#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"

class Device;
class Buffer;

class CmdPool {
    friend Device;

   private:
    Device* device;
    VkCommandPool pool;
    const bool owned;

    CmdPool(Device* device, VkCommandPool pool, bool _owned = true);

   public:
    ~CmdPool();

    inline Device* getDevice() { return device; }
    inline VkCommandPool getVkCommandPool() { return pool; }
};

class Shader;

class CmdBuffer {
   private:
    Device* device;
    CmdPool* cmdPool;
    VkCommandBufferLevel level;
    VkCommandBuffer cmdBuffer{};

   public:
    explicit CmdBuffer(CmdPool* cmdPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    ~CmdBuffer();

    // Basic
    void reset();
    void begin(bool oneTimeSubmit = false);
    void defaultState();
    void end();
    void beginRendering(const RenderingInfo& info);
    void endRendering();
    void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);
    void drawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);
    void copyBuffer(Buffer* src, Buffer* dst, u64 size, u64 srcOffset = 0, u64 dstOffset = 0);
    void pushConstant(Shader* shader, u32 offset, u32 size, void* data);

    void imageMemoryBarrier(VkImageMemoryBarrier2 barrier);

    // Binding
    void bindShader(VkShaderStageFlagBits stage, Shader* shader);
    void bindVertexBuffer(Buffer* buffer, u32 bindingIndex);
    void bindIndexBuffer(Buffer* buffer, VkIndexType indexType);
    void bindDescriptorBuffers(const Vec<DescriptorBufferBindingInfo>& bindingInfos);

    void setViewport(VkViewport viewport);
    void setScissor(VkRect2D scissor);
    void setRasterizerDiscardEnable(bool enable);

    void setVertexInput(const Vec<VertexBinding>& bindings, const Vec<VertexAttribute>& attributes);
    void setPrimitiveTopology(VkPrimitiveTopology topology);
    void setPrimitiveRestartEnable(bool enable);

    // Rasterization state
    void setRasterizationSamples(VkSampleCountFlagBits samples);
    void setAlphaToCoverageEnable(bool enable);
    void setPolygonMode(VkPolygonMode polygonMode);
    void setLineWidth(f32 width);
    void setLineRasterizationMode(VkLineRasterizationModeEXT mode);
    void setLineStippleEnable(bool enable);
    void setLineStipple(u32 factor, u16 pattern);
    void setCullMode(VkCullModeFlagBits cullMode);
    void setFrontFace(VkFrontFace frontFace);
    void setDepthTestEnable(bool enable);
    void setDepthWriteEnable(bool enable);
    void setDepthCompareOp(VkCompareOp op);
    void setDepthBiasEnable(bool enable);
    void setDepthBias(f32 constantFactor, f32 clamp, f32 slopeFactor);
    void setStencilTestEnable(bool enable);
    void setStencilOp(VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp,
                      VkStencilOp depthFailOp, VkCompareOp compareOp);

    // Color blending state
    void setColorBlendEnable(u32 attachment, bool enable);
    void setColorBlendEquation(u32 attachment, VkColorBlendEquationEXT equation);
    void setColorWriteMask(u32 firstAttachment, const Vec<VkColorComponentFlags>& masks);

    inline Device* getDevice() { return device; }
    inline VkCommandBuffer getVkCommandBuffer() { return cmdBuffer; }
};
