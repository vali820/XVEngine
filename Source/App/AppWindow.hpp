#pragma once

#include "Core/Core.hpp"
#include "Core/Math.hpp"
#include "GpuApi/GpuApi.hpp"
#include "UI/UI.hpp"

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
    Rect viewport;

    UIRenderer* uiRenderer;

    Widget* child;

   public:
    explicit AppWindow(App* app, const String& title);
    ~AppWindow();

    void setChild(Widget* child);

    void update();

    inline App* getApp() { return app; }

   private:
    void resize();
};
