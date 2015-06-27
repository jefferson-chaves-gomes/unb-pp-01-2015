#include "MPISettings.h"

#include <mpi/mpi.h>

MPISettings::MPISettings() :
    PROC_SIZE_(0),
    PROC_ID_(0)
{
    MPI_Comm_size(MPI_COMM_WORLD, &PROC_SIZE_);
    MPI_Comm_rank(MPI_COMM_WORLD, &PROC_ID_);
}

MPISettings& MPISettings::instance()
{
    static MPISettings inst;
    return inst;
}

int MPISettings::PROC_SIZE()
{
    return instance().PROC_SIZE_;
}

int MPISettings::PROC_ID()
{
    return instance().PROC_ID_;
}
