add_library(UI DrawData.cpp Widget/Widget.cpp Widget/Button.cpp)

target_link_libraries(UI Core GpuApi)