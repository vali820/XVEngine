#pragma once

#include "Core/Core.hpp"
#include "Core/Math.hpp"
#include "GpuApi/GpuApi.hpp"

class App;
class Window;

struct Vertex {
    FVec3 pos;
    FVec3 color;
};

class AppWindow {
   private:
    App* app;
    Window* window;

    Device* device;
    Surface* surface;
    CmdBuffer* cmdBuffer;
    Queue* queue;

    Semaphore* imageAvailable;
    Semaphore* renderFinished;
    Fence* fence;

    Shader* vs;
    Shader* fs;

    SurfaceConfig config{};
    VkSurfaceFormatKHR surfaceFormat{};
    RenderingInfo renderingInfo;
    u32 imageIndex = 0;

    Buffer* vertexBuffer{};
    Buffer* indexBuffer{};

    DescriptorSetLayout* descriptorSetLayout{};

    u32 width = 800, height = 600;

    f32 pushConstant = 0.0f;

   public:
    explicit AppWindow(App* app, const String& title);
    ~AppWindow();

    void update();

    inline App* getApp() { return app; }

   private:
    void recordCommandBuffer();
    Buffer* createBufferLocal(VkBufferUsageFlags usage, void* data, u64 size);
};
