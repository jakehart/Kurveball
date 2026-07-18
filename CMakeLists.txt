# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.6)

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

# Add post-build step to copy the source files to the Unreal plugin directory
add_custom_command(
    TARGET Kurveball POST_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Scripts
    COMMAND cmd /c "${CMAKE_SOURCE_DIR}/Scripts/SymlinkToUnrealPlugin.bat"
    COMMENT "Running SymlinkToUnrealPlugin.bat..."
)

# Combine all header files for Solution Explorer visibility
set(ALL_HEADERS ${HEADER_FILES} ${DEPENDENCY_HEADERS})

# Add all header files to the target
target_sources(Kurveball PRIVATE
    FILE_SET HEADERS FILES ${ALL_HEADERS}
)


# Unit tests
add_subdirectory(Tests)
# Note: the Unreal plugin doesn't appear here because it's built with Unreal Build Tool. See KurveballPlugin.uplugin
add_subdirectory(Source/GodotWrapper)
add_subdirectory(Examples/CurveEditor)

# Generate the .lib files in the Lib directory
set_target_properties(Kurveball PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Lib)
set_target_properties(Kurveball PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Lib)

#set_target_properties(Kurveball PROPERTIES VS_STARTUP_PROJECT KurveballTests)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT KurveballTests)