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

TEST_F(ForgingDetectorMPITest, charactVector)
{
    int BLOCK_SIZE = 47;

    Timer timeNew;
    ListCharVectPtr vListNew; ForgingDetectorMPI::charactVector(vListNew, BITMAP, BITMAP.getWidth(), BITMAP.getHeight(), BLOCK_SIZE);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    MPI_Barrier(MPI_COMM_WORLD);

    if(MPISettings::IS_PROC_ID_MASTER())
    {
        Timer timeOld;
        ListCharVect vListOld; ForgingDetector::charactVector(vListOld, BITMAP, BLOCK_SIZE);
        long double elapsedOld= timeOld.elapsedMicroseconds();

        std::cout << "Old: " << elapsedOld << std::endl;
        std::cout << "New: " << elapsedNew << std::endl;
        std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

//        TestUtils::printCharVectList(vListOld);
//
//        std::cout << "#############################" << std::endl;
//
//        TestUtils::printCharVectListPtr(vListNew);

        ASSERT_EQ(vListOld.size(), vListNew.size());

        assertEqualsCharVectList(vListOld, vListNew);
    }

}
