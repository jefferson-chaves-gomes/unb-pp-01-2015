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
    static bool byCharact(Bitmap image, bool multiregion = false, int bSize = 16);
    static Bitmap opening(Bitmap image, int bSize);
    static Bitmap dilation(Bitmap image, int bSize);
    static Bitmap erosion(Bitmap image, int bSize);

protected:
    ForgingDetector()
    {
    }
    ~ForgingDetector()
    {
    }
    /* metodos de deteccao */
    static CharVectList* charactVector(Bitmap const& image, int bSize);
    static CharVectList* getCharVectListForBlock(Bitmap const& image, int blkPosX, int blkPosY, int blkSize);
    static CharVectList* addVectLexOrder(CharVectList* start, CharVectList* vetor);
    static SimilarBlocks* createSimilarBlockList(Bitmap const& image, int bSize, CharVectList* vList);
    static void filterSpuriousRegions(SimilarBlocks* simList, bool multiregion);

    static int getShift(int x1, int x2, int y1, int y2);
    static MaxShifts getMainShifts(SimilarBlocks* blocks);
    static SimilarBlocks* getMainShiftVector(SimilarBlocks* blocks);
    static bool isGreaterShift(SimilarBlocks* simBlock, MaxShifts maxSh, int maxShift);
};

#endif
