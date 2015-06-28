#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include "Timer.h"
#include "main.h"

#if defined(MPI_ENABLED)
#include <mpi/mpi.h>
#include <MPISettings.h>
#include <ForgingDetectorMPI.h>
#elif defined(OMP_ENABLED)
#include <omp.h>
#include <ForgingDetectorOMP.h>
#endif

int main(int argc, char *argv[])
{
#if defined(MPI_ENABLED)
    // MPI Parallel process
    Timer mpiTime;
    startMPIProcess(argc, argv);
    std::cout << "OpenMP time for file: " << argv[2] << std::endl;
    std::cout << mpiTime.elapsedMicroseconds() << std::endl;
#elif defined(OMP_ENABLED)
    // OMP Parallel process
    Timer openMpTime;
    startOmpProcess(argc, argv);
    std::cout << "OpenMP time for file: " << argv[2] << std::endl;
    std::cout << openMpTime.elapsedMicroseconds() << std::endl;
#else
    // Serial process
    Timer serialTime;
    startSerialProcess(argc, argv);
    std::cout << "Serial time for file: " << argv[2] << std::endl;
    std::cout << serialTime.elapsedMicroseconds() << std::endl;
#endif
}

#if defined(MPI_ENABLED)

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
    Bitmap image;
    if(MPISettings::IS_PROC_ID_MASTER())
    {
        bSize = atoi(argv[2]);
        imagePath = argv[1];
        image.load_bitmap(imagePath);
    }

    bool tampered = ForgingDetectorMPI::byCharact(image, bSize);

    if(MPISettings::IS_PROC_ID_MASTER())
        printResult(tampered, std::string(argv[1]));

    finalizeExecution(EXIT_SUCCESS);
}
#elif defined(OMP_ENABLED)
void startOmpProcess(int argc, char *argv[])
{
    validateArgs(argc);
    int blockSize = BLOCK_SIZE;
    if(argc == 4)
        blockSize = atoi(argv[3]);
    bool tampered = ForgingDetectorOMP::isTampered(Bitmap(argv[2]), blockSize);
    printResult(tampered, argv[2]);

}
#else
void startSerialProcess(int argc, char *argv[])
{
    validateArgs(argc);
    int blockSize = BLOCK_SIZE;
    if (argc == 4)
        blockSize = atoi(argv[3]);

    bool tampered = ForgingDetector::isTampered(Bitmap(argv[2]), blockSize);
    printResult(tampered, std::string(argv[2]));
}
#endif

void validateArgs(const int argc)
{
    if(argc < 3)
    {
        printUsage();
        exit(EXIT_SUCCESS);
    }
}


void printResult(const bool tampered, std::string const& fileName)
{
    if(tampered)
        std::cout << "Tampering was detected in image '" << fileName << "'." << std::endl;
    else
        std::cout << "Image '" << fileName << "' is assumed to be authentic." << std::endl;
}

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
