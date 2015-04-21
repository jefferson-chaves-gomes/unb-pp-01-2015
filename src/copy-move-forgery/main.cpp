
#include "functions.h"
#include "ForgingDetector.h"
#include "ImgUtils.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printUsage();
        exit(0);
    }

    if(std::string(argv[1]) != CHARACT_VECTOR &&
       std::string(argv[1]) != MULT_CHARACT_VECTOR &&
       std::string(argv[1]) != EROSION &&
       std::string(argv[1]) != DILATION &&
       std::string(argv[1]) != OPENING &&
       std::string(argv[1]) != GRAYSCALE)
    {
        printUsage();
        exit(0);
    }

    /* escolha de modo de operacao */
    if(std::string(argv[1]) == CHARACT_VECTOR || std::string(argv[1]) == MULT_CHARACT_VECTOR)
    {
        int bSize = 16;
        int multi = false;
        bool tampered = false;
        if(std::string(argv[1]) == MULT_CHARACT_VECTOR)
            multi = true;
        if(argc == 4)
            bSize = atoi(argv[3]);
        tampered = ForgingDetector::byCharact(Bitmap(argv[2]), multi, bSize);
        
        if(tampered)
            std::cout << "Tampering was detected in image '" << argv[2] << "'." << std::endl;
        else
            std::cout << "Image '" << argv[2] << "' is assumed to be authentic." << std::endl;
        
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == GRAYSCALE)
    {
        ImgUtils::imgGrayScale(Bitmap(argv[2]), true);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == EROSION)
    {
        Bitmap eroded = ForgingDetector::erosion(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_eroded.bmp"));
        ImgUtils::saveImageAs(eroded, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == DILATION)
    {
        Bitmap dilated = ForgingDetector::dilation(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_dilated.bmp"));
        ImgUtils::saveImageAs(dilated, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == OPENING)
    {
        Bitmap opened = ForgingDetector::opening(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_opened.bmp"));
        ImgUtils::saveImageAs(opened, path);
        std::cout << "Done." << std::endl;
    }
    else
        printUsage();

    return 0;
}

