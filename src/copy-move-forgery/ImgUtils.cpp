#include "ImgUtils.h"

#include "functions.h"
#include <sstream>
#include <fstream>
#include <iostream>

/**
* @func saveImageAs
* @brief salva imagem como novo arquivo
* @param image imagem bitmap
* @param imgPath novo path da imagem
*/
void ImgUtils::saveImageAs(Bitmap image, std::string imgPath)
{
    Bitmap newImage(image);
    std::stringstream newFile;
    newFile << imgPath;
    newImage.saveImage(newFile.str());

    std::cout << "New image generated: " << newFile.str() << std::endl;
}

/**
* @func imgTrueName
* @brief recupera o path da imagem sem a extensao de arquivo
* @param file path completo
* @return nome do arquivo, sem extensao
*/
std::string ImgUtils::imgTrueName(std::string file)
{
    int aux_pos = file.find_last_of(".");
    std::string trueName(file.substr(0, aux_pos));
    return trueName;
}

/**
* @func imgGrayScale
* @brief converte uma imagem para escala de cinza
* @param image imagem original
* @param fromfile informa se a imagem foi lida de um arquivo
* @return imagem em 256 niveis de cinza
*/
Bitmap ImgUtils::imgGrayScale(Bitmap image, bool fromfile)
{
    int width = image.getWidth();
    int height = image.getHeight();
    Bitmap newImage(width, height);
    unsigned char red, green, blue, grey;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            image.getPixel(i, j, red, green, blue);
            grey = toUnsignedChar(0.299*(int)red + 0.587*(int)green + 0.114*(int)blue);
            newImage.setPixel(i, j, grey, grey, grey);
        }
    }

    // salvar a imagem criada
    if(fromfile)
    {
        std::string path;
        path.append(imgTrueName(image.getPath()));
        path.append(std::string("_greyscale.bmp"));
        saveImageAs(newImage, path);
    }

    return newImage;
}