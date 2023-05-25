#include "WlConnection.hpp"

#include <sys/mman.h>

#include "LinuxCommon.hpp"
#include "WlWindow.hpp"

WlConnection::WlConnection() {
    context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    display = wl_display_connect(nullptr);
    if (!display) throw std::runtime_error("Failed to connect to wayland compositor");
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registryListener, this);
    wl_display_roundtrip(display);
}

WlConnection::~WlConnection() {
    wl_registry_destroy(registry);
    wl_display_disconnect(display);

    xkb_state_unref(state);
    xkb_state_unref(textState);
    xkb_keymap_unref(keymap);
    xkb_context_unref(context);
}

void WlConnection::update() { wl_display_dispatch_pending(display); }

/******************
 * Base callbacks *
 ******************/

void WlConnection::registryHandleGlobal(void *data, wl_registry *registry, u32 name, const char *interface,
                                        u32 version) {
    auto self = (WlConnection *)data;

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

void WlConnection::registryHandleGlobalRemove(void *, wl_registry *, u32) {}

void WlConnection::wmBaseHandlePing(void *, xdg_wm_base *wmBase, u32 serial) {
    xdg_wm_base_pong(wmBase, serial);
}

void WlConnection::seatHandleCapabilities(void *data, wl_seat *seat, u32 capabilities) {
    auto self = (WlConnection *)data;

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

void WlConnection::seatHandleName(void *, wl_seat *, const char *) {}

/*******************
 * Mouse callbacks *
 *******************/

void WlConnection::pointerHandleEnter(void *data, wl_pointer *, u32 serial, wl_surface *surface, wl_fixed_t x,
                                      wl_fixed_t y) {
    auto self = (WlConnection *)data;

    self->pointerCurrentSurface = surface;

    self->pointerEvent.mask   |= PointerEventMaskEnter;
    self->pointerEvent.serial  = serial;
    self->pointerEvent.x       = x;
    self->pointerEvent.y       = y;
}

void WlConnection::pointerHandleLeave(void *data, wl_pointer *, u32 serial, wl_surface *) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask   |= PointerEventMaskLeave;
    self->pointerEvent.serial  = serial;
}

void WlConnection::pointerHandleMotion(void *data, wl_pointer *, u32 time, wl_fixed_t x, wl_fixed_t y) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask |= PointerEventMaskMotion;
    self->pointerEvent.time  = time;
    self->pointerEvent.x     = x;
    self->pointerEvent.y     = y;
}

void WlConnection::pointerHandleButton(void *data, wl_pointer *, u32 serial, u32 time, u32 button,
                                       u32 state) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask   |= PointerEventMaskButton;
    self->pointerEvent.serial  = serial;
    self->pointerEvent.time    = time;
    self->pointerEvent.button  = button;
    self->pointerEvent.state   = state;
}

void WlConnection::pointerHandleAxis(void *data, wl_pointer *, u32 time, u32 axis, wl_fixed_t value) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask             |= PointerEventMaskAxis;
    self->pointerEvent.time              = time;
    self->pointerEvent.axes[axis].valid  = true;
    self->pointerEvent.axes[axis].valid  = value;
}

void WlConnection::pointerHandleAxisSource(void *data, wl_pointer *, u32 axisSource) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask       |= PointerEventMaskAxisSource;
    self->pointerEvent.axisSource  = axisSource;
}

void WlConnection::pointerHandleAxisStop(void *data, wl_pointer *, u32 time, u32 axis) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask             |= PointerEventMaskAxisStop;
    self->pointerEvent.time              = time;
    self->pointerEvent.axes[axis].valid  = true;
}

// axis-discrete is deprecated with wl_pointer version 8, won't be sent anymore
void WlConnection::pointerHandleAxisDiscrete(void *, wl_pointer *, u32, i32) {}

void WlConnection::pointerHandleAxisValue120(void *data, wl_pointer *, u32 axis, i32 value120) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask                |= PointerEventMaskAxisValue120;
    self->pointerEvent.axes[axis].valid     = true;
    self->pointerEvent.axes[axis].value120  = value120;
}

void WlConnection::pointerHandleAxisRelativeDirection(void *data, wl_pointer *, u32 axis, u32 direction) {
    auto self = (WlConnection *)data;

    self->pointerEvent.mask                 |= PointerEventMaskAxisRelativeDirection;
    self->pointerEvent.axes[axis].valid      = true;
    self->pointerEvent.axes[axis].direction  = direction;
}

void WlConnection::pointerHandleFrame(void *data, wl_pointer *) {
    auto self             = (WlConnection *)data;
    WlPointerEvent &event = self->pointerEvent;
    WlWindow *window;
    for (WlWindow *w : self->windows) {
        if (w->surface == self->pointerCurrentSurface) window = w;
    }

    if (event.mask & PointerEventMaskMotion) {
        i32 x = wl_fixed_to_int(event.x), y = wl_fixed_to_int(event.y);
        window->mouseMoveF({x, y, x - self->lastMouseX, y - self->lastMouseY});
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
            window->mouseButtonPressedF(MouseButtonPressedEvent(button));
        else
            window->mouseButtonReleasedF(MouseButtonReleasedEvent(button));
    }

    // TODO: Axis events and enter/leave

    event = {};
}

/**********************
 * Keyboard callbacks *
 **********************/

void WlConnection::keyboardHandleKeymap(void *data, wl_keyboard *, u32 format, i32 fd, u32 size) {
    auto self = (WlConnection *)data;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        throw std::runtime_error("Unsupported wayland keymap format");

    void *ptr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);

    xkb_state_unref(self->state);
    xkb_state_unref(self->textState);
    xkb_keymap_unref(self->keymap);
    self->keymap = xkb_keymap_new_from_string(
        self->context, (char *)ptr, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    self->state     = xkb_state_new(self->keymap);
    self->textState = xkb_state_new(self->keymap);

    self->shiftIdx    = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_SHIFT);
    self->ctrlIdx     = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_CTRL);
    self->altIdx      = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_ALT);
    self->capsLockIdx = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_CAPS);
    self->numLockIdx  = xkb_keymap_mod_get_index(self->keymap, XKB_MOD_NAME_NUM);

    munmap(ptr, size);
    close(fd);
}

void WlConnection::keyboardHandleEnter(void *data, wl_keyboard *, u32, wl_surface *surface, wl_array *) {
    auto self = (WlConnection *)data;

    self->keyboardCurrentSurface = surface;
}

void WlConnection::keyboardHandleLeave(void *, wl_keyboard *, u32, wl_surface *) {}

void WlConnection::keyboardHandleKey(void *data, wl_keyboard *, u32, u32, u32 key, u32 state) {
    auto self = (WlConnection *)data;
    WlWindow *window;
    for (WlWindow *w : self->windows) {
        if (w->surface == self->keyboardCurrentSurface) window = w;
    }

    u32 keycode      = key + 8;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(self->state, keycode);
    Key _key         = xkbKeysymToKey(sym);

    // TODO: text input event
    //    char buf[64];
    //    bool isText = xkb_state_key_get_utf8(self->textState, keycode, buf, 64);

    if (state)
        window->keyPressedF(KeyPressedEvent(_key, self->modifiers));
    else
        window->keyReleasedF(KeyReleasedEvent(_key, self->modifiers));
}

void WlConnection::keyboardHandleModifiers(void *data, wl_keyboard *, u32, u32 modsDepressed, u32 modsLatched,
                                           u32 modsLocked, u32 group) {
    auto self = (WlConnection *)data;

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

void WlConnection::keyboardHandleRepeatInfo(void *, wl_keyboard *, i32, i32) {}