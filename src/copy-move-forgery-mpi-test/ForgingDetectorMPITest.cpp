#include <time.h>       /* time */
#include <commons.h>
#include <ForgingDetectorMPI.h>
#include <mpi/mpi.h>
#include <MPISettings.h>
#include <TestUtils.h>

class ForgingDetectorMPITest : public ::testing::Test, public ForgingDetectorMPI
{
protected:
    void assertLexicalOrderCharVectList(ListCharVectPtr const& vecOrdered)
    {
        CharVect * prev;
        for(ListCharVectPtr::const_iterator it = vecOrdered.begin(); it != vecOrdered.end(); prev = *(it++))
        {
            if(it == vecOrdered.begin())
                continue;
            ASSERT_TRUE(CharVect::lessOrEqualsToPtr(prev, *(it)));
        }
    }

    void assertEqualsSimilarBlocks(ListSimilarBlocks& vectLeft, ListSimilarBlocks& vectRight)
    {
        ASSERT_TRUE(vectLeft.size() != 0);
        ASSERT_TRUE(vectRight.size() != 0);
//        int count(0);

        for(ListSimilarBlocks::iterator left = vectLeft.begin(), right = vectRight.begin();
                left != vectLeft.end() || right != vectRight.end();
                left++, right++)
        {
            ASSERT_TRUE(left != vectLeft.end());
            ASSERT_TRUE(right != vectRight.end());

            ASSERT_EQ(left->b1, right->b1);
            ASSERT_EQ(left->b2, right->b2);
            ASSERT_EQ(left->delta.dx, right->delta.dx);
            ASSERT_EQ(left->delta.dy, right->delta.dy);
        }
    }

    void assertEqualsCharVectList(ListCharVect const& vectLeft, ListCharVectPtr const& vectRight)
    {
//        int count(0);

        ListCharVect::const_iterator leftValue;
        ListCharVectPtr::const_iterator rightValue;
        for(leftValue = vectLeft.begin(), rightValue = vectRight.begin();
                leftValue != vectLeft.end() || rightValue != vectRight.end();
                leftValue++, rightValue++)
        {
            ASSERT_TRUE(leftValue != vectLeft.end());
            ASSERT_TRUE(rightValue != vectRight.end());

            ASSERT_EQ((leftValue)->pos, (*rightValue)->pos);
            for(int i = 0; i < CharVect::CHARS_SIZE; i++)
                ASSERT_EQ((leftValue)->c[i], (*rightValue)->c[i]);
        }
    }
};

//TEST_F(ForgingDetectorMPITest, charactVector)
//{
//    int BLOCK_SIZE = 16;
//
//    Timer timeNew;
//    ListCharVectPtr vListNew; ForgingDetectorMPI::charactVector(vListNew, BITMAP_NORMAL, BITMAP_NORMAL.getWidth(), BITMAP_NORMAL.getHeight(), BLOCK_SIZE);
//    long double elapsedNew = timeNew.elapsedMicroseconds();
//
//    MPI_Barrier(MPI_COMM_WORLD);
//
//    if(MPISettings::IS_PROC_ID_MASTER())
//    {
//        Timer timeOld;
//        ListCharVect vListOld; ForgingDetector::charactVector(vListOld, BITMAP_NORMAL, BLOCK_SIZE);
//        long double elapsedOld= timeOld.elapsedMicroseconds();
//
//        std::cout << "Old: " << elapsedOld << std::endl;
//        std::cout << "New: " << elapsedNew << std::endl;
//        std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;
//
//        ASSERT_EQ(vListOld.size(), vListNew.size());
//
//        assertEqualsCharVectList(vListOld, vListNew);
//    }
//}

TEST_F(ForgingDetectorMPITest, createSimilarBlockList)
{
    int BLOCK_SIZE = 16;
    ListCharVectPtr vListNew; ForgingDetectorMPI::charactVector(vListNew, BITMAP_NORMAL, BITMAP_NORMAL.getWidth(), BITMAP_NORMAL.getHeight(), BLOCK_SIZE);

    Timer timeNew;
    ListSimilarBlocks simBlkNew;
    createSimilarBlockList(BITMAP_NORMAL, BLOCK_SIZE, vListNew, simBlkNew, getVectOffsetSize(BITMAP_NORMAL.getWidth(), BITMAP_NORMAL.getHeight(), BLOCK_SIZE));
    long double elapsedNew = timeNew.elapsedMicroseconds();

    if(MPISettings::IS_PROC_ID_MASTER())
    {
        ListCharVect vListOld; ForgingDetector::charactVector(vListOld, BITMAP_NORMAL, BLOCK_SIZE);

        Timer timeOld;
        ListSimilarBlocks simBlkOld; ForgingDetector::createSimilarBlockList(BITMAP_NORMAL, BLOCK_SIZE, vListOld, simBlkOld);
        long double elapsedOld = timeOld.elapsedMicroseconds();

        std::cout << "Old: " << elapsedOld << std::endl;
        std::cout << "New: " << elapsedNew << std::endl;
        std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

        ASSERT_EQ(simBlkOld.size(), simBlkNew.size());

//        assertEqualsSimilarBlocks(simBlkOld, simBlkNew);
    }
}
