#include "LinuxCommon.hpp"

#include <xkbcommon/xkbcommon.h>

#include <Core/Core.hpp>

// I don't know how efficient this is, but seems better that a switch statement
const u32 KEY_MAP_MAX = 512;
bool keymapInit       = false;
Vec<Key> KEYMAP(KEY_MAP_MAX, Key::Unknown);
void initKeyMap() {
    // First part
    KEYMAP[XKB_KEY_space]      = Key::Space;
    KEYMAP[XKB_KEY_apostrophe] = Key::Apostrophe;
    KEYMAP[XKB_KEY_comma]      = Key::Comma;
    KEYMAP[XKB_KEY_minus]      = Key::Minus;
    KEYMAP[XKB_KEY_period]     = Key::Period;
    KEYMAP[XKB_KEY_slash]      = Key::Slash;
    for (int i = XKB_KEY_0, i2 = (i32)Key::Num0; i <= XKB_KEY_9 and i2 <= (i32)Key::Num9; i++, i2++)
        KEYMAP[i] = (Key)i2;
    KEYMAP[XKB_KEY_semicolon] = Key::Semicolon;
    KEYMAP[XKB_KEY_equal]     = Key::Equals;
    for (int i = XKB_KEY_A, i2 = (i32)Key::A; i <= XKB_KEY_Z and i2 <= (i32)Key::Z; i++, i2++)
        KEYMAP[i] = (Key)i2;
    KEYMAP[XKB_KEY_bracketleft]  = Key::LBracket;
    KEYMAP[XKB_KEY_backslash]    = Key::Backslash;
    KEYMAP[XKB_KEY_bracketright] = Key::RBracket;
    KEYMAP[XKB_KEY_grave]        = Key::Grave;
    for (int i = XKB_KEY_a, i2 = (i32)Key::A; i <= XKB_KEY_z and i2 <= (i32)Key::Z; i++, i2++)
        KEYMAP[i] = (Key)i2;

    // Second part, starts at 0xff00, so we place it at 0x100
    KEYMAP[XKB_KEY_BackSpace - 0xfe00]   = Key::Backspace;
    KEYMAP[XKB_KEY_Tab - 0xfe00]         = Key::Tab;
    KEYMAP[XKB_KEY_Return - 0xfe00]      = Key::Enter;
    KEYMAP[XKB_KEY_Pause - 0xfe00]       = Key::Pause;
    KEYMAP[XKB_KEY_Scroll_Lock - 0xfe00] = Key::ScrollLock;
    KEYMAP[XKB_KEY_Sys_Req - 0xfe00]     = Key::Print;
    KEYMAP[XKB_KEY_Escape - 0xfe00]      = Key::Escape;
    KEYMAP[XKB_KEY_Delete - 0xfe00]      = Key::Delete;
    KEYMAP[XKB_KEY_Home - 0xfe00]        = Key::Home;
    KEYMAP[XKB_KEY_Left - 0xfe00]        = Key::Left;
    KEYMAP[XKB_KEY_Up - 0xfe00]          = Key::Up;
    KEYMAP[XKB_KEY_Right - 0xfe00]       = Key::Right;
    KEYMAP[XKB_KEY_Down - 0xfe00]        = Key::Down;
    KEYMAP[XKB_KEY_Page_Up - 0xfe00]     = Key::PageUp;
    KEYMAP[XKB_KEY_Page_Down - 0xfe00]   = Key::PageDown;
    KEYMAP[XKB_KEY_End - 0xfe00]         = Key::End;
    KEYMAP[XKB_KEY_Insert - 0xfe00]      = Key::Insert;
    KEYMAP[XKB_KEY_Menu - 0xfe00]        = Key::Menu;
    KEYMAP[XKB_KEY_F1 - 0xfe00]          = Key::F1;
    KEYMAP[XKB_KEY_F2 - 0xfe00]          = Key::F2;
    KEYMAP[XKB_KEY_F3 - 0xfe00]          = Key::F3;
    KEYMAP[XKB_KEY_F4 - 0xfe00]          = Key::F4;
    KEYMAP[XKB_KEY_F5 - 0xfe00]          = Key::F5;
    KEYMAP[XKB_KEY_F6 - 0xfe00]          = Key::F6;
    KEYMAP[XKB_KEY_F7 - 0xfe00]          = Key::F7;
    KEYMAP[XKB_KEY_F8 - 0xfe00]          = Key::F8;
    KEYMAP[XKB_KEY_F9 - 0xfe00]          = Key::F9;
    KEYMAP[XKB_KEY_F10 - 0xfe00]         = Key::F10;
    KEYMAP[XKB_KEY_F11 - 0xfe00]         = Key::F11;
    KEYMAP[XKB_KEY_F12 - 0xfe00]         = Key::F12;
    KEYMAP[XKB_KEY_Shift_L - 0xfe00]     = Key::LShift;
    KEYMAP[XKB_KEY_Shift_R - 0xfe00]     = Key::RShift;
    KEYMAP[XKB_KEY_Control_L - 0xfe00]   = Key::LCtrl;
    KEYMAP[XKB_KEY_Control_R - 0xfe00]   = Key::RCtrl;
    KEYMAP[XKB_KEY_Caps_Lock - 0xfe00]   = Key::CapsLock;
    KEYMAP[XKB_KEY_Alt_L - 0xfe00]       = Key::LAlt;
    KEYMAP[XKB_KEY_Alt_R - 0xfe00]       = Key::RAlt;
    KEYMAP[XKB_KEY_Super_L - 0xfe00]     = Key::Super;

    keymapInit = true;
}

Key xkbKeysymToKey(u32 keysym) {
    if (!keymapInit) initKeyMap();

    if (keysym >= 0xff00 and keysym < 0x10000) keysym -= 0xfe00;  // control keys
    if (keysym >= KEY_MAP_MAX) return Key::Unknown;
    return KEYMAP[keysym];
}