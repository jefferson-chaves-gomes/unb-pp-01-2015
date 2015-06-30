#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <ForgingDetectorOld.h>
#include <Timer.h>
#include <utils.h>
#include "ImgUtils.h"

#include <iostream>
#include <string>

const std::string IMG_PATH("../copy-move-forgery/resource/publico.bmp");
const Bitmap BITMAP(IMG_PATH);


const std::string IMG_PATH_NORMAL("../copy-move-forgery/resource/icone.bmp");
const Bitmap BITMAP_NORMAL(IMG_PATH_NORMAL);

const std::string IMG_PATH_SMALL("../copy-move-forgery/resource/small.bmp");
const Bitmap BITMAP_SMALL(IMG_PATH_SMALL);

class UtilsTest
{
public:

    static void printCharVect(CharVect const& charVec)
    {
        std::cout << "|";
        for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        {
            std::cout << (double) charVec.c[i] << "|";
        }
        std::cout << std::endl;
    }

    static void printCharVectList(ListCharVect& charVectList)
    {
        for(ListCharVect::iterator it = charVectList.begin(); it != charVectList.end(); it++)
        {
            std::cout << "Pos: " << it->pos.x << "," << it->pos.y << " # ";
            printCharVect(*it);
        }
    }

    static void printDeltaPos(DeltaPos const& delta)
    {
        std::cout << "Delta: " << delta.dx << "|" << delta.dy << std::endl;
    }

    static void printSimilarBlocks(ListSimilarBlocks const& auxBlock, int limit = 0)
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

    static void printSimilarBlocksOld(SimilarBlocksOld* auxBlock, int limit = 0)
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
