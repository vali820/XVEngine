#pragma once

#include "Core/Core.hpp"

class AppWindow;
class Device;
class WindowConnection;

/**
 * @brief Base application class
 */
class App {
    friend class AppWindow;

   private:
    const String name;
    Vec<AppWindow*> windows;

    Device* device;
    WindowConnection* windowConnection;

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
    inline Device* getDevice() { return device; }
};