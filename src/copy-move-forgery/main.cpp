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

int main(int argc, char *argv[]) {
#if defined(MPI_ENABLED)
    startMPIProcess(argc, argv);
#elif defined(OMP_ENABLED)
    // Parallel process
    Timer openMpTime;
    startOpenMPProcess(argc, argv);
    std::cout << "OpenMP time for file: " << argv[2] << std::endl;
    std::cout << openMpTime.elapsedMicroseconds() << std::endl;
#else
    // Serial process
//    Timer serialTime;
//    startSerialProcess(argc, argv);
//    std::cout << "Serial time for file: " << argv[2] << std::endl;
//    std::cout << serialTime.elapsedMicroseconds() << std::endl;
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
void startOpenMPProcess(int argc, char *argv[])
{
    if (argc < 3) {
        printUsage();
        exit(EXIT_SUCCESS);
    }
    int coresCount = omp_get_num_procs();
    omp_set_num_threads(coresCount);
#pragma omp parallel
    {
        int threadId, threadsRegionCount, maxTreadsCount;
        coresCount = omp_get_num_procs();
        maxTreadsCount = omp_get_max_threads();
        threadId = omp_get_thread_num();
        threadsRegionCount = omp_get_num_threads();
        if (threadId == MASTER_THREAD) {
            printf("t%i : coresCount\t\t= %i\n", threadId, coresCount);
            printf("t%i : maxTreadsCount\t= %i\n", threadId, maxTreadsCount);
            printf("t%i : threadsRegionCount\t= %i\n\n", threadId, threadsRegionCount);
        }
        printf("t%i is present\n", threadId);
    }
    printf("\n");

    int blockSize = BLOCK_SIZE;
    if (argc == 4)
        blockSize = atoi(argv[3]);
    bool tampered = ForgingDetectorOMP::isTampered(Bitmap(argv[2]), blockSize);
    printResult(tampered, argv[2]);

}
#else
void startSerialProcess(int argc, char *argv[]) {
    if (argc < 3) {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    int blockSize = BLOCK_SIZE;
    if (argc == 4)
        blockSize = atoi(argv[3]);

    bool tampered = ForgingDetector::isTampered(Bitmap(argv[2]), blockSize);
    printResult(tampered, std::string(argv[2]));
}
#endif

void printResult(const bool tampered, std::string const& fileName)
{
    if (tampered)
        std::cout << "Tampering was detected in image '" << fileName << "'." << std::endl;
    else
        std::cout << "Image '" << fileName << "' is assumed to be authentic." << std::endl;
}

void printUsage()
{
    std::cout << "Usage: CustomBitmap.exe <img_file> <block_size...>" << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}
