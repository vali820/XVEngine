add_library(App
        App.cpp AppWindow.cpp
        Window/Window.cpp Window/WindowConnection.cpp Event/Event.cpp
        Draw/DrawContext.cpp
        Widget/Widget.cpp
        )
target_link_libraries(App Core GpuApi)

if (LINUX)
    target_sources(App PRIVATE
            Window/WlConnection.cpp Window/WlWindow.cpp
            Window/X11Connection.cpp Window/X11Window.cpp
            Window/LinuxCommon.cpp ../ThirdParty/wayland/xdg-shell.c ../ThirdParty/wayland/xdg-decoration.c
            )
    target_link_libraries(App xcb wayland-client xkbcommon)
endif ()