#pragma once

#include "Core/Core.hpp"

class AppWindow;
class Device;
class WlConnection;
class X11Connection;

enum class WindowingSystem {
    None,
    X11,
    Wayland,
    Windows,
    MacOS,
};

/**
 * @brief Base application class
 */
class App {
    friend class AppWindow;

   private:
    const String name;
    Vec<AppWindow*> windows;
    const WindowingSystem windowingSystem = WindowingSystem::None;

    Device* device;
    WlConnection* wlConnection{};
    X11Connection* x11Connection{};

    bool running = true;

   protected:
    virtual void init() {}

   public:
    explicit App(const String& name);
    ~App();

    void run();

    void addWindow(AppWindow* window);

    inline void exit() { running = false; }

    [[nodiscard]] inline const String& getName() const { return name; }
    [[nodiscard]] inline WindowingSystem getWindowingSystem() const { return windowingSystem; }
    inline Device* getDevice() { return device; }
};