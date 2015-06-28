#ifndef MAIN_H_
#define MAIN_H_

#include <string>

void printUsage();
void startSerialProcess(int, char*[]);
void startOpenMPProcess(int, char*[]);
void printResult(const bool, const std::string);

#endif
