#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <ForgingDetectorOld.h>
#include <Timer.h>
#include <utils.h>

#include <iostream>
#include <string>

const int BLOCK_SIZE = 16;
const std::string IMG_PATH("../copy-move-forgery/resource/publico.bmp");
const Bitmap BITMAP(IMG_PATH);


class ForgingDetectorTest :
        public ::testing::Test,
        public ForgingDetector
{
public:

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
