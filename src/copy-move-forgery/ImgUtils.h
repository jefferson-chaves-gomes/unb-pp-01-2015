#ifndef IMGUTILS_H_
#define IMGUTILS_H_

#include <string>
#include "Bitmap.h"

const int BLOCK_SIZE(16);

class ImgUtils
{
    public:
        static std::string imgTrueName(std::string file);
        static void saveImageAs(Bitmap image, std::string imgPath);
        static Bitmap imgGrayScale(Bitmap image, bool fromfile = false);
};

#endif
