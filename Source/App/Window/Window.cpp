#include "Window.hpp"

#ifdef __linux__

#include "WaylandWindow.hpp"
#include "X11Window.hpp"

bool wayland = std::getenv("XDG_SESSION_TYPE") == std::string("wayland");

Ptr<Window> Window::create(u32 _width, u32 _height) {
    if (wayland)
        return Ptr<WaylandWindow>(new WaylandWindow(_width, _height));
    else
        return Ptr<X11Window>(new X11Window(_width, _height));
}

#endif

void Window::exitF() {
    if (_exitF) _exitF();
}
void Window::resizeF(const ResizeEvent& e) {
    if (_resizeF) _resizeF(e);
}
void Window::mouseMoveF(const MouseMoveEvent& e) {
    if (_mouseMoveF) _mouseMoveF(e);
}
void Window::mouseButtonPressedF(const MouseButtonPressedEvent& e) {
    if (_mouseButtonPressedF) _mouseButtonPressedF(e);
}
void Window::mouseButtonReleasedF(const MouseButtonReleasedEvent& e) {
    if (_mouseButtonReleasedF) _mouseButtonReleasedF(e);
}
void Window::keyPressedF(const KeyPressedEvent& e) {
    if (_keyPressedF) _keyPressedF(e);
}
void Window::keyReleasedF(const KeyReleasedEvent& e) {
    if (_keyReleasedF) _keyReleasedF(e);
}

void Window::setExitCallback(const std::function<void()>& f) { _exitF = f; }
void Window::setResizeCallback(const std::function<void(const ResizeEvent&)>& f) { _resizeF = f; }
void Window::setMouseMoveCallback(const std::function<void(const MouseMoveEvent&)>& f) { _mouseMoveF = f; }
void Window::setMouseButtonPressedCallback(const std::function<void(const MouseButtonPressedEvent&)>& f) {
    _mouseButtonPressedF = f;
}
void Window::setMouseButtonReleasedCallback(const std::function<void(const MouseButtonReleasedEvent&)>& f) {
    _mouseButtonReleasedF = f;
}
void Window::setKeyPressedCallback(const std::function<void(const KeyPressedEvent&)>& f) { _keyPressedF = f; }
void Window::setKeyReleasedCallback(const std::function<void(const KeyReleasedEvent&)>& f) {
    _keyReleasedF = f;
}