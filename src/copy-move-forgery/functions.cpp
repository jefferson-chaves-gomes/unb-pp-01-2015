
#include "functions.h"

#include <cmath>
#include <iostream>
#include "utils.h"

#define _DEBUG_

#ifdef _DEBUG_
int dbgmsg = 0;
#endif

/**
* @func printUsage
* @brief Imprime as instrucoes de utilizacao do programa
*/
void printUsage(void)
{
    std::cout << "Usage: CustomBitmap.exe <op_mode> <img_file> <param...>" << std::endl;
    std::cout << "- <op_mode>: Program operation mode." << std::endl;
    std::cout << "- <img_file>: path of bitmap file." << std::endl;
    std::cout << "- <param>: Operation mode parameters." << std::endl;
    std::cout << std::endl << "Available operation modes:" << std::endl;
    std::cout << CHARACT_VECTOR << "\tsearches image <image> for adulteration by characteristics vector" \
         << " with block size <param>. Block size defaults to 16px." << std::endl;
    std::cout << MULT_CHARACT_VECTOR << "\tsearches image <image> by characteristics vector for adulteration" \
         << " in multiple regions with block size <param>. Block size defaults to 16px." << std::endl;
    std::cout << GRAYSCALE << "\tconverts image <image> to grayscale. No <param>." << std::endl;
    std::cout << EROSION << "\tapplies erosion operation to image <image> with" \
         << " structuring element of size <param>. Block size defaults to 16px." << std::endl;
    std::cout << DILATION << "\tapplies erosion operation to image <image> with" \
         << " structuring element of size <param>. Block size defaults to 16px." << std::endl;
    std::cout << OPENING << "\tapplies erosion operation to image <image> with" \
         << " structuring element of size <param>. Block size defaults to 16px." << std::endl;
    std::cout << HELP << "\tshows usage manual." << std::endl;
}


/**
* @func toUnsignedChar
* @brief converte um valor numerico para sua representacao como pixel de uma imagem
* @param value valor numerico
* @return valor do pixel (8 bits)
*/
unsigned char toUnsignedChar(double value)
{
    if (value > 255.0)
        return 255;
    else if (value < 0.0)
        return 0;
    else
        return static_cast<unsigned char>(floor(value + 0.5));
}
