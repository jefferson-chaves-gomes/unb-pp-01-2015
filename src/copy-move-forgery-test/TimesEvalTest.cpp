#include "Tests.h"
#include "ForgingDetectorTest.h"

class TimesEvalTest : public ::testing::Test, public ForgingDetector
{
}

TEST_F(TimesEvalTest, timesEval)
{

    std::cout << std::endl << std::endl;
    std::cout << "n/n/n/############################## SERIAL TIMES ################################# n/" << std::endl;
    // charactVector_timeEval
    Timer charactVector_timeEval;
    ForgingDetector::charactVector(BITMAP, BLOCK_SIZE);
    std::cout << " 1 - charactVector_timeEval: " << charactVector_timeEval.elapsedMicroseconds() << std::endl;
    // createSimilarBlockList_timeEval
    CharVectList* lstCharacterVector = ForgingDetectorTest::getCopyOfCharacVec();
    VecSimilarBlocks vecSimilarBlocks;
    Timer createSimilarBlockList_timeEval;
    ForgingDetector::createSimilarBlockList(BITMAP, BLOCK_SIZE, lstCharacterVector, vecSimilarBlocks);
    std::cout << "2 - createSimilarBlockList_timeEval: " << createSimilarBlockList_timeEval.elapsedMicroseconds();
    //
    std::cout << "############################## SERIAL TIMES ################################# \n\n\n";
}
