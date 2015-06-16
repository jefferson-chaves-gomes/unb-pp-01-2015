
#include <gtest/gtest.h>
#include <ForgingDetector.h>

#include <iostream>
#include <string>

class ForgingDetectorTest :
        public ::testing::Test,
        public ForgingDetector
{

protected:
    static CharVectList* charactVector(Bitmap& image, int bSize)
    {
        return ForgingDetector::charactVector(image, bSize);
    }

};

TEST_F(ForgingDetectorTest, charac_vec)
{
    Bitmap bmp(std::string("./resource/publico.bmp"));
    CharVectList* vList = ForgingDetectorTest::charactVector(bmp, 16);
}
