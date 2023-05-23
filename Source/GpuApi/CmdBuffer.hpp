#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"

class Device;

class CmdPool {
    friend Device;

   private:
    Device* device;
    VkCommandPool pool;
    const bool owned;

    CmdPool(Device* _device, VkCommandPool _pool, bool _owned = true);

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
    CmdBufferLevel level;
    VkCommandBuffer cmdBuffer{};

   public:
    explicit CmdBuffer(CmdPool* _cmdPool, CmdBufferLevel _level = CmdBufferLevel::Primary);
    ~CmdBuffer();

    // Basic
    void reset();
    void begin();
    void end();
    void beginRendering(const RenderingInfo& info);
    void endRendering();
    void draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance);

    void barrier(const Vec<VkImageMemoryBarrier2>& imageBarriers);

    // Binding
    void bindShader(VkShaderStageFlagBits stage, Shader* shader);

    void setViewport(VkViewport viewport);
    void setScissor(VkRect2D scissor);
    void setRasterizerDiscardEnable(bool enable);

    void setVertexInput(const Vec<VertexBinding>& bindings, const Vec<VertexAttribute>& attributes);
    void setPrimitiveTopology(VkPrimitiveTopology topology);
    void setPrimitiveRestartEnable(bool enable);

    // Rasterization state
    void setRasterizationSamples(VkSampleCountFlagBits samples);
    void setSampleMask(VkSampleCountFlagBits samples, const Vec<u32>& masks);
    void setAlphaToCoverageEnable(bool enable);
    void setPolygonMode(VkPolygonMode polygonMode);
    void setLineWidth(f32 width);
    void setCullMode(VkCullModeFlagBits cullMode);
    void setFrontFace(VkFrontFace frontFace);
    void setDepthTestEnable(bool enable);
    void setDepthWriteEnable(bool enable);
    void setDepthBiasEnable(bool enable);
    void setStencilTestEnable(bool enable);

    // Color blending state
    void setColorBlendEnable(u32 firstAttachment, const Vec<bool>& enables);
    void setColorBlendEquation(u32 firstAttachment, const Vec<VkColorBlendEquationEXT>& equations);
    void setColorWriteMask(u32 firstAttachment, const Vec<VkColorComponentFlags>& masks);

    inline Device* getDevice() { return device; }
    inline VkCommandBuffer getVkCommandBuffer() { return cmdBuffer; }
};
