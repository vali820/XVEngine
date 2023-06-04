#include "X11Window.hpp"

#include <xcb/xcb.h>

#include "../Input/Event.hpp"
#include "X11Connection.hpp"

X11Window::X11Window(X11Connection* _connection, u32 _width, u32 _height)
    : connection(_connection),
      xcbCon(_connection->connection),
      screen(_connection->screen),
      width(_width),
      height(_height) {
    connection->windows.push(this);

    id = xcb_generate_id(xcbCon);

    u32 mask          = XCB_CW_EVENT_MASK;
    u32 value_list[1] = {XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
                         XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

    xcb_create_window(xcbCon,
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

    xcb_change_property(xcbCon,
                        XCB_PROP_MODE_REPLACE,
                        id,
                        connection->protocolsAtom,
                        XCB_ATOM_ATOM,
                        32,
                        1,
                        &connection->deleteWindowAtom);

    xcb_map_window(xcbCon, id);
    xcb_flush(xcbCon);
}

X11Window::~X11Window() {
    for (u64 i = 0; i < connection->windows.getSize(); i++) {
        if (connection->windows[i] == this) {
            connection->windows.remove(i);
        }
    }
    xcb_destroy_window(xcbCon, id);
}

xcb_connection_t* X11Window::getXcbConnection() { return xcbCon; }

void X11Window::update() {
    xcb_generic_event_t* event;
    while (true) {
        event = xcb_poll_for_event(xcbCon);
        if (!event) break;
        handleEvent(event);
    }
}

void X11Window::handleEvent(xcb_generic_event_t* event) {
    switch (event->response_type & ~0x80) {
        case XCB_CLIENT_MESSAGE: {
            auto clientMessage = (xcb_client_message_event_t*)event;
            if (clientMessage->data.data32[0] == connection->deleteWindowAtom) {
                exitF();
            }
            break;
        }
        case XCB_CONFIGURE_NOTIFY: {
            auto configure = (xcb_configure_notify_event_t*)event;
            if (configure->width != width or configure->height != height) {
                width  = configure->width;
                height = configure->height;
                resizeF(ResizeEvent(width, height));
            }
            break;
        }
    }
}

void X11Window::show() {
    xcb_map_window(xcbCon, id);
    xcb_flush(xcbCon);
}

void X11Window::hide() {
    xcb_unmap_window(xcbCon, id);
    xcb_flush(xcbCon);
}

void X11Window::minimize() {}
void X11Window::setMaximized(bool value) {}
void X11Window::setFullscreen(bool value) {}

void X11Window::setTitle(const String& title) {
    xcb_change_property(xcbCon,
                        XCB_PROP_MODE_REPLACE,
                        id,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        8,
                        title.getSize(),
                        title.getData());
    xcb_flush(xcbCon);
}

void X11Window::resize(u32 _width, u32 _height) {
    width = _width, height = _height;
    u32 values[2] = {width, height};
    xcb_configure_window(xcbCon, id, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    xcb_flush(xcbCon);
}

u32 X11Window::getWidth() { return 0; }
u32 X11Window::getHeight() { return 0; }