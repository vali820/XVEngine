#include "Window.hpp"

void Window::exitF() {
    if (_exitF) _exitF();
}
void Window::resizeF(const ResizeEvent& e) {
    if (_resizeF) _resizeF(e);
}
void Window::mouseMoveF(const MouseMoveEvent& e) {
    mouse.setPos(e.x, e.y);
    if (_mouseMoveF) _mouseMoveF(e);
}
void Window::mouseButtonPressedF(const MouseButtonPressedEvent& e) {
    mouse.setButton(e.button, true);
    if (_mouseButtonPressedF) _mouseButtonPressedF(e);
}
void Window::mouseButtonReleasedF(const MouseButtonReleasedEvent& e) {
    mouse.setButton(e.button, false);
    if (_mouseButtonReleasedF) _mouseButtonReleasedF(e);
}
void Window::keyPressedF(const KeyPressedEvent& e) {
    keyboard.setKey(e.key, true);
    keyboard.setModifiers(e.modifiers);
    if (_keyPressedF) _keyPressedF(e);
}
void Window::keyReleasedF(const KeyReleasedEvent& e) {
    keyboard.setKey(e.key, false);
    keyboard.setModifiers(e.modifiers);
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