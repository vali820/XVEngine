#include "App.hpp"

#include "AppWindow.hpp"
#include "GpuApi/Device.hpp"
#include "Window/WindowConnection.hpp"

App::App(const String& _name) : name(_name) {
    device           = new Device;
    windowConnection = WindowConnection::create();
}

App::~App() {
    for (AppWindow* w : windows) delete w;

    delete windowConnection;
    delete device;
}

void App::run() {
    init();

    while (running) {
        windowConnection->update();
        for (AppWindow* w : windows) w->update();
    }

    device->waitIdle();
}

void App::addWindow(AppWindow* window) { windows.push(window); }