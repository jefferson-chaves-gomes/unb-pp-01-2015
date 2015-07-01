#ifndef MAIN_H_
#define MAIN_H_

#include <string>

void validateArgs(const int);
void printUsage();
void readOmpArgs(int, char*[], int&, int&, int&);
void startSerialProcess(int, char*[]);
void startOmpProcess(int, char*[]);
void startMPIProcess(int, char*[]);
void printResult(const bool, std::string const&);

#endif
