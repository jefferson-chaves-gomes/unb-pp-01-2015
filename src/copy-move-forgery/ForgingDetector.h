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
    static bool byCharact(Bitmap const& image, int bSize = 16);
    static Bitmap opening(Bitmap const& image, int bSize);
    static Bitmap dilation(Bitmap const& image, int bSize);
    static Bitmap erosion(Bitmap const& image, int bSize);

protected:
    ForgingDetector()
    {
    }
    ~ForgingDetector()
    {
    }
    /* metodos de deteccao */
    static CharVectListOld* charactVector(Bitmap const& image, int bSize);
    static CharVectListOld* getCharVectListForBlock(Bitmap const& image, int blkPosX, int blkPosY, int blkSize);
    static CharVectListOld* addVectLexOrder(CharVectListOld* start, CharVectListOld* vetor);
    static void createSimilarBlockList(Bitmap const& image, int bSize, CharVectListOld* vList, ListSimilarBlocks &simList);
    static void filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift);

    static bool isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift);
    static int getShift(Pos const& pos1, Pos const& pos2);
    static DeltaPos getMainShiftVector(ListSimilarBlocks const& blocks);
};

#endif
