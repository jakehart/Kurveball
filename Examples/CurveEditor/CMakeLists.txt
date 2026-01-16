cmake_minimum_required(VERSION 3.28)
project(CurveLib)

# Glob all files in this directory
file(GLOB CURVE_EDITOR_SOURCES "*.cpp" "*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/*.cpp")
file(GLOB CURVE_EDITOR_HEADERS "*.h")

add_executable(CurveEditor ${CURVE_EDITOR_SOURCES})

target_include_directories(CurveEditor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../Source" "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")

# Link CurveLib for velocity curve calculations and OpenGL for graphics
find_package(OpenGL REQUIRED)
target_link_libraries(CurveEditor PRIVATE 
    CurveLib
	OpenGL::GL
)

target_link_directories(CurveEditor PRIVATE "../../Lib")

target_sources(CurveEditor PRIVATE
    FILE_SET HEADERS FILES ${CURVE_EDITOR_HEADERS}
)

set_property(TARGET CurveEditor PROPERTY CXX_STANDARD 20)
set_property(TARGET CurveEditor PROPERTY CXX_STANDARD_REQUIRED ON)