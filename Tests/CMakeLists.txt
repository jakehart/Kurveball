# Glob all test files in this directory
file(GLOB TEST_SOURCES "*.cpp")

# Create the executable that will run the tests
# The target name is still CurveLibTests
add_executable(CurveLibTests ${TEST_SOURCES})

# The test executable links to the CurveLib target from the parent
target_link_libraries(CurveLibTests PRIVATE 
    CurveLib 
)

# Register the executable with CTest
add_test(NAME run_all_tests COMMAND CurveLibTests)

set_property(TARGET CurveLibTests PROPERTY CXX_STANDARD 20)
set_property(TARGET CurveLibTests PROPERTY CXX_STANDARD_REQUIRED ON)