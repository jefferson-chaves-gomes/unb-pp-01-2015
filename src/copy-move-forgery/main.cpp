#include "functions.h"
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>   // for gettimeofday

int main(int argc, char *argv[])
{
    if(argc < 2
            || (std::string(argv[1]) != CHARACT_VECTOR
                && std::string(argv[1]) != EROSION
                && std::string(argv[1]) != DILATION
                && std::string(argv[1]) != OPENING
                && std::string(argv[1]) != GRAYSCALE))
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    startSerialProcess(argc, argv);

    return EXIT_SUCCESS;
}

void startSerialProcess(int argc, char *argv[])
{

    timeval startTime;
    gettimeofday(&startTime, NULL);

    /* escolha de modo de operacao */
    if(std::string(argv[1]) == CHARACT_VECTOR)
    {
        int bSize = 16;
        bool tampered = false;
        if(argc == 4)
            bSize = atoi(argv[3]);
        tampered = ForgingDetector::byCharact(Bitmap(argv[2]), bSize);

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
        Bitmap eroded = ForgingDetector::imageErosionOperation(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_eroded.bmp"));
        ImgUtils::saveImageAs(eroded, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == DILATION)
    {
        Bitmap dilated = ForgingDetector::imageDilationOperation(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_dilated.bmp"));
        ImgUtils::saveImageAs(dilated, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == OPENING)
    {
        Bitmap opened = ForgingDetector::imageOpeningOperation(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(ImgUtils::imgTrueName(argv[2]));
        path.append(std::string("_opened.bmp"));
        ImgUtils::saveImageAs(opened, path);
        std::cout << "Done." << std::endl;
    }

    timeval endTime;
    gettimeofday(&endTime, NULL);
    long time = ((endTime.tv_sec * 1000000 + endTime.tv_usec) - (startTime.tv_sec * 1000000 + startTime.tv_usec));
    std::cout << "Time for file: " << argv[2] << std::endl;
    std::cout << time << std::endl;
}
