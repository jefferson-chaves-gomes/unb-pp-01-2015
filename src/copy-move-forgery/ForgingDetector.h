#ifndef FORGINGDETECTOR_H_
#define FORGINGDETECTOR_H_

#include "utils.h"
#include "Bitmap.h"

class ForgingDetector
{

#ifdef _DEBUG_
    static int dbgmsg;
#endif

public:
    static bool isTampered(Bitmap const& image, int bSize = 16);
    static Bitmap imageOpeningOperation(Bitmap const& image, int bSize);
    static Bitmap imageDilationOperation(Bitmap const& image, int bSize);
    static Bitmap imageErosionOperation(Bitmap const& image, int bSize);

    static void charactVector(ListCharVect& listChar, Bitmap const& image, int bSize);
    static void createSimilarBlockList(Bitmap const& image, int bSize, ListCharVect const& vList, ListSimilarBlocks &simList);
    static void filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift);

protected:
    ForgingDetector()
    {
    }
    ~ForgingDetector()
    {
    }
    /* metodos de deteccao */
    static void charactVectorBySections(ListCharVect& listChar, Bitmap const& image, unsigned int bSize, unsigned int sections);
    static void getCharVectListForBlock(CharVect& charVect, Bitmap const& image, int blkPosX, int blkPosY, int blkSize);
    static void addVectLexOrder(ListCharVect& orderedVector, CharVect& charVect);

    static bool isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift);
    static int getShift(Pos const& pos1, Pos const& pos2);
    static DeltaPos getMainShiftVector(ListSimilarBlocks const& blocks);
    static void createImageWithSimilarAreas(Bitmap& detectImage, Bitmap const& image, int bSize, ListSimilarBlocks const& simList);
    static bool isImageForged(Bitmap const& image, Bitmap const& detectImage, Bitmap& mergedImage);
};

#endif
