#include <gtest/gtest.h>
#include <mpi/mpi.h>
#include <MPISettings.h>

int main(int argc, char **argv) {
//  ::testing::GTEST_FLAG(filter) = "TimesEvalTest.timesEval"; // --gtest_filter=TimesEvalTest.timesEval
//  ::testing::GTEST_FLAG(filter) = "ForgingDetectorTest.*";
//    ::testing::GTEST_FLAG(filter) = "BitmapTest.savingImage";

    MPI_Init(&argc, &argv);


    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    MPI_Finalize();

    return result;
}
