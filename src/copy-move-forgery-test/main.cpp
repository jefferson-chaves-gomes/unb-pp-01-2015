#include <gtest/gtest.h>

int main(int argc, char **argv) {
//  ::testing::GTEST_FLAG(filter) = "TimesEvalTest.timesEval"; // --gtest_filter=TimesEvalTest.timesEval
//  ::testing::GTEST_FLAG(filter) = "ForgingDetectorTest.*";
    ::testing::GTEST_FLAG(filter) = "BitmapTest.savingImage";
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
