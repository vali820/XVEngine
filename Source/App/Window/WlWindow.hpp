#pragma once

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "Core/Core.hpp"
#include "ThirdParty/wayland/xdg-decoration.h"
#include "ThirdParty/wayland/xdg-shell.h"
#include "Window.hpp"

class WlConnection;

class WlWindow : public Window {
    friend class WlConnection;

   private:
    WlConnection* connection;

    wl_surface* surface;
    xdg_surface* xdgSurface;
    xdg_toplevel* toplevel;

    i32 width = 0, height = 0;

   public:
    explicit WlWindow(WlConnection* connection);
    ~WlWindow() override;

    wl_display* getWlDisplay();
    inline wl_surface* getWlSurface() { return surface; }

    inline u32 getWidth() override { return width; }
    inline u32 getHeight() override { return height; }

    void show() override;
    void hide() override;

    void minimize() override;
    void setMaximized(bool value) override;
    void setFullscreen(bool value) override;
    void resize(u32 width, u32 height) override;
    void setTitle(const String& title) override;

   private:
    const xdg_surface_listener xdgSurfaceListener{
        .configure = xdgSurfaceHandleConfigure,
    };
    static void xdgSurfaceHandleConfigure(void* data, xdg_surface* surface, u32 serial);

    const xdg_toplevel_listener toplevelListener{
        .configure        = toplevelHandleConfigure,
        .close            = toplevelHandleClose,
        .configure_bounds = toplevelHandleConfigureBounds,
        .wm_capabilities  = toplevelHandleWmCapabilities,
    };
    static void toplevelHandleConfigure(void* data, xdg_toplevel* toplevel, i32 width, i32 height,
                                        wl_array* array);
    static void toplevelHandleClose(void* data, xdg_toplevel* toplevel);
    static void toplevelHandleConfigureBounds(void* data, xdg_toplevel* toplevel, i32 width, i32 height);
    static void toplevelHandleWmCapabilities(void* data, xdg_toplevel* toplevel, wl_array* capabilities);
};
