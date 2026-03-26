# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(Kurveball)

file(GLOB SOURCE_FILES "Source/*.cpp")
file(GLOB HEADER_FILES "Source/*.h" "Source/*.inl")

# Combine all source files
set(ALL_SOURCES ${SOURCE_FILES} ${DEPENDENCY_SOURCES})

# Create a static library target using all source files
add_library(Kurveball STATIC ${ALL_SOURCES})

target_include_directories(Kurveball PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
	${CMAKE_CURRENT_SOURCE_DIR}/Dependencies
)

# Set C++ standard
set_property(TARGET Kurveball PROPERTY CXX_STANDARD 20)
set_property(TARGET Kurveball PROPERTY CXX_STANDARD_REQUIRED ON)

# Combine all header files for Solution Explorer visibility
set(ALL_HEADERS ${HEADER_FILES} ${DEPENDENCY_HEADERS})

# Add all header files to the target
target_sources(Kurveball PRIVATE
    FILE_SET HEADERS FILES ${ALL_HEADERS}
)


# Unit tests
add_subdirectory(Tests)
add_subdirectory(Examples/CurveEditor)

# Generate the .lib files in the Lib directory
set_target_properties(Kurveball PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Lib)
set_target_properties(Kurveball PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Lib)