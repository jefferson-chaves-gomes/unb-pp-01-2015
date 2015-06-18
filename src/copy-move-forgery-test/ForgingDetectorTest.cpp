#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <utils.h>

#include <iostream>
#include <string>
#include "Timer.h"

class ForgingDetectorTest : public ::testing::Test, public ForgingDetector
{
protected:

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

    void printCharVect(CharVect* charVec)
    {
        if(charVec == NULL)
        {
            std::cout << "VEC IS NULL" << std::endl;
            return;
        }

        std::cout << "|";
        for(int i = 0; i < CHARS_SIZE; i++)
        {
            std::cout << (int)charVec->c[i] << "|";
        }
        std::cout << std::endl;
    }

    void assertLexicalOrderCharVectList(CharVectList* charVectList)
    {
        CharVectList* aux = charVectList;

        while(aux != NULL)
        {
            if(aux->next !=NULL)
                ASSERT_TRUE(aux->vect <= aux->next->vect);
            aux = aux->next;
        }
    }

    void assertEqualsCharVectList(CharVectList* left, CharVectList*  right)
    {
        ASSERT_TRUE(left != NULL);
        ASSERT_TRUE(right != NULL);

        while(left != NULL || right != NULL)
        {
            ASSERT_TRUE(left != NULL);
            ASSERT_TRUE(right != NULL);

            ASSERT_EQ(left->vect.x, right->vect.x);
            ASSERT_EQ(left->vect.y, right->vect.y);

            for(int i = 0; i < CHARS_SIZE; i++)
                ASSERT_EQ(left->vect.c[i], right->vect.c[i]);

            left = left->next;
            right = right->next;
        }
    }
};

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

    orderedOld = addVectLexOrder(orderedOld, secndOld);
    ASSERT_EQ(orderedOld, secndOld);
    orderedOld = addVectLexOrder(orderedOld, firstOld);
    orderedOld = addVectLexOrder(orderedOld, thirdOld);
    orderedOld = addVectLexOrder(orderedOld, thirdRepOld);

    orderedNew = addVectLexOrderNew(orderedNew, secndNew);
    ASSERT_EQ(orderedNew, secndNew);
    orderedNew = addVectLexOrderNew(orderedNew, firstNew);
    orderedNew = addVectLexOrderNew(orderedNew, thirdNew);
    orderedNew = addVectLexOrderNew(orderedNew, thirdRepNew);
    assertEqualsCharVectList(orderedOld, orderedNew);

    orderedNew = addVectLexOrderNew(orderedNew, secndOld);
    orderedNew = addVectLexOrderNew(orderedNew, thirdRepOld);
    orderedNew = addVectLexOrderNew(orderedNew, firstOld);
    orderedNew = addVectLexOrderNew(orderedNew, thirdOld);
    assertLexicalOrderCharVectList(orderedNew);

    thirdRepOld->next = NULL;
    orderedNew = NULL;
    orderedNew = addVectLexOrderNew(orderedNew, thirdRepOld);
    orderedNew = addVectLexOrderNew(orderedNew, thirdNew);
    orderedNew = addVectLexOrderNew(orderedNew, secndOld);
    orderedNew = addVectLexOrderNew(orderedNew, firstNew);
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

TEST_F(ForgingDetectorTest, charac_vec)
{
    Bitmap bmp(std::string("../copy-move-forgery/resource/icone.bmp"));

    const int BLOCK_SIZE = 20;

    Timer timeOld;
    CharVectList* vListOld = ForgingDetectorTest::charactVector(bmp, BLOCK_SIZE);
    long double elapsedOld = timeOld.elapsedMicroseconds();

    Timer timeNew;
    CharVectList* vListNew = ForgingDetectorTest::charactVectorNew(bmp, BLOCK_SIZE);
    long double elapsedNew = timeNew.elapsedMicroseconds();

    std::cout << "Old: " << elapsedOld << std::endl;
    std::cout << "New: " << elapsedNew << std::endl;
    std::cout << "Speedup: " << (elapsedOld / elapsedNew) << std::endl;

    assertEqualsCharVectList(vListOld, vListNew);
}
