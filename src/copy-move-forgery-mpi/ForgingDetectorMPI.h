#ifndef ForgingDetectorMPI_H_
#define ForgingDetectorMPI_H_

#include <utils.h>
#include <Bitmap.h>

class ForgingDetectorMPI
{

#ifdef _DEBUG_
    static int dbgmsg;
#endif

public:
    static bool byCharact(Bitmap const& image, int bSize = 16);
    static Bitmap imageOpeningOperation(Bitmap const& image, int bSize);
    static Bitmap imageDilationOperation(Bitmap const& image, int bSize);
    static Bitmap imageErosionOperation(Bitmap const& image, int bSize);

protected:
    ForgingDetectorMPI()
    {
    }
    ~ForgingDetectorMPI()
    {
    }
    /* metodos de deteccao */
    static void charactVector(ListCharVectPtr& listChar, Bitmap const& image, int imageWidth, int imageHeight, int bSize);
    static void getCharVectListForBlock(CharVect& charVect, Bitmap const& image, int blkPosX, int blkPosY, int blkSize);
    static void createSimilarBlockList(Bitmap const& image, int bSize, ListCharVectPtr & vList, ListSimilarBlocks &simList, int vectOffsetSize);
    static void filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift);
    static int getVectOffsetSize(int imageWidth, int imageHeight, int bSize);

    static bool isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift);
    static int getShift(Pos const& pos1, Pos const& pos2);
    static DeltaPos getMainShiftVector(ListSimilarBlocks const& blocks);
    static void createImageWithSimilarAreas(Bitmap& detectImage, Bitmap const& image, int bSize, ListSimilarBlocks const& simList);
    static bool isImageForged(Bitmap const& image, Bitmap const& detectImage, Bitmap& mergedImage);
};

#endif
