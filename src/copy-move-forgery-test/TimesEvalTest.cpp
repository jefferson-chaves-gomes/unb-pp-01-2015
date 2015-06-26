#include "UtilsTest.h"

class TimesEvalTest : public ::testing::Test, public ForgingDetector
{
};

TEST_F(TimesEvalTest, timesEval)
{
    std::cout << "\n\n############################## SERIAL TIMES ################################# \n" << std::endl;

    // charactVector_timeEval
    Timer charactVector_timeEval;
    ListCharVect lstCharacterVector;
    charactVector(lstCharacterVector, BITMAP, BLOCK_SIZE);
    std::cout << "1 - charactVector_timeEval:           " << charactVector_timeEval.elapsedMicroseconds() << std::endl;

    // createSimilarBlockList_timeEval
    ListSimilarBlocks lstSimilarBlocks;
    Timer createSimilarBlockList_timeEval;
    createSimilarBlockList(BITMAP, BLOCK_SIZE, lstCharacterVector, lstSimilarBlocks);
    std::cout << "2 - createSimilarBlockList_timeEval:  " << createSimilarBlockList_timeEval.elapsedMicroseconds()
            << std::endl;

    // getMainShiftVector_timeEval
    Timer getMainShiftVector_timeEval;
    DeltaPos mainShift = getMainShiftVector(lstSimilarBlocks);
    std::cout << "3 - getMainShiftVector_timeEval:      " << getMainShiftVector_timeEval.elapsedMicroseconds()
            << std::endl;

    // filterSpuriousRegions_timesEval
    Timer filterSpuriousRegions_timesEval;
    filterSpuriousRegions(lstSimilarBlocks, mainShift);
    std::cout << "4 - filterSpuriousRegions_timesEval:  " << filterSpuriousRegions_timesEval.elapsedMicroseconds()
            << std::endl;

    std::cout << "\n############################## SERIAL TIMES ################################# \n\n";
}
