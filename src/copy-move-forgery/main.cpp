#include "functions.h"
#include "ForgingDetector.h"
#include "ForgingDetectorOld.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include "Timer.h"
#include <omp.h>

#ifdef MPI_ENABLED
#include <mpi.h>
#endif

// TEMP: Não consegui por na functions.h.... sei lah pq... da erro...
void printResult(const bool, const std::string);

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

    if (argc < 2 || std::string(argv[1]) != CHARACT_VECTOR) {
        printUsage();
        exit(EXIT_SUCCESS);
    }

    // Serial process
//    Timer serialTime;
//    startSerialProcess(argc, argv);
//    std::cout << "Serial time for file: " << argv[2] << std::endl;
//    std::cout << serialTime.elapsedMicroseconds() << std::endl;
    // Parallel process
    Timer openMpTime;
    startOpenMPProcess(argc, argv);
    std::cout << "OpenMP time for file: " << argv[2] << std::endl;
    std::cout << openMpTime.elapsedMicroseconds() << std::endl;

#ifdef MPI_ENABLED
}
MPI::Finalize();
#endif

    return EXIT_SUCCESS;
}

void startSerialProcess(int argc, char *argv[]) {
    int blockSize = BLOCK_SIZE;
    if (argc == 4)
        blockSize = atoi(argv[3]);

    bool tampered = ForgingDetector::isTampered(Bitmap(argv[2]), blockSize);
    printResult(tampered, std::string(argv[2]));
}

void startOpenMPProcess(int argc, char *argv[]) {
    int blockSize = BLOCK_SIZE;
    if (argc == 4)
        blockSize = atoi(argv[3]);

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

//    bool tampered = ForgingDetector::isTampered(Bitmap(argv[2]), blockSize);
//    printResult(tampered, std::string(argv[2]));

}

void printResult(const bool tampered, const std::string fileName) {
    if (tampered)
        std::cout << "Tampering was detected in image '" << fileName << "'." << std::endl;
    else
        std::cout << "Image '" << fileName << "' is assumed to be authentic." << std::endl;
}
