add_library(App SHARED Window/Window.cpp Event/Event.cpp)
target_link_libraries(App Core)

if (LINUX)
    target_sources(App PRIVATE
            Window/LinuxCommon.cpp Window/X11Window.cpp Window/WaylandWindow.cpp
            ../ThirdParty/wayland/xdg-shell.c ../ThirdParty/wayland/xdg-decoration.c
            )
    target_link_libraries(App xcb wayland-client xkbcommon)
endif ()