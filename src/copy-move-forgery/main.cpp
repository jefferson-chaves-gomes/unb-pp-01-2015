
#include "functions.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printUsage();
        exit(0);
    }

    if(std::string(argv[1]) != CHARACT_VECTOR &&
       std::string(argv[1]) != MULT_CHARACT_VECTOR &&
       std::string(argv[1]) != EROSION &&
       std::string(argv[1]) != DILATION &&
       std::string(argv[1]) != OPENING &&
       std::string(argv[1]) != GRAYSCALE)
    {
        printUsage();
        exit(0);
    }

    /* escolha de modo de operacao */
    if(std::string(argv[1]) == CHARACT_VECTOR || std::string(argv[1]) == MULT_CHARACT_VECTOR)
    {
        int bSize = 16;
        int multi = false;
        bool tampered = false;
        if(std::string(argv[1]) == MULT_CHARACT_VECTOR)
            multi = true;
        if(argc == 4)
            bSize = atoi(argv[3]);
        tampered = forgeringByCharact(Bitmap(argv[2]), multi, bSize);
        
        if(tampered)
            std::cout << "Tampering was detected in image '" << argv[2] << "'." << std::endl;
        else
            std::cout << "Image '" << argv[2] << "' is assumed to be authentic." << std::endl;
        
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == GRAYSCALE)
    {
        imgGrayScale(Bitmap(argv[2]), true);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == EROSION)
    {
        Bitmap eroded = erosion(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(imgTrueName(argv[2]));
        path.append(std::string("_eroded.bmp"));
        saveImageAs(eroded, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == DILATION)
    {
        Bitmap dilated = dilation(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(imgTrueName(argv[2]));
        path.append(std::string("_dilated.bmp"));
        saveImageAs(dilated, path);
        std::cout << "Done." << std::endl;
    }
    else if(std::string(argv[1]) == OPENING)
    {
        Bitmap opened = opening(Bitmap(argv[2]), atoi(argv[3]));
        std::string path;
        path.append(imgTrueName(argv[2]));
        path.append(std::string("_opened.bmp"));
        saveImageAs(opened, path);
        std::cout << "Done." << std::endl;
    }
    else
        printUsage();

    return 0;
}

