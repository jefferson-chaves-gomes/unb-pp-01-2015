#ifndef MAIN_H_
#define MAIN_H_

#ifdef MPI_ENABLED
#include <mpi.h>
#include "../ForgingDetectorMPI.h"
void startMPIProcess(int, char*[]);
#else
void startSerialProcess(int, char*[]);
#endif

void printUsage();

#endif
