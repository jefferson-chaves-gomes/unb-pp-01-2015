#include <gtest/gtest.h>
#include <ForgingDetector.h>

#include <iostream>
#include <string>
#include "Timer.h"

class ForgingDetectorTest : public ::testing::Test, public ForgingDetector
{
};

TEST_F(ForgingDetectorTest, charac_vec)
{
    Bitmap bmp(std::string("../copy-move-forgery/resource/publico.bmp"));

    const int BLOCK_SIZE = 16;

    Timer timeOld;
    CharVectList* vListOld = ForgingDetectorTest::charactVector(bmp, BLOCK_SIZE);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    CharVectList* vListNew = ForgingDetectorTest::charactVectorNew(bmp, BLOCK_SIZE);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    ASSERT_TRUE(vListOld != NULL);
    ASSERT_TRUE(vListNew != NULL);

    while(vListOld != NULL || vListNew != NULL)
    {
        ASSERT_EQ(vListOld->vect.x, vListNew->vect.x);
        ASSERT_EQ(vListOld->vect.y, vListNew->vect.y);

        for(int i = 0; i < CHARS_SIZE; i++)
            ASSERT_EQ(vListOld->vect.c[i], vListNew->vect.c[i]);

        vListOld = vListOld->next;
        vListNew = vListNew->next;
    }
}
