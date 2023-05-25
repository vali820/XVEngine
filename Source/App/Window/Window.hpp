#pragma once

#include "../Event/Event.hpp"
#include "Core/Core.hpp"

class Window {
   private:
    std::function<void()> _exitF                                               = nullptr;
    std::function<void(const ResizeEvent&)> _resizeF                           = nullptr;
    std::function<void(const MouseMoveEvent&)> _mouseMoveF                     = nullptr;
    std::function<void(const MouseButtonPressedEvent&)> _mouseButtonPressedF   = nullptr;
    std::function<void(const MouseButtonReleasedEvent&)> _mouseButtonReleasedF = nullptr;
    std::function<void(const KeyPressedEvent&)> _keyPressedF                   = nullptr;
    std::function<void(const KeyReleasedEvent&)> _keyReleasedF                 = nullptr;

   protected:
    Window() = default;
    void exitF();
    void resizeF(const ResizeEvent& e);
    void mouseMoveF(const MouseMoveEvent& e);
    void mouseButtonPressedF(const MouseButtonPressedEvent& e);
    void mouseButtonReleasedF(const MouseButtonReleasedEvent& e);
    void keyPressedF(const KeyPressedEvent& e);
    void keyReleasedF(const KeyReleasedEvent& e);

   public:
    virtual ~Window() = default;

    void setExitCallback(const std::function<void()>& f);
    void setResizeCallback(const std::function<void(const ResizeEvent&)>& f);
    void setMouseMoveCallback(const std::function<void(const MouseMoveEvent&)>& f);
    void setMouseButtonPressedCallback(const std::function<void(const MouseButtonPressedEvent&)>& f);
    void setMouseButtonReleasedCallback(const std::function<void(const MouseButtonReleasedEvent&)>& f);
    void setKeyPressedCallback(const std::function<void(const KeyPressedEvent&)>& f);
    void setKeyReleasedCallback(const std::function<void(const KeyReleasedEvent&)>& f);

    virtual void show() = 0;
    virtual void hide() = 0;

    virtual void minimize()                         = 0;
    virtual void setMaximized(bool value)           = 0;
    virtual void setFullscreen(bool value)          = 0;
    virtual void resize(u32 width, u32 height)      = 0;
    virtual void setTitle(const String& title) = 0;

    virtual u32 getWidth()  = 0;
    virtual u32 getHeight() = 0;
};