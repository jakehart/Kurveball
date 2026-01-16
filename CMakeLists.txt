# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(CurveLib)

file(GLOB SOURCE_FILES "Source/*.cpp")
file(GLOB HEADER_FILES "Source/*.h" "Source/*.inl")

# Glob files in the dependencies directory
file(GLOB DEPENDENCY_SOURCES "dependencies/*.cpp" "dependencies/*.c")
file(GLOB DEPENDENCY_HEADERS "dependencies/*.hpp" "dependencies/*.h")

# Combine all source files
set(ALL_SOURCES ${SOURCE_FILES} ${DEPENDENCY_SOURCES})

# Create a static library target using all source files
add_library(CurveLib STATIC ${ALL_SOURCES})

target_include_directories(CurveLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies
)

# Set C++ standard
set_property(TARGET CurveLib PROPERTY CXX_STANDARD 20)
set_property(TARGET CurveLib PROPERTY CXX_STANDARD_REQUIRED ON)

# Combine all header files for Solution Explorer visibility
set(ALL_HEADERS ${HEADER_FILES} ${DEPENDENCY_HEADERS})

# Add all header files to the target
target_sources(CurveLib PRIVATE
    FILE_SET HEADERS FILES ${ALL_HEADERS}
)


# Unit tests
add_subdirectory(Tests)
add_subdirectory(examples/CurveEditor)