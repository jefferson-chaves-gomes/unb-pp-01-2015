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
    startMPIProcess(argc, argv);
#elif defined(OMP_ENABLED)
    // OMP Parallel process
    startOmpProcess(argc, argv);
#else
    // Serial process
    startSerialProcess(argc, argv);
#endif

    return EXIT_SUCCESS;
}

#if defined(MPI_ENABLED)

void startMPIProcess(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    validateArgs(argc);

    int bSize = BLOCK_SIZE;
    std::string imagePath;
    Bitmap image;
    if(MPISettings::IS_PROC_ID_MASTER())
    {
        bSize = atoi(argv[2]);
        imagePath = argv[1];
        image.load_bitmap(imagePath);
    }

    Timer time(true, argv[1]); // nao entendi pq o destrutor nao é chamado apos eu usar esse construtor....
    bool tampered = ForgingDetectorMPI::isTampered(image, bSize);

    if(MPISettings::IS_PROC_ID_MASTER()) // foi necessario fazer isto devido o destrutur nao ser chamado
        std::cout << "[" << time.elapsedMicroseconds() << "]\t" << argv[1] << std::endl;

//    if(MPISettings::IS_PROC_ID_MASTER())
//        printResult(tampered, std::string(argv[1]));

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
#elif defined(OMP_ENABLED)
void startOmpProcess(int argc, char *argv[])
{
    validateArgs(argc);
    int blockSize, ompNumThreads, ompSchedOrdinal;
    readOmpArgs(argc, argv, blockSize, ompNumThreads, ompSchedOrdinal);
    omp_sched_t ompSchedule = static_cast<omp_sched_t>(ompSchedOrdinal);
    Timer time(true, argv[1]);
    bool tampered = ForgingDetectorOMP::isTampered(Bitmap(argv[1]), blockSize, ompNumThreads, ompSchedule);
//    printResult(tampered, argv[1]);
}

void readOmpArgs(int argc, char *argv[], int& blockSize, int& ompNumThreads, int& ompSchedule)
{
    blockSize = BLOCK_SIZE;
    ompNumThreads = omp_get_num_procs();
    ompSchedule = static_cast<int>(omp_sched_dynamic);
    std::string scheduler;
    switch(argc)
    {
        case 3:
            blockSize = atoi(argv[2]);
            break;
        case 4:
            blockSize = atoi(argv[2]);
            ompNumThreads = atoi(argv[3]);
            break;
        case 5:
            blockSize = atoi(argv[2]);
            ompNumThreads = atoi(argv[3]);
            scheduler = argv[4];
            if(scheduler == "dynamic")
                ompSchedule = static_cast<int>(omp_sched_dynamic);
            else if(scheduler == "static")
                ompSchedule = static_cast<int>(omp_sched_static);
            else if(scheduler == "guided")
                ompSchedule = static_cast<int>(omp_sched_guided);
            else if(scheduler == "auto")
                ompSchedule = static_cast<int>(omp_sched_auto);
            break;
        default:
            break;
    }
}

#else
void startSerialProcess(int argc, char *argv[])
{
    validateArgs(argc);
    int blockSize = BLOCK_SIZE;
    if (argc == 3)
        blockSize = atoi(argv[2]);

    Timer time(true, argv[1]);
    bool tampered = ForgingDetector::isTampered(Bitmap(argv[1]), blockSize);
//    printResult(tampered, std::string(argv[1]));
}
#endif

void validateArgs(const int argc)
{
    int numArgs = 2;
#if defined(MPI_ENABLED)
    numArgs = 3;
#endif
    if(argc < numArgs)
    {
#if defined(MPI_ENABLED)
        if(MPISettings::IS_PROC_ID_MASTER())
            printUsage();
        MPI_Finalize();
#else
        printUsage();
#endif
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

#if defined(MPI_ENABLED)
    std::cout << "Usage: mpirun -n <num_process> copy-move-forgery-mpi ";
#elif defined(OMP_ENABLED)
    std::cout << "Usage: copy-move-forgery-omp ";
#else
    std::cout << "Usage: copy-move-forgery ";
#endif
    std::cout << "<img_file> <block_size (default: 16)> ";
#if defined(OMP_ENABLED)
    std::cout
            << "<omp_num_thread (default: cores count)> <omp_schedule [dynamic|static|guided|auto] (default: dynamic)>";
#endif
    std::cout << std::endl;
}
