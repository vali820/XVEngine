#include "X11Connection.hpp"

#include "X11Window.hpp"

X11Connection::X11Connection() {
    connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) throw std::runtime_error("Failed to connect to X11 server");
    screen           = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    protocolsAtom    = getInternAtom("WM_PROTOCOLS");
    deleteWindowAtom = getInternAtom("WM_DELETE_WINDOW");
}

X11Connection::~X11Connection() { xcb_disconnect(connection); }

void X11Connection::update() {
    for (X11Window* w : windows) w->update();
}

Window* X11Connection::createWindow() { return new X11Window(this, 800, 600); }

xcb_atom_t X11Connection::getInternAtom(const String& name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, name.getSize(), name.getData());
    xcb_intern_atom_reply_t* reply  = xcb_intern_atom_reply(connection, cookie, nullptr);
    xcb_atom_t atom                 = reply->atom;
    delete reply;
    return atom;
}