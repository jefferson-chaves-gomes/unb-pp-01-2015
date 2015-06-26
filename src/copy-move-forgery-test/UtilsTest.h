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

class UtilsTest : public ::testing::Test, public ForgingDetector
{
public:

    void printCharVect(CharVect const& charVec)
    {
        std::cout << "|";
        for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        {
            std::cout << (double) charVec.c[i] << "|";
        }
        std::cout << std::endl;
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

    void printDeltaPos(DeltaPos const& delta)
    {
        std::cout << "Delta: " << delta.dx << "|" << delta.dy << std::endl;
    }

    void printSimilarBlocks(ListSimilarBlocks const& auxBlock, int limit = 0)
    {
        int count(0);
        for(ListSimilarBlocks::const_iterator it = auxBlock.begin(); it != auxBlock.end(); it++)
        {
            std::cout << "Block " << count++ << ": ";
            printDeltaPos(it->delta);
            if(limit != 0 && limit <= count)
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
            if(limit != 0 && limit <= count)
                break;
        }
    }

};
