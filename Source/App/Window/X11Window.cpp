#include "X11Window.hpp"

#include <xcb/xcb.h>

#include "App/Event/Event.hpp"

X11Window::X11Window(u32 _width, u32 _height) {
    con = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(con)) throw std::runtime_error("Failed to connect to X11 server");
    screen = xcb_setup_roots_iterator(xcb_get_setup(con)).data;
    id = xcb_generate_id(con);

    u32 mask = XCB_CW_EVENT_MASK;
    u32 value_list[1] = {XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                         XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE};

    xcb_create_window(con,
                      XCB_COPY_FROM_PARENT,
                      id,
                      screen->root,
                      0,
                      0,
                      _width,
                      _height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      mask,
                      value_list);

    protocolsAtom = getInternAtom("WM_PROTOCOLS");
    deleteWindowAtom = getInternAtom("WM_DELETE_WINDOW");
    xcb_change_property(
        con, XCB_PROP_MODE_REPLACE, id, protocolsAtom, XCB_ATOM_ATOM, 32, 1, &deleteWindowAtom);

    xcb_map_window(con, id);
    xcb_flush(con);
}

X11Window::~X11Window() { xcb_disconnect(con); }

void X11Window::update() {
    xcb_generic_event_t* event;
    while (true) {
        event = xcb_poll_for_event(con);
        if (!event) break;
        handleEvent(event);
    }
}

void X11Window::handleEvent(xcb_generic_event_t* event) const {
    switch (event->response_type & ~0x80) {
        case XCB_CLIENT_MESSAGE:
            auto clientMessage = (xcb_client_message_event_t*)event;
            if (clientMessage->data.data32[0] == deleteWindowAtom) {
                // TODO
            }
            break;
    }
}

void X11Window::show() {
    xcb_map_window(con, id);
    xcb_flush(con);
}

void X11Window::hide() {
    xcb_unmap_window(con, id);
    xcb_flush(con);
}

void X11Window::minimize() {}
void X11Window::setMaximized(bool value) {}
void X11Window::setFullscreen(bool value) {}

void X11Window::setTitle(const std::string& title) {
    xcb_change_property(con,
                        XCB_PROP_MODE_REPLACE,
                        id,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        title.size(),
                        title.data());
    xcb_flush(con);
}

void X11Window::resize(u32 width, u32 height) {
    u32 values[2] = {width, height};
    xcb_configure_window(con, id, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    xcb_flush(con);
}

u32 X11Window::getWidth() { return 0; }
u32 X11Window::getHeight() { return 0; }

xcb_atom_t X11Window::getInternAtom(const std::string& name) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(con, 0, name.size(), name.data());
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(con, cookie, nullptr);
    xcb_atom_t atom = reply->atom;
    delete reply;
    return atom;
}