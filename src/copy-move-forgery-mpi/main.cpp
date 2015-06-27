
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "Timer.h"
#include "main.h"

#include <mpi/mpi.h>
#include "ForgingDetectorMPI.h"
#include "MPISettings.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    startMPIProcess(argc, argv);

    return EXIT_SUCCESS;
}

void startMPIProcess(int argc, char *argv[])
{
    MPI::Init(argc, argv);

    if(MPISettings::PROC_ID()==0)
        std::cout << "Initializing MPI processing..." << std::endl;

    if(MPISettings::PROC_ID()==0)
    {
        int bSize = BLOCK_SIZE;
        bool tampered = false;
        if (argc == 4)
            bSize = atoi(argv[3]);

        Timer serialTime;
        tampered = ForgingDetectorMPI::byCharact(Bitmap(argv[2]), bSize);

        if (tampered)
            std::cout << "Tampering was detected in image '" << argv[2] << "'." << std::endl;
        else
            std::cout << "Image '" << argv[2] << "' is assumed to be authentic." << std::endl;

        std::cout << "Done." << std::endl;

        std::cout << "Serial time for file: " << argv[2] << std::endl;
        std::cout << serialTime.elapsedMicroseconds() << std::endl;
    }

    MPI::Finalize();
}

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
