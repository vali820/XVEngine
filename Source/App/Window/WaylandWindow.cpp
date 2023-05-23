#include "WaylandWindow.hpp"

#include <sys/mman.h>

#include "LinuxCommon.hpp"

WaylandWindow::WaylandWindow(u32 _width, u32 _height) : width((i32)_width), height((i32)_height) {
    // XKB
    context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Connection
    display = wl_display_connect(nullptr);
    if (!display) throw std::runtime_error("Failed to connect to wayland compositor");
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registryListener, this);
    wl_display_roundtrip(display);

    // Window
    surface    = wl_compositor_create_surface(compositor);
    xdgSurface = xdg_wm_base_get_xdg_surface(wmBase, surface);
    xdg_surface_add_listener(xdgSurface, &xdgSurfaceListener, this);
    toplevel = xdg_surface_get_toplevel(xdgSurface);
    xdg_toplevel_add_listener(toplevel, &toplevelListener, this);

    // Server side decorations. Should remove when ui is implemented
    zxdg_toplevel_decoration_v1 *toplevelDecoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(decorationManager, toplevel);
    zxdg_toplevel_decoration_v1_set_mode(toplevelDecoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);

    wl_surface_commit(surface);
}

WaylandWindow::~WaylandWindow() {
    xdg_toplevel_destroy(toplevel);
    xdg_surface_destroy(xdgSurface);
    wl_surface_destroy(surface);
    wl_registry_destroy(registry);
    wl_display_disconnect(display);

    xkb_state_unref(state);
    xkb_keymap_unref(keymap);
    xkb_context_unref(context);
}

void WaylandWindow::update() { wl_display_dispatch_pending(display); }

void WaylandWindow::minimize() { xdg_toplevel_set_minimized(toplevel); }

void WaylandWindow::setMaximized(bool value) {
    if (value)
        xdg_toplevel_set_maximized(toplevel);
    else
        xdg_toplevel_unset_maximized(toplevel);
}

void WaylandWindow::setFullscreen(bool value) {
    if (value)
        xdg_toplevel_set_fullscreen(toplevel, nullptr);
    else
        xdg_toplevel_unset_fullscreen(toplevel);
}

void WaylandWindow::resize(u32 _width, u32 _height) {
    width = (i32)_width, height = (i32)_height;
    xdg_surface_set_window_geometry(xdgSurface, 0, 0, (i32)width, (i32)height);
    wl_surface_commit(surface);
}

void WaylandWindow::setTitle(const std::string &title) { xdg_toplevel_set_title(toplevel, title.data()); }

/******************
 * Base callbacks *
 ******************/

void WaylandWindow::registryHandleGlobal(void *data, wl_registry *registry, u32 name, const char *interface,
                                         u32 version) {
    auto self = (WaylandWindow *)data;

    // wl_compositor
    if (std::strcmp(interface, wl_compositor_interface.name) == 0) {
        self->compositor = (wl_compositor *)wl_registry_bind(registry, name, &wl_compositor_interface, 5);
    }
    // wl_seat
    else if (std::strcmp(interface, wl_seat_interface.name) == 0) {
        self->seat = (wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 8);
        wl_seat_add_listener(self->seat, &self->seatListener, self);
    }
    // xdg_wm_base
    else if (std::strcmp(interface, xdg_wm_base_interface.name) == 0) {
        self->wmBase = (xdg_wm_base *)wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(self->wmBase, &self->wmBaseListener, self);
    }
    // zxdg_decoration_manager_v1
    else if (std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
        self->decorationManager = (zxdg_decoration_manager_v1 *)wl_registry_bind(
            registry, name, &zxdg_decoration_manager_v1_interface, version);
    }
}

void WaylandWindow::registryHandleGlobalRemove(void *, wl_registry *, u32) {}

void WaylandWindow::wmBaseHandlePing(void *, xdg_wm_base *wmBase, u32 serial) {
    xdg_wm_base_pong(wmBase, serial);
}

void WaylandWindow::xdgSurfaceHandleConfigure(void *data, xdg_surface *surface, u32 serial) {
    auto self = (WaylandWindow *)data;
    xdg_surface_ack_configure(surface, serial);
    wl_surface_commit(self->surface);
}

// This is where we receive resize events
void WaylandWindow::toplevelHandleConfigure(void *data, xdg_toplevel *, i32 width, i32 height, wl_array *) {
    auto self = (WaylandWindow *)data;
    if (!width or !height) return;
    self->width = width, self->height = height;
    self->resizeF(ResizeEvent(width, height));
}

void WaylandWindow::toplevelHandleClose(void *data, xdg_toplevel *) {
    auto self = (WaylandWindow *)data;
    self->exitF();
}

void WaylandWindow::toplevelHandleConfigureBounds(void *, xdg_toplevel *, i32, i32) {}

void WaylandWindow::toplevelHandleWmCapabilities(void *, xdg_toplevel *, wl_array *) {}

// Get mouse and keyboard if available
void WaylandWindow::seatHandleCapabilities(void *data, wl_seat *seat, u32 capabilities) {
    auto self       = (WaylandWindow *)data;

    bool hasPointer = capabilities & WL_SEAT_CAPABILITY_POINTER;
    if (hasPointer and !self->pointer) {
        self->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(self->pointer, &self->pointerListener, self);
    } else if (!hasPointer and self->pointer) {
        wl_pointer_release(self->pointer);
        self->pointer = nullptr;
    }

    bool hasKeyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;
    if (hasKeyboard and !self->keyboard) {
        self->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(self->keyboard, &self->keyboardListener, self);
    } else if (!hasKeyboard and self->pointer) {
        wl_keyboard_release(self->keyboard);
        self->keyboard = nullptr;
    }
}

void WaylandWindow::seatHandleName(void *, wl_seat *, const char *) {}

/*******************
 * Mouse callbacks *
 *******************/

void WaylandWindow::pointerHandleEnter(void *data, wl_pointer *, u32 serial, wl_surface *, wl_fixed_t x,
                                       wl_fixed_t y) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskEnter;
    self->pointerEvent.serial = serial;
    self->pointerEvent.x      = x;
    self->pointerEvent.y      = y;
}

void WaylandWindow::pointerHandleLeave(void *data, wl_pointer *, u32 serial, wl_surface *) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskLeave;
    self->pointerEvent.serial = serial;
}

void WaylandWindow::pointerHandleMotion(void *data, wl_pointer *, u32 time, wl_fixed_t x, wl_fixed_t y) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskMotion;
    self->pointerEvent.time = time;
    self->pointerEvent.x    = x;
    self->pointerEvent.y    = y;
}

void WaylandWindow::pointerHandleButton(void *data, wl_pointer *, u32 serial, u32 time, u32 button,
                                        u32 state) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskButton;
    self->pointerEvent.serial = serial;
    self->pointerEvent.time   = time;
    self->pointerEvent.button = button;
    self->pointerEvent.state  = state;
}

void WaylandWindow::pointerHandleAxis(void *data, wl_pointer *, u32 time, u32 axis, wl_fixed_t value) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskAxis;
    self->pointerEvent.time             = time;
    self->pointerEvent.axes[axis].valid = true;
    self->pointerEvent.axes[axis].valid = value;
}

void WaylandWindow::pointerHandleAxisSource(void *data, wl_pointer *, u32 axisSource) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskAxisSource;
    self->pointerEvent.axisSource = axisSource;
}

void WaylandWindow::pointerHandleAxisStop(void *data, wl_pointer *, u32 time, u32 axis) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskAxisStop;
    self->pointerEvent.time             = time;
    self->pointerEvent.axes[axis].valid = true;
}

// axis-discrete is deprecated with wl_pointer version 8, won't be sent anymore
void WaylandWindow::pointerHandleAxisDiscrete(void *, wl_pointer *, u32, i32) {}

void WaylandWindow::pointerHandleAxisValue120(void *data, wl_pointer *, u32 axis, i32 value120) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskAxisValue120;
    self->pointerEvent.axes[axis].valid    = true;
    self->pointerEvent.axes[axis].value120 = value120;
}

void WaylandWindow::pointerHandleAxisRelativeDirection(void *data, wl_pointer *, u32 axis, u32 direction) {
    auto self = (WaylandWindow *)data;

    self->pointerEvent.mask |= PointerEventMaskAxisRelativeDirection;
    self->pointerEvent.axes[axis].valid     = true;
    self->pointerEvent.axes[axis].direction = direction;
}

void WaylandWindow::pointerHandleFrame(void *data, wl_pointer *) {
    auto self             = (WaylandWindow *)data;
    WlPointerEvent &event = self->pointerEvent;

    if (event.mask & PointerEventMaskMotion) {
        i32 x = wl_fixed_to_int(event.x), y = wl_fixed_to_int(event.y);
        self->mouseMoveF({x, y, x - self->lastMouseX, y - self->lastMouseY});
        self->lastMouseX = x, self->lastMouseY = y;
    } else if (event.mask & PointerEventMaskButton) {
        MouseButton button;
        switch (event.button) {
            case 0x110: button = MouseButton::Left; break;
            case 0x111: button = MouseButton::Right; break;
            case 0x112: button = MouseButton::Middle; break;
            case 0x113: button = MouseButton::_4; break;
            case 0x114: button = MouseButton::_5; break;
        }
        if (event.state)
            self->mouseButtonPressedF(MouseButtonPressedEvent(button));
        else
            self->mouseButtonReleasedF(MouseButtonReleasedEvent(button));
    }

    // TODO: Axis events and enter/leave

    event = {};
}

/**********************
 * Keyboard callbacks *
 **********************/

void WaylandWindow::keyboardHandleKeymap(void *data, wl_keyboard *, u32 format, i32 fd, u32 size) {
    auto self = (WaylandWindow *)data;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        throw std::runtime_error("Unsupported wayland keymap format");

    void *ptr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    xkb_state_unref(self->state);
    xkb_state_unref(self->textState);
    xkb_keymap_unref(self->keymap);
    self->keymap = xkb_keymap_new_from_string(
        self->context, (char *)ptr, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    self->state       = xkb_state_new(self->keymap);
    self->textState   = xkb_state_new(self->keymap);

    self->shiftIdx    = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_SHIFT);
    self->ctrlIdx     = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_CTRL);
    self->altIdx      = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_ALT);
    self->capsLockIdx = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_CAPS);
    self->numLockIdx  = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_NUM);

    munmap(ptr, size);
    close(fd);
}

void WaylandWindow::keyboardHandleEnter(void *, wl_keyboard *, u32, wl_surface *, wl_array *) {}
void WaylandWindow::keyboardHandleLeave(void *, wl_keyboard *, u32, wl_surface *) {}

void WaylandWindow::keyboardHandleKey(void *data, wl_keyboard *, u32, u32, u32 key, u32 state) {
    auto self        = (WaylandWindow *)data;

    u32 keycode      = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(self->state, keycode);
    Key _key         = xkbKeysymToKey(sym);

    // TODO: input event
    //    char buf[64];
    //    bool isText = xkb_state_key_get_utf8(self->textState, keycode, buf, 64);

    if (state)
        self->keyPressedF(KeyPressedEvent(_key, self->modifiers));
    else
        self->keyReleasedF(KeyReleasedEvent(_key, self->modifiers));
}

void WaylandWindow::keyboardHandleModifiers(void *data, wl_keyboard *, u32, u32 modsDepressed,
                                            u32 modsLatched, u32 modsLocked, u32 group) {
    auto self = (WaylandWindow *)data;

    xkb_state_update_mask(self->textState, modsDepressed, modsLatched, modsLocked, 0, 0, group);

    self->modifiers.setShift(
        xkb_state_mod_index_is_active(self->textState, self->shiftIdx, XKB_STATE_MODS_EFFECTIVE));
    self->modifiers.setCtrl(
        xkb_state_mod_index_is_active(self->textState, self->ctrlIdx, XKB_STATE_MODS_EFFECTIVE));
    self->modifiers.setAlt(
        xkb_state_mod_index_is_active(self->textState, self->altIdx, XKB_STATE_MODS_EFFECTIVE));
    self->modifiers.setCapsLock(
        xkb_state_mod_index_is_active(self->textState, self->capsLockIdx, XKB_STATE_MODS_EFFECTIVE));
    self->modifiers.setNumLock(
        xkb_state_mod_index_is_active(self->textState, self->numLockIdx, XKB_STATE_MODS_EFFECTIVE));
}

void WaylandWindow::keyboardHandleRepeatInfo(void *, wl_keyboard *, i32, i32) {}

// TODO
void WaylandWindow::show() {}
void WaylandWindow::hide() {}