#include <catch_amalgamated.hpp>
#include <fstream>
#include <string>
#include "Vector2.h"


TEST_CASE("De/serialize Vector2")
{
    using namespace Kurveball;
    const std::string TEST_FILE_NAME = "Vector2TestFile.bin";
    const Float2 originalVector {123.456f, -999.999f};
    
    std::ofstream outStream(TEST_FILE_NAME);
    originalVector.ToBinary(outStream);
    outStream.close();
    
    std::ifstream inStream(TEST_FILE_NAME);
    const auto serializedVector = Float2::FromBinary(inStream);
    inStream.close();

    REQUIRE_THAT(serializedVector.X, Catch::Matchers::WithinAbs(originalVector.X, sFloatEpsilon));
    REQUIRE_THAT(serializedVector.Y, Catch::Matchers::WithinAbs(originalVector.Y, sFloatEpsilon));    
}