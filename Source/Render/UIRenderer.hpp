#pragma once

#include "GpuApi/GpuApi.hpp"
#include "UI/DrawData.hpp"

class UIRenderer {
   private:
    Device* device;
    Surface* surface;
    RenderingInfo renderingInfo;

    CmdBuffer* cmdBuffer;
    Queue* queue;
    Fence* fence;

    Shader* vs;
    Shader* roundedBoxShader;

    Vec<UIDrawCmd> drawCommands;

    CpuVisibleBuffer* vertexBuffer;

    u32 width, height;

   public:
    UIRenderer(Device* device, Surface* surface, u32 width, u32 height);
    ~UIRenderer();

    void render(const UIDrawData& drawData,u32 imageIndex, Semaphore* imageAvailable, Semaphore* renderFinished);
    void resize(u32 width, u32 height);

   private:
    void recordCommandBuffer(const UIDrawData& drawData);
    void setVertexBufferRect(const Rect& rect);
};
