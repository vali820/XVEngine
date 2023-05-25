#pragma once

#include <xcb/xcb.h>

#include "Core/Core.hpp"

class X11Window;

class X11Connection {
    friend class X11Window;

   private:
    xcb_connection_t* connection;
    xcb_screen_t* screen;

    xcb_atom_t deleteWindowAtom;
    xcb_atom_t protocolsAtom;

    Vec<X11Window*> windows;

   public:
    X11Connection();
    ~X11Connection();

    void update();

   private:
    xcb_atom_t getInternAtom(const String& name);
};
