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
    delete child;
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
    drawData.setViewport(viewport);
    // draw window background
    drawData.addRoundedRect({0, 0, viewport.getBottomRight()}, 12, 1);
    drawData.setViewport({viewport.getPosition() + 6, viewport.getSize() - 12});

    if (child) {
        Vec2 minSize = child->minSize();
        if (minSize.x <= viewport.w and minSize.y <= viewport.h) {
            child->draw(drawData, minSize);
        }
    }

    uiRenderer->render(drawData, imageIndex, imageAvailable, uiRenderFinished);

    surface->getImages()[imageIndex]->transitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    device->getGraphicsQueue()->present({uiRenderFinished}, surface, imageIndex);
}

void AppWindow::resize() {
    config.width = width, config.height = height;
    viewport = {2, 2, f32(width - 4), f32(height - 4)};
    surface->configure(config);
    uiRenderer->resize(width, height);
}

void AppWindow::setChild(Widget* _child) { child = _child; }