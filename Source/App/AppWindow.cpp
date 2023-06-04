#include "AppWindow.hpp"

#include "App.hpp"
#include "GpuApi/GpuApi.hpp"
#include "Input/Event.hpp"
#include "Render/UIRenderer.hpp"
#include "Window/Window.hpp"
#include "Window/WindowConnection.hpp"

AppWindow::AppWindow(App* _app, const String& title) : app(_app), device(app->getDevice()) {
    window = app->windowConnection->createWindow();

    window->setTitle(title);
    window->setExitCallback([this]() { app->exit(); });
    window->setResizeCallback([this](const ResizeEvent& e) {
        width = e.width, height = e.height;
        resize();
    });

    surface       = new Surface(device, window);
    surfaceFormat = surface->getSupportedFormats()[0];
    config        = {
               .format      = surfaceFormat.format,
               .colorSpace  = surfaceFormat.colorSpace,
               .presentMode = VK_PRESENT_MODE_FIFO_KHR,
               .width       = 800,
               .height      = 600,
    };

    uiRenderFinished = new Semaphore(device);
    imageAvailable   = new Semaphore(device);

    uiRenderer = new UIRenderer(device, surface, width, height);

    resize();
}

AppWindow::~AppWindow() {
    delete uiRenderFinished, delete imageAvailable;
    delete uiRenderer;
    delete surface;
    delete window;
}

void AppWindow::update() {
    surface->getNextImageIndex(UINT64_MAX, imageAvailable, nullptr, imageIndex);

    surface->getImages()[imageIndex]->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    UIDrawData drawData;
    drawData.setColor({0.1, 0.1, 0.1, 1.0});
    drawData.setSecondaryColor({0.3, 0.3, 0.3, 1.0});
    drawData.addRoundedRect({100, 100, 200, 100}, 20, 2);
    drawData.addRoundedRect({150, 150, 200, 100}, 20, 1);
    drawData.addRoundedRect({200, 200, 200, 100}, 20, 0.5);

    uiRenderer->render(drawData, imageIndex, imageAvailable, uiRenderFinished);

    surface->getImages()[imageIndex]->transitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    device->getGraphicsQueue()->present({uiRenderFinished}, surface, imageIndex);
}

void AppWindow::resize() {
    config.width = width, config.height = height;
    surface->configure(config);
    uiRenderer->resize(width, height);
}