#include "UtilsTest.h"

class TimesEvalTest : public ::testing::Test, public ForgingDetector
{
};

TEST_F(TimesEvalTest, timesEval)
{
    std::cout << "\n\n############################## SERIAL TIMES ################################# \n" << std::endl;

    // charactVector_timeEval
    Timer timeEval01;
    ListCharVect lstCharacterVector;
    charactVector(lstCharacterVector, BITMAP, BLOCK_SIZE);
    std::cout << "1 - charactVector_timeEval:                 " << timeEval01.elapsedMicroseconds() << std::endl;

    // createSimilarBlockList_timeEval
    ListSimilarBlocks lstSimilarBlocks;
    Timer timeEval02;
    createSimilarBlockList(BITMAP, BLOCK_SIZE, lstCharacterVector, lstSimilarBlocks);
    std::cout << "2 - createSimilarBlockList_timeEval:        " << timeEval02.elapsedMicroseconds() << std::endl;

    // getMainShiftVector_timeEval
    Timer timeEval03;
    DeltaPos mainShift = getMainShiftVector(lstSimilarBlocks);
    std::cout << "3 - getMainShiftVector_timeEval:            " << timeEval03.elapsedMicroseconds() << std::endl;

    // filterSpuriousRegions_timesEval
    Timer timeEval04;
    filterSpuriousRegions(lstSimilarBlocks, mainShift);
    std::cout << "4 - filterSpuriousRegions_timesEval:        " << timeEval04.elapsedMicroseconds() << std::endl;

    // createImageWithSimilarAreas_timesEval
    Timer timeEval05;
    Bitmap detectImage(BITMAP.getWidth(), BITMAP.getHeight());
    createImageWithSimilarAreas(detectImage, BITMAP, BLOCK_SIZE, lstSimilarBlocks);
    std::cout << "5 - createImageWithSimilarAreas_timesEval:  " << timeEval05.elapsedMicroseconds() << std::endl;

    Timer timeEval06;
    detectImage = imageOpeningOperation(detectImage, BLOCK_SIZE);
    std::cout << "6 - imageOpeningOperation_timesEval:        " << timeEval06.elapsedMicroseconds() << std::endl;

    Timer timeEval07;
    Bitmap forgedImage(BITMAP.getWidth(), BITMAP.getHeight());
    isImageForged(BITMAP, detectImage, forgedImage);
    std::cout << "7 - isImageForged_timeEval:                 " << timeEval07.elapsedMicroseconds() << std::endl;

    Timer timeEval08;
    std::string path(ImgUtils::imgTrueName(BITMAP.getPath()));
    path.append(std::string("_detect.bmp"));
    ImgUtils::saveImageAs(forgedImage, path);
    std::cout << "8 - saveImageAs_timesEval:                  " << timeEval08.elapsedMicroseconds() << std::endl;

    std::cout << "\n############################## SERIAL TIMES ################################# \n\n";
}
