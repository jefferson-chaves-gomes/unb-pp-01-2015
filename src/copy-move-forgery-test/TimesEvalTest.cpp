#include "UtilsTest.h"

class TimesEvalTest : public ::testing::Test, public ForgingDetector
{
};

TEST_F(TimesEvalTest, timesEval)
{
    std::cout << "\n\n\n############################## SERIAL TIMES ################################# \n" << std::endl;

    // charactVector_timeEval
    Timer charactVector_timeEval;
    CharVectList* lstCharacterVector = ForgingDetector::charactVector(BITMAP, BLOCK_SIZE);
    std::cout << "1 - charactVector_timeEval:          " << charactVector_timeEval.elapsedMicroseconds() << std::endl;

    // createSimilarBlockList_timeEval
    ListSimilarBlocks vecSimilarBlocks;
    Timer createSimilarBlockList_timeEval;
    ForgingDetector::createSimilarBlockList(BITMAP, BLOCK_SIZE, lstCharacterVector, vecSimilarBlocks);
    std::cout << "2 - createSimilarBlockList_timeEval: " << createSimilarBlockList_timeEval.elapsedMicroseconds() << std::endl;

    // filterSpuriousRegions_timesEval
    Timer filterSpuriousRegions_timesEval;
    filterSpuriousRegions(vecSimilarBlocks);
    std::cout << "3 - filterSpuriousRegions_timesEval: " << filterSpuriousRegions_timesEval.elapsedMicroseconds() << std::endl;

    std::cout << "\n############################## SERIAL TIMES ################################# \n\n\n";
}
