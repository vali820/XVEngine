#include "WlWindow.hpp"

#include "WlConnection.hpp"

WlWindow::WlWindow(WlConnection *_connection) : connection(_connection) {
    connection->windows.push(this);
    surface    = wl_compositor_create_surface(connection->compositor);
    xdgSurface = xdg_wm_base_get_xdg_surface(connection->wmBase, surface);
    xdg_surface_add_listener(xdgSurface, &xdgSurfaceListener, this);
    toplevel = xdg_surface_get_toplevel(xdgSurface);
    xdg_toplevel_add_listener(toplevel, &toplevelListener, this);

    zxdg_toplevel_decoration_v1 *decoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(connection->decorationManager, toplevel);
    zxdg_toplevel_decoration_v1_set_mode(decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);

    wl_surface_commit(surface);
}

WlWindow::~WlWindow() {
    for (u64 i = 0; i < connection->windows.getSize(); i++) {
        if (connection->windows[i] == this) {
            connection->windows.remove(i);
        }
    }
    xdg_toplevel_destroy(toplevel);
    xdg_surface_destroy(xdgSurface);
    wl_surface_destroy(surface);
}

wl_display *WlWindow::getWlDisplay() { return connection->display; }

void WlWindow::show() {}
void WlWindow::hide() {}

void WlWindow::minimize() { xdg_toplevel_set_minimized(toplevel); }

void WlWindow::setMaximized(bool value) {
    if (value)
        xdg_toplevel_set_maximized(toplevel);
    else
        xdg_toplevel_unset_maximized(toplevel);
}

void WlWindow::setFullscreen(bool value) {
    if (value)
        xdg_toplevel_set_fullscreen(toplevel, nullptr);
    else
        xdg_toplevel_unset_fullscreen(toplevel);
}

void WlWindow::resize(u32 _width, u32 _height) {
    width = (i32)_width, height = (i32)_height;
    xdg_surface_set_window_geometry(xdgSurface, 0, 0, (i32)width, (i32)height);
    wl_surface_commit(surface);
}

void WlWindow::setTitle(const String &title) { xdg_toplevel_set_title(toplevel, title.getData()); }

void WlWindow::xdgSurfaceHandleConfigure(void *data, xdg_surface *surface, u32 serial) {
    auto self = (WlWindow *)data;
    xdg_surface_ack_configure(surface, serial);
    wl_surface_commit(self->surface);
}

// This is where we receive resize events
void WlWindow::toplevelHandleConfigure(void *data, xdg_toplevel *, i32 width, i32 height, wl_array *) {
    auto self = (WlWindow *)data;
    if (!width or !height) return;
    self->width = width, self->height = height;
    self->resizeF(ResizeEvent(width, height));
}

void WlWindow::toplevelHandleClose(void *data, xdg_toplevel *) {
    auto self = (WlWindow *)data;
    self->exitF();
}

void WlWindow::toplevelHandleConfigureBounds(void *, xdg_toplevel *, i32, i32) {}

void WlWindow::toplevelHandleWmCapabilities(void *, xdg_toplevel *, wl_array *) {}