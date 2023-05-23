#pragma once

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "ThirdParty/wayland/xdg-decoration.h"
#include "ThirdParty/wayland/xdg-shell.h"
#include "Window.hpp"

class WaylandWindow : public Window {
    friend Window;

   private:
    enum {
        PointerEventMaskEnter                 = 1 << 0,
        PointerEventMaskLeave                 = 1 << 1,
        PointerEventMaskMotion                = 1 << 2,
        PointerEventMaskButton                = 1 << 3,
        PointerEventMaskAxis                  = 1 << 4,
        PointerEventMaskAxisSource            = 1 << 5,
        PointerEventMaskAxisStop              = 1 << 6,
        PointerEventMaskAxisValue120          = 1 << 7,
        PointerEventMaskAxisRelativeDirection = 1 << 8,
    };

    struct WlAxis {
        bool valid;
        wl_fixed_t value;
        i32 value120;
        u32 direction;
    };

    struct WlPointerEvent {
        u32 mask;
        wl_fixed_t x, y;
        u32 button;
        u32 state;
        u32 time;
        u32 serial;
        WlAxis axes[2];
        u32 axisSource;
    };

   private:
    wl_display* display;
    wl_registry* registry;

    wl_surface* surface;
    xdg_surface* xdgSurface;
    xdg_toplevel* toplevel;

    wl_compositor* compositor{};
    xdg_wm_base* wmBase{};
    wl_seat* seat{};
    zxdg_decoration_manager_v1* decorationManager{};

    wl_pointer* pointer{};
    wl_keyboard* keyboard{};

    i32 width, height;

    // Current mouse event
    WlPointerEvent pointerEvent{};

    // For delta mouse movement
    i32 lastMouseX = 0, lastMouseY = 0;

    xkb_context* context;
    xkb_keymap* keymap{};
    xkb_state* state{};
    xkb_state* textState{};

    u32 shiftIdx{};
    u32 ctrlIdx{};
    u32 altIdx{};
    u32 capsLockIdx{};
    u32 numLockIdx{};
    Modifiers modifiers{0};

   public:
    WaylandWindow(u32 width, u32 height);

    ~WaylandWindow() override;

    inline wl_display* getWlDisplay() { return display; }
    inline wl_surface* getWlSurface() { return surface; }

    void update() override;

    void show() override;
    void hide() override;

    void minimize() override;
    void setMaximized(bool value) override;
    void setFullscreen(bool value) override;
    void resize(u32 width, u32 height) override;
    void setTitle(const std::string& title) override;

    inline u32 getWidth() override { return width; }
    inline u32 getHeight() override { return height; }

   private:
    const wl_registry_listener registryListener{
        .global        = registryHandleGlobal,
        .global_remove = registryHandleGlobalRemove,
    };
    static void registryHandleGlobal(void* data, wl_registry* registry, u32 name, const char* interface,
                                     u32 version);
    static void registryHandleGlobalRemove(void* data, wl_registry* registry, u32 name);

    const xdg_wm_base_listener wmBaseListener{
        .ping = wmBaseHandlePing,
    };
    static void wmBaseHandlePing(void* data, xdg_wm_base* wmBase, u32 serial);

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

    const wl_seat_listener seatListener{
        .capabilities = seatHandleCapabilities,
        .name         = seatHandleName,
    };
    static void seatHandleCapabilities(void* data, wl_seat* seat, u32 capabilities);
    static void seatHandleName(void* data, wl_seat* seat, const char* name);

    const wl_pointer_listener pointerListener{
        .enter                   = pointerHandleEnter,
        .leave                   = pointerHandleLeave,
        .motion                  = pointerHandleMotion,
        .button                  = pointerHandleButton,
        .axis                    = pointerHandleAxis,
        .frame                   = pointerHandleFrame,
        .axis_source             = pointerHandleAxisSource,
        .axis_stop               = pointerHandleAxisStop,
        .axis_discrete           = pointerHandleAxisDiscrete,
        .axis_value120           = pointerHandleAxisValue120,
        .axis_relative_direction = pointerHandleAxisRelativeDirection,
    };
    static void pointerHandleEnter(void* data, wl_pointer* pointer, u32 serial, wl_surface* surface,
                                   wl_fixed_t x, wl_fixed_t y);
    static void pointerHandleLeave(void* data, wl_pointer* pointer, u32 serial, wl_surface* surface);
    static void pointerHandleMotion(void* data, wl_pointer* pointer, u32 time, wl_fixed_t x, wl_fixed_t y);
    static void pointerHandleButton(void* data, wl_pointer* pointer, u32 serial, u32 time, u32 button,
                                    u32 state);
    static void pointerHandleAxis(void* data, wl_pointer* pointer, u32 time, u32 axis, wl_fixed_t value);
    static void pointerHandleFrame(void* data, wl_pointer* pointer);
    static void pointerHandleAxisSource(void* data, wl_pointer* pointer, u32 axisSource);
    static void pointerHandleAxisStop(void* data, wl_pointer* pointer, u32 time, u32 axis);
    static void pointerHandleAxisDiscrete(void* data, wl_pointer* pointer, u32 axis, i32 discrete);
    static void pointerHandleAxisValue120(void* data, wl_pointer* pointer, u32 axis, i32 value120);
    static void pointerHandleAxisRelativeDirection(void* data, wl_pointer* pointer, u32 axis, u32 direction);

    const wl_keyboard_listener keyboardListener{
        .keymap      = keyboardHandleKeymap,
        .enter       = keyboardHandleEnter,
        .leave       = keyboardHandleLeave,
        .key         = keyboardHandleKey,
        .modifiers   = keyboardHandleModifiers,
        .repeat_info = keyboardHandleRepeatInfo,
    };
    static void keyboardHandleKeymap(void* data, wl_keyboard* keyboard, u32 format, i32 fd, u32 size);
    static void keyboardHandleEnter(void* data, wl_keyboard* keyboard, u32 serial, wl_surface* surface,
                                    wl_array* keys);
    static void keyboardHandleLeave(void* data, wl_keyboard* keyboard, u32 serial, wl_surface* surface);
    static void keyboardHandleKey(void* data, wl_keyboard* keyboard, u32 serial, u32 time, u32 key,
                                  u32 state);
    static void keyboardHandleModifiers(void* data, wl_keyboard* keyboard, u32 serial, u32 modsDepressed,
                                        u32 modsLatched, u32 modsLocked, u32 group);
    static void keyboardHandleRepeatInfo(void* data, wl_keyboard* keyboard, i32 rate, i32 delay);
};
