# Glob all files in this directory
file(GLOB CURVE_EDITOR_SOURCES "*.cpp" "*.c" "backends/*.cpp" "glad/glad_gl.c")
file(GLOB CURVE_EDITOR_HEADERS "*.hpp" "*.h" "backends/*.h" "glad/*.h")

add_executable(CurveEditor ${CURVE_EDITOR_SOURCES})

# Link to CurveLib from the parent
target_link_libraries(CurveEditor PRIVATE 
    CurveLib 
)

target_sources(CurveEditor PRIVATE
    FILE_SET HEADERS FILES ${CURVE_EDITOR_HEADERS}
)

add_compile_definitions(_GLFW_WIN32)

set_property(TARGET CurveEditor PROPERTY CXX_STANDARD 20)
set_property(TARGET CurveEditor PROPERTY CXX_STANDARD_REQUIRED ON)