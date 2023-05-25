#pragma once

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "../Event/Event.hpp"
#include "Core/Core.hpp"
#include "ThirdParty/wayland/xdg-decoration.h"
#include "ThirdParty/wayland/xdg-shell.h"

class WlWindow;

class WlConnection {
    // WlConnection and WlWindow are tightly coupled
    // This may be bad practice, but keeping them separate is impossible as they need each other to function
    friend class WlWindow;

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

    xkb_context* context;
    xkb_keymap* keymap{};
    xkb_state* state{};
    xkb_state* textState{};

    wl_compositor* compositor{};
    xdg_wm_base* wmBase{};
    wl_seat* seat{};
    zxdg_decoration_manager_v1* decorationManager{};

    wl_keyboard* keyboard{};
    wl_pointer* pointer{};

    Vec<WlWindow*> windows;
    wl_surface* pointerCurrentSurface{};
    wl_surface* keyboardCurrentSurface{};

    WlPointerEvent pointerEvent{};
    i32 lastMouseX{}, lastMouseY{};

    u32 shiftIdx{};
    u32 ctrlIdx{};
    u32 altIdx{};
    u32 capsLockIdx{};
    u32 numLockIdx{};
    Modifiers modifiers{0};

   public:
    WlConnection();
    ~WlConnection();

    void update();

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
