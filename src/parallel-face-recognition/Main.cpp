
#include <iostream>
#include <omp.h>
#include <boost/thread.hpp>

void printHelloWorld(int tid)
{
	std::cout << "Hello World from thread number: " << tid << std::endl;
}

void printWithOpenMP(int numThreads)
{
	std::cout << "########## Printing with OpenMP" << std::endl;
#pragma omp parallel shared(numThreads) num_threads(numThreads)
	printHelloWorld(omp_get_thread_num());
}

void printWithBoost(int numThreads)
{
	std::cout << "########## Printing with Boost" << std::endl;
	boost::thread_group group;
	for(int i=0; i<numThreads; i++)
	     group.create_thread(boost::bind(&printHelloWorld, i));
	group.join_all();
}

int main (int argc, char *argv[])
{
	const int numThreads(8);

	printWithOpenMP(numThreads);
	printWithBoost(numThreads);

	return 0;
}
