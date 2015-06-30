#ifndef FORGINGDETECTOROMP_H_
#define FORGINGDETECTOROMP_H_

#include "utils.h"
#include "Bitmap.h"

class ForgingDetectorOMP
{

#ifdef _DEBUG_
    static int dbgmsg;
#endif

public:
    static bool isTampered(Bitmap const& image, int bSize = 16);
    static Bitmap imageOpeningOperation(Bitmap const& image, int bSize);
    static Bitmap imageDilationOperation(Bitmap const& image, int bSize);
    static Bitmap imageErosionOperation(Bitmap const& image, int bSize);

protected:
    ForgingDetectorOMP()
    {
    }
    ~ForgingDetectorOMP()
    {
    }
    /* metodos de deteccao */
    static void charactVector(ListCharVectPtr& listChar, Bitmap const& image, int bSize);
    static void getCharVectListForBlock(CharVect& charVect, Bitmap const& image, int blkPosX, int blkPosY, int blkSize);
    static void addVectLexOrder(ListCharVectPtr& orderedVector, CharVect* charVect);
    static void createSimilarBlockList(Bitmap const& image, int bSize, ListCharVectPtr const& vList, ListSimilarBlocks &simList);
    static void filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift);

    static bool isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift);
    static int getShift(Pos const& pos1, Pos const& pos2);
    static DeltaPos getMainShiftVector(ListSimilarBlocks const& blocks);
    static void createImageWithSimilarAreas(Bitmap& detectImage, Bitmap const& image, int bSize, ListSimilarBlocks const& simList);
    static bool isImageForged(Bitmap const& image, Bitmap const& detectImage, Bitmap& mergedImage);
};

#endif
