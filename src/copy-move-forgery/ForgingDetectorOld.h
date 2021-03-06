#ifndef FORGINGDETECTOROLD_H_
#define FORGINGDETECTOROLD_H_

#include "utils_old.h"
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
    static CharVectListOld* charactVector(Bitmap image, int bSize);
    static CharVectListOld* addVectLexOrder(CharVectListOld* start, CharVectListOld* vetor);
    static SimilarBlocksOld* createSimilarBlockList(Bitmap const& image, int bSize, CharVectListOld* vList);
    static void filterSpuriousRegions(SimilarBlocksOld** simList, SimilarBlocksOld* mainShift);
    static bool isBlockSimilarSpurious(SimilarBlocksOld* current, SimilarBlocksOld* mainShift);
    static int getShift(int x1, int x2, int y1, int y2);
    static SimilarBlocksOld* newSimilarBlock(int x1, int x2, int y1, int y2);
    static SimilarBlocksOld* getMainShiftVector(SimilarBlocksOld* blocks);
    static void clearCharVectors(CharVectListOld* start);
    static void clearSimilarBlocks(SimilarBlocksOld* start);
    static HistogramOld* newHistogramOld(int dx, int dy, SimilarBlocksOld* rep);
    static void clearHistogramOld(HistogramOld* start);
};

#endif
