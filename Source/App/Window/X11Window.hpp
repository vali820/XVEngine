#pragma once

#include <xcb/xcb.h>

#include "Window.hpp"

class X11Window : public Window {
    friend Window;

   private:
    xcb_connection_t* con;
    xcb_screen_t* screen;
    xcb_window_t id;

    xcb_atom_t deleteWindowAtom;
    xcb_atom_t protocolsAtom;

    X11Window(u32 _width, u32 _height);

   public:
    ~X11Window() override;

    void update() override;

    void show() override;
    void hide() override;
    void minimize() override;
    void setMaximized(bool value) override;
    void setFullscreen(bool value) override;
    void setTitle(const std::string& title) override;
    void resize(u32 width, u32 height) override;

    u32 getWidth() override;
    u32 getHeight() override;

    inline xcb_connection_t* getXcbConnection() { return con; }
    [[nodiscard]] inline xcb_window_t getXcbWindowId() const { return id; }

   private:
    void handleEvent(xcb_generic_event_t* event) const;
    xcb_atom_t getInternAtom(const std::string& name);
};
