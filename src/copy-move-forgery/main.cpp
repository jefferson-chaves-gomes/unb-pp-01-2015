#include "functions.h"
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "Timer.h"

#ifdef MPI_ENABLED
#include <mpi.h>
#endif

int main(int argc, char *argv[]) {

#ifdef MPI_ENABLED
    MPI::Init(argc, argv);
    int PROC_SIZE(0);
    int PROC_ID(0);

    MPI_Comm_size(MPI_COMM_WORLD, &PROC_SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &PROC_ID);

    std::cout << "EXECUTOU MPI" << std::endl;

    // to remove boolean
    if(false && PROC_ID==0)
    {

#endif

    if (argc < 2
            || (std::string(argv[1]) != CHARACT_VECTOR && std::string(argv[1]) != EROSION && std::string(argv[1]) != DILATION && std::string(argv[1]) != OPENING && std::string(argv[1]) != GRAYSCALE)) {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    startSerialProcess(argc, argv);

#ifdef MPI_ENABLED
}
MPI::Finalize();
#endif

    return EXIT_SUCCESS;
}

void startSerialProcess(int argc, char *argv[]) {
    /* escolha de modo de operacao */
    if (std::string(argv[1]) == CHARACT_VECTOR) {
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
//    else if(std::string(argv[1]) == GRAYSCALE)
//    {
//        ImgUtils::imgGrayScale(Bitmap(argv[2]), true);
//        std::cout << "Done." << std::endl;
//    }
//    else if(std::string(argv[1]) == EROSION)
//    {
//        Bitmap eroded = ForgingDetector::erosion(Bitmap(argv[2]), atoi(argv[3]));
//        std::string path;
//        path.append(ImgUtils::imgTrueName(argv[2]));
//        path.append(std::string("_eroded.bmp"));
//        ImgUtils::saveImageAs(eroded, path);
//        std::cout << "Done." << std::endl;
//    }
//    else if(std::string(argv[1]) == DILATION)
//    {
//        Bitmap dilated = ForgingDetector::dilation(Bitmap(argv[2]), atoi(argv[3]));
//        std::string path;
//        path.append(ImgUtils::imgTrueName(argv[2]));
//        path.append(std::string("_dilated.bmp"));
//        ImgUtils::saveImageAs(dilated, path);
//        std::cout << "Done." << std::endl;
//    }
//    else if(std::string(argv[1]) == OPENING)
//    {
//        Bitmap opened = ForgingDetector::opening(Bitmap(argv[2]), atoi(argv[3]));
//        std::string path;
//        path.append(ImgUtils::imgTrueName(argv[2]));
//        path.append(std::string("_opened.bmp"));
//        ImgUtils::saveImageAs(opened, path);
//        std::cout << "Done." << std::endl;
//    }
}
