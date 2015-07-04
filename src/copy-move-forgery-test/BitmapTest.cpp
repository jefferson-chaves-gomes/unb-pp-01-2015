#include "TestUtils.h"

class BitmapTest : public ::testing::Test, public ForgingDetector
{
protected:

};

TEST_F(BitmapTest, getLines)
{
    int lineStart = 100;
    int lengthLines = 50;

    Bitmap blk = BITMAP.getLines(lineStart, lengthLines);

    unsigned char red, green, blue;
    unsigned char redBlk, greenBlk, blueBlk;

//    std::string path(ImgUtils::imgTrueName(BITMAP.getPath()));
//    path.append(std::string("_test.bmp"));
//    ImgUtils::saveImageAs(blk, path);

    for(int x=0; x<blk.getWidth(); x++)
    {
        for(int y=0; y<blk.getHeight(); y++)
        {
            BITMAP.getPixel(x, lineStart+y, red, green, blue);
            blk.getPixel(x, y, redBlk, greenBlk, blueBlk);

            ASSERT_EQ(red, redBlk);
            ASSERT_EQ(green, greenBlk);
            ASSERT_EQ(blue, blueBlk);
        }
    }
}

TEST_F(BitmapTest, savingImage)
{
    unsigned char red, green, blue, redBlk, greenBlk, blueBlk;

    std::string path(ImgUtils::imgTrueName(BITMAP_NORMAL.getPath()));
    path.append(std::string("_temp.bmp"));
    ImgUtils::saveImageAs(BITMAP_NORMAL, path);

    Bitmap temp(path);

    for(int x=0; x<BITMAP_NORMAL.getWidth(); x++)
    {
        for(int y=0; y<BITMAP_NORMAL.getHeight(); y++)
        {
            BITMAP_NORMAL.getPixel(x, y, red, green, blue);
            temp.getPixel(x, y, redBlk, greenBlk, blueBlk);

            ASSERT_EQ(red, redBlk);
            ASSERT_EQ(green, greenBlk);
            ASSERT_EQ(blue, blueBlk);
        }
    }
}
