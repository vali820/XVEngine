set(SHADER_OUT_DIR ${CMAKE_SOURCE_DIR}/Shaders/)
set(SHADER_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/Shaders/)

add_custom_target(Shaders)

set(SHADER_FILES UIRoundedRect.frag UIVertex.vert)

foreach (shader IN ITEMS ${SHADER_FILES})
    set(SRC_PATH "${SHADER_SRC_DIR}${shader}")
    set(SPV_PATH "${SHADER_OUT_DIR}${shader}.spv")
    add_custom_command(
            OUTPUT ${SPV_PATH}
            COMMAND glslc ${SRC_PATH} -o ${SPV_PATH}
            MAIN_DEPENDENCY ${SRC_PATH}
    )
    target_sources(Shaders PRIVATE ${SPV_PATH})
endforeach ()