#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <ForgingDetectorOld.h>
#include <utils.h>

#include <iostream>
#include <string>
#include "Timer.h"


const int BLOCK_SIZE = 20;
const std::string IMG_PATH("../copy-move-forgery/resource/publico.bmp");
const Bitmap BITMAP(IMG_PATH);

class ForgingDetectorTest :
        public ::testing::Test,
        public ForgingDetector
{
protected:

    void assertEqualsSimilarBlocks(SimilarBlocksOld* left, SimilarBlocks* right)
    {
        ASSERT_TRUE(left != NULL);
        ASSERT_TRUE(right != NULL);

        while(left != NULL || right != NULL)
        {
            ASSERT_TRUE(left != NULL);
            ASSERT_TRUE(right != NULL);

            ASSERT_TRUE(left->b1 == right->b1);
            ASSERT_TRUE(left->b2 == right->b2);
            ASSERT_TRUE(left->dx == right->delta.dx);
            ASSERT_TRUE(left->dy == right->delta.dy);

            left = left->next;
            right = right->next;
        }
    }

    void printCharVectList(CharVectList* charVectList)
    {
        CharVectList* aux = charVectList;

        while(aux != NULL)
        {
            std::cout << "######################" << std::endl;
            printCharVect(&aux->vect);

            aux = aux->next;
        }
        std::cout << "######################" << std::endl;
    }


    void printDeltaPos(DeltaPos const& delta)
    {
        std::cout << "Delta: " << delta.dx << "|" << delta.dy << std::endl;
    }

    void printSimilarBlocks(SimilarBlocks* auxBlock)
    {
        int count(0);
        while(auxBlock != NULL)
        {
            std::cout << "Block " << count++ << ": ";
            printDeltaPos(auxBlock->delta);
            auxBlock = auxBlock->next;
        }
    }

    void printSimilarBlocksOld(SimilarBlocksOld* auxBlock)
    {
        int count(0);
        while(auxBlock != NULL)
        {
            std::cout << "Block " << count++ << ": ";
            std::cout << "Delta: " << auxBlock->dx << "|" << auxBlock->dy << std::endl;
            auxBlock = auxBlock->next;
        }
    }

    void printCharVect(CharVect* charVec)
    {
        if(charVec == NULL)
        {
            std::cout << "VEC IS NULL" << std::endl;
            return;
        }

        std::cout << "|";
        for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        {
            std::cout << (int) charVec->c[i] << "|";
        }
        std::cout << std::endl;
    }

    void assertLexicalOrderCharVectList(CharVectList* charVectList)
    {
        CharVectList* aux = charVectList;

        while(aux != NULL)
        {
            if(aux->next != NULL)
                ASSERT_TRUE(aux->vect <= aux->next->vect);
            aux = aux->next;
        }
    }

    void assertEqualsCharVectList(CharVectList* left, CharVectList* right)
    {
        ASSERT_TRUE(left != NULL);
        ASSERT_TRUE(right != NULL);

        while(left != NULL || right != NULL)
        {
            ASSERT_TRUE(left != NULL);
            ASSERT_TRUE(right != NULL);

            ASSERT_EQ(left->vect.pos.x, right->vect.pos.x);
            ASSERT_EQ(left->vect.pos.y, right->vect.pos.y);

            for(int i = 0; i < CharVect::CHARS_SIZE; i++)
                ASSERT_EQ(left->vect.c[i], right->vect.c[i]);

            left = left->next;
            right = right->next;
        }
    }

    static CharVectList* getCopyOfCharacVec()
    {
        if(vList == NULL)
            return NULL;

        CharVectList* aux = vList;
        CharVectList *auxCopy = new CharVectList(*vList);

        CharVectList *res = auxCopy;

        while(aux != NULL)
        {
            if(aux->next != NULL)
                auxCopy->next = new CharVectList(*aux->next);

            aux = aux->next;
            auxCopy = auxCopy->next;
        }

        return res;
    }

    static CharVectList* vList;
    // Primeiro lugar que ele entra no teste dessa classe
    static void SetUpTestCase()
    {
        std::cout << "Generating vector of chars..." << std::endl;
        vList = ForgingDetectorTest::charactVector(BITMAP, BLOCK_SIZE);
        std::cout << "Vector of chars generated!!!" << std::endl;
    }
};

CharVectList* ForgingDetectorTest::vList;

TEST_F(ForgingDetectorTest, addVectLexOrder)
{
    CharVectList * firstOld = new CharVectList;
    firstOld->vect.setChars(0,0,0,0,0,0,1);
    CharVectList * secndOld = new CharVectList;
    secndOld->vect.setChars(0,0,0,0,0,1,1);
    CharVectList * thirdOld = new CharVectList;
    thirdOld->vect.setChars(0,0,0,0,1,1,1);
    CharVectList * thirdRepOld = new CharVectList;
    thirdOld->vect.setChars(0,0,0,0,1,1,1);

    CharVectList * firstNew = new CharVectList;
    firstNew->vect.setChars(0,0,0,0,0,0,1);
    CharVectList * secndNew = new CharVectList;
    secndNew->vect.setChars(0,0,0,0,0,1,1);
    CharVectList * thirdNew = new CharVectList;
    thirdNew->vect.setChars(0,0,0,0,1,1,1);
    CharVectList * thirdRepNew = new CharVectList;
    thirdOld->vect.setChars(0,0,0,0,1,1,1);

    CharVectList * orderedNew = NULL;
    CharVectList * orderedOld = NULL;

    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, secndOld);
    ASSERT_EQ(orderedOld, secndOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, firstOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, thirdOld);
    orderedOld = ForgingDetectorOld::addVectLexOrder(orderedOld, thirdRepOld);

    orderedNew = addVectLexOrder(orderedNew, secndNew);
    ASSERT_EQ(orderedNew, secndNew);
    orderedNew = addVectLexOrder(orderedNew, firstNew);
    orderedNew = addVectLexOrder(orderedNew, thirdNew);
    orderedNew = addVectLexOrder(orderedNew, thirdRepNew);
    assertEqualsCharVectList(orderedOld, orderedNew);

    orderedNew = addVectLexOrder(orderedNew, secndOld);
    orderedNew = addVectLexOrder(orderedNew, thirdRepOld);
    orderedNew = addVectLexOrder(orderedNew, firstOld);
    orderedNew = addVectLexOrder(orderedNew, thirdOld);
    assertLexicalOrderCharVectList(orderedNew);

    thirdRepOld->next = NULL;
    orderedNew = NULL;
    orderedNew = addVectLexOrder(orderedNew, thirdRepOld);
    orderedNew = addVectLexOrder(orderedNew, thirdNew);
    orderedNew = addVectLexOrder(orderedNew, secndOld);
    orderedNew = addVectLexOrder(orderedNew, firstNew);
    assertLexicalOrderCharVectList(orderedNew);
}

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
    SimilarBlocks* simBlkOld = new SimilarBlocks;
    SimilarBlocks* simBlkNew = new SimilarBlocks;

    Pos posOld1(0,0);
    Pos posOld2(0,0);
    simBlkOld->setValues(posOld1, posOld2);
    Pos posNew1(1,1);
    Pos posNew2(1,1);
    simBlkNew->setValues(posNew1, posNew2);
    ASSERT_TRUE(*simBlkOld != *simBlkNew);

    simBlkOld->setValues(posNew1, posNew2);
    ASSERT_TRUE(*simBlkOld == *simBlkNew);
}

TEST_F(ForgingDetectorTest, charac_vec)
{
    Bitmap bmp(std::string("../copy-move-forgery/resource/icone.bmp"));

    const int BLOCK_SIZE = 20;

    Timer timeOld;
    CharVectList* vListOld = ForgingDetectorOld::charactVector(bmp, BLOCK_SIZE);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    CharVectList* vListNew = ForgingDetectorTest::charactVector(bmp, BLOCK_SIZE);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    assertEqualsCharVectList(vListOld, vListNew);
}

TEST_F(ForgingDetectorTest, createSimilarBlockList)
{
    CharVectList* vList = getCopyOfCharacVec();

    Timer timeOld;
    SimilarBlocksOld* simBlkOld = ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    SimilarBlocks* simBlkNew = createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    assertEqualsSimilarBlocks(simBlkOld, simBlkNew);

    LinkedListCleaner::clear(vList);
}

TEST_F(ForgingDetectorTest, filterSpuriousRegions)
{
    CharVectList* vList = getCopyOfCharacVec();
    SimilarBlocksOld* simBlkOld = NULL;
    SimilarBlocks* simBlkNew = NULL;

    simBlkOld = ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);
    Timer timeOld;
    ForgingDetectorOld::filterSpuriousRegions(simBlkOld);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    simBlkNew = createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);
    Timer timeNew;
    filterSpuriousRegions(simBlkNew);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    assertEqualsSimilarBlocks(simBlkOld, simBlkNew);

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    LinkedListCleaner::clear(vList);
}

TEST_F(ForgingDetectorTest, getMainShiftVector)
{
    {
    SimilarBlocks *headNew, *auxNew;
    auxNew = headNew      = new SimilarBlocks(Pos(0,0), Pos(1,1));
    auxNew = auxNew->next = new SimilarBlocks(Pos(0,0), Pos(2,2));
    auxNew = auxNew->next = new SimilarBlocks(Pos(0,0), Pos(1,1));
    auxNew = auxNew->next = new SimilarBlocks(Pos(0,0), Pos(2,2));
    DeltaPos deltaPos = getMainShiftVector(headNew);

    SimilarBlocksOld *headOld, *auxOld;
    auxOld = headOld      = new SimilarBlocksOld(Pos(0,0), Pos(1,1));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(2,2));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(1,1));
    auxOld = auxOld->next = new SimilarBlocksOld(Pos(0,0), Pos(2,2));
    SimilarBlocksOld *deltaPosOld = ForgingDetectorOld::getMainShiftVector(headOld);

    ASSERT_EQ(deltaPos.dx, deltaPosOld->dx);
    ASSERT_EQ(deltaPos.dy, deltaPosOld->dy);
    }

    CharVectList* vList = getCopyOfCharacVec();
    SimilarBlocksOld* simBlkOld =
            ForgingDetectorOld::createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);
    SimilarBlocks* simBlkNew =
            createSimilarBlockList(BITMAP, BLOCK_SIZE, vList);

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
}
