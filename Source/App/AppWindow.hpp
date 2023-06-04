#pragma once

#include "Core/Core.hpp"
#include "Core/Math.hpp"
#include "GpuApi/GpuApi.hpp"

class App;
class Window;
class UIRenderer;

class AppWindow {
   private:
    App* app;
    Window* window;

    Device* device;
    Surface* surface;
    SurfaceConfig config{};
    VkSurfaceFormatKHR surfaceFormat{};
    u32 imageIndex = 0;
    Semaphore* imageAvailable;
    Semaphore* uiRenderFinished;

    u32 width = 800, height = 600;

    UIRenderer* uiRenderer;

   public:
    explicit AppWindow(App* app, const String& title);
    ~AppWindow();

    void update();

    inline App* getApp() { return app; }

   private:
    void resize();
};
