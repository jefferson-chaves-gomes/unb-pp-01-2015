
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "Timer.h"
#include "main.h"

#ifdef MPI_ENABLED
#include <mpi/mpi.h>
#include <MPISettings.h>
#include <ForgingDetectorMPI.h>
void startMPIProcess(int argc, char *argv[]);
#endif

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    #ifdef MPI_ENABLED
    startMPIProcess(argc, argv);
    #else
    startSerialProcess(argc, argv);
    #endif
}

#ifdef MPI_ENABLED
void startMPIProcess(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    if(MPISettings::PROC_ID()==0)
        std::cout << "Initializing MPI processing..." << std::endl;
    if(MPISettings::PROC_ID()==1)
        std::cout << "Initializing MPI processing 1..." << std::endl;

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

    MPI_Finalize();
}
#else
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
#endif

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
