#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * Prints the number of threads and the current thread id
 */
int main(int argc, char *argv[]) {

    int numThreads, threadId;

    /* This creates a team of threads; each thread has own copy of variables  */
    #pragma omp parallel private(numThreads, threadId)
    {
        threadId = omp_get_thread_num();
        printf("Hello World from thread number %d\n", threadId);

        /* The following is executed by the master thread only (threadId=0) */
        if (threadId == 0) {
            numThreads = omp_get_num_threads();
            printf("Number of threads is %d\n", numThreads);
        }
    }
    return 0;
}
