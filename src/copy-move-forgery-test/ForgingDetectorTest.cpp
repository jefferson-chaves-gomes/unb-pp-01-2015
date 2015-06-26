#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <ForgingDetectorOld.h>
#include <Timer.h>
#include <utils.h>

#include <iostream>
#include <string>

const int BLOCK_SIZE = 20;
const std::string IMG_PATH("../copy-move-forgery/resource/publico.bmp");
const Bitmap BITMAP(IMG_PATH);

class ForgingDetectorTest :
        public ::testing::Test,
        public ForgingDetector
{
protected:

    void assertEqualsSimilarBlocks(SimilarBlocksOld* left, ListSimilarBlocks& vect)
    {
        ASSERT_TRUE(left != NULL);
        ASSERT_TRUE(vect.size() != 0);
//        int count(0);

        for(ListSimilarBlocks::iterator right=vect.begin();
                left != NULL || right != vect.end();
                right++, left = left->next)
        {
            ASSERT_TRUE(left != NULL);
            ASSERT_TRUE(right != vect.end());

            ASSERT_EQ(left->b1, right->b1);
            ASSERT_EQ(left->b2, right->b2);
            ASSERT_EQ(left->dx, right->delta.dx);
            ASSERT_EQ(left->dy, right->delta.dy);
        }
    }

    void printCharVectList(ListCharVect& charVectList)
    {
        for(ListCharVect::iterator it = charVectList.begin(); it != charVectList.end(); it++)
        {
            std::cout << "######################" << std::endl;
            printCharVect(*it);
        }
        std::cout << "######################" << std::endl;
    }

    void printCharVectListOld(CharVectListOld* charVectList)
    {
        CharVectListOld* aux = charVectList;

        while(aux != NULL)
        {
            std::cout << "######################" << std::endl;
            printCharVect(aux->vect);

            aux = aux->next;
        }
        std::cout << "######################" << std::endl;
    }

    void printDeltaPos(DeltaPos const& delta)
    {
        std::cout << "Delta: " << delta.dx << "|" << delta.dy << std::endl;
    }

    void printSimilarBlocks(ListSimilarBlocks const& auxBlock, int limit = 0)
    {
        int count(0);
        for(ListSimilarBlocks::const_iterator it=auxBlock.begin();
                it != auxBlock.end();
                it++)
        {
            std::cout << "Block " << count++ << ": ";
            printDeltaPos(it->delta);
            if(limit!=0 && limit <= count)
                break;
        }
    }

    void printSimilarBlocksOld(SimilarBlocksOld* auxBlock, int limit = 0)
    {
        int count(0);
        while(auxBlock != NULL)
        {
            std::cout << "Old Block " << count++ << ": ";
            std::cout << "Delta: " << auxBlock->dx << "," << auxBlock->dy;
            std::cout << " | B1: " << auxBlock->b1.x << "," << auxBlock->b1.y;
            std::cout << " | B2: " << auxBlock->b2.x << "," << auxBlock->b2.y << std::endl;
            auxBlock = auxBlock->next;
            if(limit!=0 && limit <= count)
                break;
        }
    }

    void printCharVect(CharVect const& charVec)
    {
        std::cout << "|";
        for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        {
            std::cout << (double) charVec.c[i] << "|";
        }
        std::cout << std::endl;
    }

    void assertLexicalOrderCharVectListOld(CharVectListOld* vecOrdered)
    {
        CharVectListOld* aux = vecOrdered;

        while(aux != NULL)
        {
            if(aux->next != NULL)
                ASSERT_TRUE(aux->vect <= aux->next->vect);
            aux = aux->next;
        }
    }

    void assertLexicalOrderCharVectList(ListCharVect const& vecOrdered)
    {
        CharVect prev;
        for(ListCharVect::const_iterator it = vecOrdered.begin(); it != vecOrdered.end(); prev = *(it++))
        {
            if(it == vecOrdered.begin())
                continue;
            ASSERT_TRUE(prev <= *(it));
        }
    }

    void assertEqualsCharVectListOld(CharVectListOld* left, ListCharVect const& vect)
    {
        ASSERT_TRUE(left != NULL);
        ASSERT_TRUE(vect.size() != 0);
//        int count(0);

        for(ListCharVect::const_iterator right=vect.begin();
                left != NULL || right != vect.end();
                right++, left = left->next)
        {
            ASSERT_TRUE(left != NULL);
            ASSERT_TRUE(right != vect.end());

            ASSERT_EQ(left->vect.pos, right->pos);
            for(int i=0; i<CharVect::CHARS_SIZE; i++)
                ASSERT_EQ(left->vect.c[i], right->c[i]);
        }
    }


    typedef std::vector< std::pair<Pos, Pos> > VectPos;
    VectPos createFakeVectPos()
    {
        std::vector< std::pair<Pos, Pos> > vecPos;

        vecPos.push_back(std::make_pair(Pos(302,52), Pos(217,61)));
        vecPos.push_back(std::make_pair(Pos(216,62), Pos(302,53)));
        vecPos.push_back(std::make_pair(Pos(302,49), Pos(217,58)));
        vecPos.push_back(std::make_pair(Pos(301,53), Pos(218,61)));
        vecPos.push_back(std::make_pair(Pos(218,61), Pos(300,52)));
        vecPos.push_back(std::make_pair(Pos(302,50), Pos(218,60)));
        vecPos.push_back(std::make_pair(Pos(218,60), Pos(299,52)));
        vecPos.push_back(std::make_pair(Pos(298,52), Pos(218,59)));
        vecPos.push_back(std::make_pair(Pos(218,59), Pos(301,49)));
        vecPos.push_back(std::make_pair(Pos(304,51), Pos(28,136)));
        vecPos.push_back(std::make_pair(Pos(27,136), Pos(302,54)));
        vecPos.push_back(std::make_pair(Pos(302,54), Pos(28,135)));
        vecPos.push_back(std::make_pair(Pos(304,49), Pos(217,62)));
        vecPos.push_back(std::make_pair(Pos(217,62), Pos(27,134)));
        vecPos.push_back(std::make_pair(Pos(301,50), Pos(219,61)));
        vecPos.push_back(std::make_pair(Pos(28,135), Pos(304,51)));
        vecPos.push_back(std::make_pair(Pos(28,135), Pos(304,51)));
        vecPos.push_back(std::make_pair(Pos(28,135), Pos(304,51)));
        vecPos.push_back(std::make_pair(Pos(28,135), Pos(304,51)));

        return vecPos;
    }

    SimilarBlocksOld * createSimilarBlocksFakeOld(VectPos const& vectPos)
    {
        SimilarBlocksOld *head = NULL, *aux = NULL;

        for(VectPos::const_iterator it = vectPos.begin(); it != vectPos.end(); it++)
        {
            SimilarBlocksOld * simBlk = new SimilarBlocksOld(it->first, it->second);
            if(head == NULL)
                head = aux = simBlk;
            else
                aux = aux->next = simBlk;
        }

        return head;
    }

    ListSimilarBlocks createSimilarBlocksFake(VectPos const& vectPos)
    {
        ListSimilarBlocks headNew;

        for(VectPos::const_iterator it = vectPos.begin(); it != vectPos.end(); it++)
            headNew.push_back(SimilarBlocks(it->first, it->second));

        return headNew;
    }

    static CharVectListOld* getCopyOfCharacVec()
    {
        CharVectListOld *aux = NULL, *head = NULL;

        for(ListCharVect::const_iterator it = vList.begin(); it != vList.end(); it++)
        {
            if(head == NULL)
                aux = head = new CharVectListOld(*it);
            else
                aux = aux->next  = new CharVectListOld(*it);
        }

        return head;
    }

    static ListCharVect vList;
    // Primeiro lugar que ele entra no teste dessa classe
    static void SetUpTestCase()
    {
        std::cout << "Generating vector of chars..." << std::endl;
        ForgingDetector::charactVector(vList, BITMAP, BLOCK_SIZE);
        std::cout << "Vector of chars generated!!!" << std::endl;
    }
};

ListCharVect ForgingDetectorTest::vList;

TEST_F(ForgingDetectorTest, operator_less_or_equals_to)
{
    CharVect secnd;
    CharVect first;

    first.setChars(0,0,0,0,0,0,1);
    secnd.setChars(0,0,0,0,1,1,1);

    ASSERT_FALSE( secnd <= first);
    ASSERT_TRUE( first <= secnd);

    first.setChars(0,0,0,1,1,1,1);
    secnd.setChars(0,0,1,1,1,1,1);

    ASSERT_FALSE( secnd <= first);
    ASSERT_TRUE( first <= secnd);

    first.setChars(0,0,0,0,0,0,1);
    secnd.setChars(0,0,0,0,0,1,0);

    ASSERT_FALSE( secnd <= first);
    ASSERT_TRUE( first <= secnd);

    first.setChars(0,0,0,0,0,0,0);
    secnd.setChars(1,0,0,0,0,0,0);

    ASSERT_FALSE( secnd <= first);
    ASSERT_TRUE( first <= secnd);

    first.setChars(0,0,0,0,0,0,0);
    secnd.setChars(0,0,0,0,0,0,0);

    ASSERT_TRUE( secnd <= first);
    ASSERT_TRUE( first <= secnd);
}

TEST_F(ForgingDetectorTest, operatorEqualSimilarBlock)
{
    Timer timeOld;

    Pos posOld1(0,0);
    Pos posOld2(0,0);

    Pos posNew1(1,1);
    Pos posNew2(1,1);

    SimilarBlocks simBlkOld(posOld1, posOld2);
    SimilarBlocks simBlkNew(posNew1, posNew2);

    ASSERT_TRUE(simBlkOld != simBlkNew);

    simBlkOld.setValues(posNew1, posNew2);
    ASSERT_TRUE(simBlkOld == simBlkNew);
}

TEST_F(ForgingDetectorTest, addVectLexOrder)
{
    CharVectListOld * firstOld = new CharVectListOld;
    firstOld->vect.setChars(0,0,0,0,0,0,1);
    CharVectListOld * secndOld = new CharVectListOld;
    secndOld->vect.setChars(0,0,0,0,0,1,1);
    CharVectListOld * thirdOld = new CharVectListOld;
    thirdOld->vect.setChars(0,0,0,0,1,1,1);
    CharVectListOld * thirdRepOld = new CharVectListOld;
    thirdRepOld->vect.setChars(0,0,0,0,1,1,1);

    CharVect firstNew;
    firstNew.setChars(0,0,0,0,0,0,1);
    CharVect secndNew;
    secndNew.setChars(0,0,0,0,0,1,1);
    CharVect thirdNew;
    thirdNew.setChars(0,0,0,0,1,1,1);
    CharVect thirdRepNew;
    thirdRepNew.setChars(0,0,0,0,1,1,1);

    ListCharVect orderedNew;
    CharVectListOld * orderedOld = NULL;

    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, secndOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, firstOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, thirdOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, thirdRepOld);

    addVectLexOrder(orderedNew, secndNew);
    addVectLexOrder(orderedNew, firstNew);
    addVectLexOrder(orderedNew, thirdNew);
    addVectLexOrder(orderedNew, thirdRepNew);

    assertEqualsCharVectListOld(orderedOld, orderedNew);
    assertLexicalOrderCharVectList(orderedNew);
    assertLexicalOrderCharVectListOld(orderedOld);

    LinkedListCleaner::clear(orderedOld);
}

TEST_F(ForgingDetectorTest, charactVectorFast)
{
    Bitmap bmp(std::string("../copy-move-forgery/resource/icone.bmp"));

    CharVectListOld* vListOld = ForgingDetectorOld::charactVector(bmp, BLOCK_SIZE);

    ListCharVect vListNew; ForgingDetectorTest::charactVector(vListNew, bmp, BLOCK_SIZE);

    assertEqualsCharVectListOld(vListOld, vListNew);

    LinkedListCleaner::clear(vListOld);
}

// Too slow for testing....................
//TEST_F(ForgingDetectorTest, charactVector)
//{
//    Timer timeOld;
//    CharVectListOld* vListOld = ForgingDetectorOld::charactVector(BITMAP, BLOCK_SIZE);
//    long double elapsedOld = timeOld.elapsedMicroseconds();
//
//    Timer timeNew;
//    ListCharVect vListNew; ForgingDetectorTest::charactVector(vListNew, BITMAP, BLOCK_SIZE);
//    long double elapsedNew = timeNew.elapsedMicroseconds();
//
//    std::cout << "Old: " << elapsedOld << std::endl;
//    std::cout << "New: " << elapsedNew << std::endl;
//    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;
//
//    assertEqualsCharVectListOld(vListOld, vListNew);
//
//    LinkedListCleaner::clear(vListOld);
//}

TEST_F(ForgingDetectorTest, createSimilarBlockList)
{
    CharVectListOld* vListOld = getCopyOfCharacVec();

    Timer timeOld;
    SimilarBlocksOld* simBlkOld = ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vListOld);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    ListSimilarBlocks simBlkNew;
    createSimilarBlockList(BITMAP, BLOCK_SIZE, vList, simBlkNew);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    assertEqualsSimilarBlocks(simBlkOld, simBlkNew);

    LinkedListCleaner::clear(vListOld);
    LinkedListCleaner::clear(simBlkOld);
}

TEST_F(ForgingDetectorTest, getMainShiftVectorFast)
{
    ListSimilarBlocks headNew;
    headNew.push_back(SimilarBlocks(Pos(0,0), Pos(1,1)));
    headNew.push_back(SimilarBlocks(Pos(0,0), Pos(2,2)));
    headNew.push_back(SimilarBlocks(Pos(0,0), Pos(1,1)));
    headNew.push_back(SimilarBlocks(Pos(0,0), Pos(2,2)));
    DeltaPos deltaPos = getMainShiftVector(headNew);

    SimilarBlocksOld *headOld, *auxOld;
    auxOld = headOld      = new SimilarBlocksOld(Pos(0,0), Pos(1,1));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(2,2));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(1,1));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(2,2));
    SimilarBlocksOld *deltaPosOld = ForgingDetectorOld::getMainShiftVector(headOld);

    ASSERT_EQ(deltaPos.dx, deltaPosOld->dx);
    ASSERT_EQ(deltaPos.dy, deltaPosOld->dy);

    LinkedListCleaner::clear(headOld);
}

TEST_F(ForgingDetectorTest, getMainShiftVector)
{
    CharVectListOld* vListOld = getCopyOfCharacVec();
    SimilarBlocksOld* simBlkOld = ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vListOld);
    ListSimilarBlocks simBlkNew; createSimilarBlockList(BITMAP, BLOCK_SIZE, vList, simBlkNew);

    Timer timeOld;
    SimilarBlocksOld* deltaPosOld = ForgingDetectorOld::getMainShiftVector(simBlkOld);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    DeltaPos deltaPos = getMainShiftVector(simBlkNew);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    ASSERT_EQ(deltaPos.dx, deltaPosOld->dx);
    ASSERT_EQ(deltaPos.dy, deltaPosOld->dy);

    LinkedListCleaner::clear(vListOld);
    LinkedListCleaner::clear(simBlkOld);
}

TEST_F(ForgingDetectorTest, filterSpuriousRegionsFast)
{
    VectPos vectPos(createFakeVectPos());

    ListSimilarBlocks headNew(createSimilarBlocksFake(vectPos));
    SimilarBlocksOld *headOld(createSimilarBlocksFakeOld(vectPos));

    assertEqualsSimilarBlocks(headOld, headNew);

    DeltaPos deltaPos = getMainShiftVector(headNew);
    SimilarBlocksOld *deltaPosOld = ForgingDetectorOld::getMainShiftVector(headOld);

    ASSERT_EQ(deltaPos.dx, deltaPosOld->dx);
    ASSERT_EQ(deltaPos.dy, deltaPosOld->dy);

    ForgingDetectorOld::filterSpuriousRegions(&headOld, deltaPosOld);
    filterSpuriousRegions(headNew, deltaPos);

    assertEqualsSimilarBlocks(headOld, headNew);

    LinkedListCleaner::clear(headOld);
}

TEST_F(ForgingDetectorTest, filterSpuriousRegions)
{
    CharVectListOld* vListOld = getCopyOfCharacVec();

    SimilarBlocksOld* simBlkOld = ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vListOld);
    ListSimilarBlocks simBlkNew; createSimilarBlockList(BITMAP, BLOCK_SIZE, vList, simBlkNew);

    SimilarBlocksOld* deltaPosOld = ForgingDetectorOld::getMainShiftVector(simBlkOld);
    DeltaPos deltaPos = getMainShiftVector(simBlkNew);

    ASSERT_EQ(deltaPos.dx, deltaPosOld->dx);
    ASSERT_EQ(deltaPos.dy, deltaPosOld->dy);

    Timer timeOld;
    ForgingDetectorOld::filterSpuriousRegions(&simBlkOld, deltaPosOld);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    filterSpuriousRegions(simBlkNew, deltaPos);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    assertEqualsSimilarBlocks(simBlkOld, simBlkNew);

    LinkedListCleaner::clear(vListOld);
    LinkedListCleaner::clear(simBlkOld);
}
