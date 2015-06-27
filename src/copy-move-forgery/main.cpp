
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "Timer.h"
#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    startSerialProcess(argc, argv);
}

void startSerialProcess(int argc, char *argv[])
{
    int bSize = BLOCK_SIZE;
    bool tampered = false;
    if (argc == 4)
        bSize = atoi(argv[3]);

    Timer serialTime;
    tampered = ForgingDetector::byCharact(Bitmap(argv[2]), bSize);

    if (tampered)
        std::cout << "Tampering was detected in image '" << argv[2] << "'." << std::endl;
    else
        std::cout << "Image '" << argv[2] << "' is assumed to be authentic." << std::endl;

    std::cout << "Done." << std::endl;

    std::cout << "Serial time for file: " << argv[2] << std::endl;
    std::cout << serialTime.elapsedMicroseconds() << std::endl;
}

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
