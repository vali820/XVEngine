#include "WindowConnection.hpp"

#include "WlConnection.hpp"
#include "X11Connection.hpp"

#ifdef __linux__
const bool useWayland = std::getenv("XDG_SESSION_TYPE") == std::string("wayland");
#endif

WindowConnection* WindowConnection::create() {
#ifdef __linux__
    if (useWayland)
        return new WlConnection;
    else
        return new X11Connection;
#endif
}