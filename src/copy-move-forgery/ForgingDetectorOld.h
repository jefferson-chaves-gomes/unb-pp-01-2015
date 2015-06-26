#ifndef FORGINGDETECTOROLD_H_
#define FORGINGDETECTOROLD_H_

#include "utils.h"
#include "Bitmap.h"

class ForgingDetectorOld
{

#ifdef _DEBUG_
    static int dbgmsg;
#endif

public:
    static bool byCharact(Bitmap image, int bSize = 16);
    static Bitmap opening(Bitmap image, int bSize);
    static Bitmap dilation(Bitmap image, int bSize);
    static Bitmap erosion(Bitmap image, int bSize);

//protected:
    ForgingDetectorOld()
    {
    }
    ~ForgingDetectorOld()
    {
    }
    /* metodos de deteccao */
    static CharVectList* charactVector(Bitmap image, int bSize);
    static CharVectList* addVectLexOrder(CharVectList* start, CharVectList* vetor);
    static SimilarBlocksOld* createSimilarBlockList(Bitmap const& image, int bSize, CharVectList* vList);
    static void filterSpuriousRegions(SimilarBlocksOld** simList, SimilarBlocksOld* mainShift);
    static bool isBlockSimilarSpurious(SimilarBlocksOld* current, SimilarBlocksOld* mainShift);
    static int getShift(int x1, int x2, int y1, int y2);
    static SimilarBlocksOld* newSimilarBlock(int x1, int x2, int y1, int y2);
    static SimilarBlocksOld* getMainShiftVector(SimilarBlocksOld* blocks);
    static void clearCharVectors(CharVectList* start);
    static void clearSimilarBlocks(SimilarBlocksOld* start);
    static HistogramOld* newHistogramOld(int dx, int dy, SimilarBlocksOld* rep);
    static void clearHistogramOld(HistogramOld* start);
};

#endif
