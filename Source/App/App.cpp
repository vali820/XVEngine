#include "App.hpp"

#include "AppWindow.hpp"
#include "GpuApi/Device.hpp"

#ifdef __linux__
#include "Window/WlConnection.hpp"
#include "Window/X11Connection.hpp"
#endif

WindowingSystem determineWindowingSystem() {
#ifdef __linux__
    std::string xdgType = std::getenv("XDG_SESSION_TYPE");
    if (xdgType == "wayland")
        return WindowingSystem::Wayland;
    else if (xdgType == "x11")
        return WindowingSystem::X11;
#endif

    return WindowingSystem::None;
}

App::App(const String& _name) : name(_name), windowingSystem(determineWindowingSystem()) {
    device = new Device;
#ifdef __linux__
    if (windowingSystem == WindowingSystem::Wayland)
        wlConnection = new WlConnection;
    else if (windowingSystem == WindowingSystem::X11)
        x11Connection = new X11Connection;
#endif
}

App::~App() {
    for (AppWindow* w : windows) delete w;

#ifdef __linux__
    delete wlConnection;
    delete x11Connection;
#endif

    delete device;
}

void App::run() {
    init();

    while (running) {
#ifdef __linux__
        if (windowingSystem == WindowingSystem::Wayland) wlConnection->update();
        if (windowingSystem == WindowingSystem::X11) x11Connection->update();
#endif
        for (AppWindow* w : windows) w->update();
    }

    device->waitIdle();
}

void App::addWindow(AppWindow* window) { windows.push(window); }