
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
    #ifdef MPI_ENABLED
    startMPIProcess(argc, argv);
    #else
    startSerialProcess(argc, argv);
    #endif
}

#ifdef MPI_ENABLED

void finalizeExecution(int error)
{
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}

void startMPIProcess(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    Timer serialTime(true, __PRETTY_FUNCTION__, __LINE__, "Forging detector FINISHED!!!");

    if (argc < 3)
    {
        if(MPISettings::IS_PROC_ID_MASTER())
            printUsage();
        finalizeExecution(EXIT_FAILURE);
    }

    if(MPISettings::IS_PROC_ID_MASTER())
        std::cout << "Initializing MPI processing..." << std::endl;

    int bSize = BLOCK_SIZE;
    std::string imagePath;
    if(MPISettings::IS_PROC_ID_MASTER())
    {
        bSize = atoi(argv[2]);
        imagePath = argv[1];
    }

    MPI_Bcast(&bSize, 1, MPI_INT, MPISettings::PROC_MASTER, MPI_COMM_WORLD);

    bool tampered = false;
    if(MPISettings::IS_PROC_ID_MASTER())
        tampered = ForgingDetector::byCharact(Bitmap(imagePath), bSize);
    else
        ForgingDetector::byCharact(Bitmap(), bSize);

    if(MPISettings::IS_PROC_ID_MASTER())
    {
        if (tampered)
            std::cout << "Tampering was detected in image '" << argv[1] << "'." << std::endl;
        else
            std::cout << "Image '" << argv[1] << "' is assumed to be authentic." << std::endl;
    }

    finalizeExecution(EXIT_SUCCESS);
}
#else
void startSerialProcess(int argc, char *argv[])
{
    if (argc < 3)
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    int bSize = BLOCK_SIZE;
    bool tampered = false;
    bSize = atoi(argv[2]);

    Timer serialTime;
    tampered = ForgingDetector::byCharact(Bitmap(argv[2]), bSize);

    if (tampered)
        std::cout << "Tampering was detected in image '" << argv[1] << "'." << std::endl;
    else
        std::cout << "Image '" << argv[1] << "' is assumed to be authentic." << std::endl;

    std::cout << "Done." << std::endl;

    std::cout << "Serial time for file: " << argv[1] << std::endl;
    std::cout << serialTime.elapsedMicroseconds() << std::endl;
}
#endif

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
