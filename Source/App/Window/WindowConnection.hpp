#pragma once

#include "Window.hpp"

class WindowConnection {
   protected:
    WindowConnection() = default;

   public:
    static WindowConnection* create();

    virtual ~WindowConnection()    = default;
    virtual Window* createWindow() = 0;
    virtual void update()          = 0;
};