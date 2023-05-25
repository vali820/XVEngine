#pragma once

#include <xcb/xcb.h>

#include "Window.hpp"

class X11Connection;

class X11Window : public Window {
    friend class X11Connection;

   private:
    X11Connection* connection;
    xcb_connection_t* xcbCon;
    xcb_screen_t* screen;
    xcb_window_t id;

    u16 width{}, height{};

   public:
    X11Window(X11Connection* connection, u32 width, u32 height);
    ~X11Window() override;

    void update();

    void show() override;
    void hide() override;
    void minimize() override;
    void setMaximized(bool value) override;
    void setFullscreen(bool value) override;
    void setTitle(const String& title) override;
    void resize(u32 width, u32 height) override;

    u32 getWidth() override;
    u32 getHeight() override;

    xcb_connection_t* getXcbConnection();
    [[nodiscard]] inline xcb_window_t getXcbWindow() const { return id; }

   private:
    void handleEvent(xcb_generic_event_t* event);
};
