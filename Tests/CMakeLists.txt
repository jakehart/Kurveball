# Glob all test files in this directory
file(GLOB TEST_SOURCES "*.cpp")

# Glob files in the dependencies directory
file(GLOB DEPENDENCY_SOURCES "../Dependencies/*.cpp" "../Dependencies/*.c")
file(GLOB DEPENDENCY_HEADERS "../Dependencies/*.hpp" "../Dependencies/*.h")

# Create the executable that will run the tests
add_executable(KurveballTests ${TEST_SOURCES} ${DEPENDENCY_SOURCES})

# The test executable links to the Kurveball target from the parent
target_link_libraries(KurveballTests PRIVATE 
    Kurveball 
)

target_include_directories(KurveballTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Source
)

# Register the executable with CTest
add_test(NAME run_all_tests COMMAND KurveballTests)

set_property(TARGET KurveballTests PROPERTY CXX_STANDARD 20)
set_property(TARGET KurveballTests PROPERTY CXX_STANDARD_REQUIRED ON)