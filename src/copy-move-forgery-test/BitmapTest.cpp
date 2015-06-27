#include "UtilsTest.h"

class BitmapTest : public ::testing::Test, public ForgingDetector
{
protected:

};

TEST_F(BitmapTest, getBlock)
{
    int blockSize(50);

    Pos pos(0, 0);
    Bitmap blk = BITMAP.getBlock(pos, blockSize);

    unsigned char red, green, blue;
    unsigned char redBlk, greenBlk, blueBlk;

//    std::string path(ImgUtils::imgTrueName(bitmap.getPath()));
//    path.append(std::string("_test.bmp"));
//    ImgUtils::saveImageAs(blk, path);

    for(int i=0; i<blockSize; i++)
    {
        std::cout << "Testing line: " << i << std::endl;
        for(int j=0; j<blockSize; j++)
        {
            BITMAP.getPixel(pos.x+i, pos.y+j, red, green, blue);
            blk.getPixel(i, j, redBlk, greenBlk, blueBlk);

            ASSERT_EQ(red, redBlk);
            ASSERT_EQ(green, greenBlk);
            ASSERT_EQ(blue, blueBlk);
        }
    }
}
