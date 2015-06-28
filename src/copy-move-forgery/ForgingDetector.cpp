#include "ForgingDetector.h"

#include "ImgUtils.h"
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include "Timer.h"

//#define _DEBUG_
#ifdef _DEBUG_
#define logger(token) \
    { \
        std::stringstream o; \
        o << token; \
        std::cout << o.str() << std::endl; \
    }
#else
#define logger(token)
#endif

#ifdef _DEBUG_
int ForgingDetector::dbgmsg(0);
const bool PRINT_TIME = true;
#else
const bool PRINT_TIME = false;
#endif

/* parametros pre-definidos */
const double t1 = 2.8, t2 = 0.02;      // t1 e t2
const double vectorP[CharVect::CHARS_SIZE] = {
        11.8,       // P(1)
        11.8,       // P(2)
        11.8,       // P(3)
        0.0125,     // P(4)
        0.0125,     // P(5)
        0.0125,     // P(6)
        0.0125};    // P(7)
const int MAX_SHIFT = 2;

/**
 * @func forgeringByCharact
 * @brief algoritmo de deteccao por vetor de caracteristicas
 * @param image imagem verificada
 * @param multiregion informa se multiplas regioes devem ser pesquisadas
 * @param bSize dimensao do bloco
 * @return true, foi detectada adulteracao; false, se imagem eh original
 */
bool ForgingDetector::byCharact(Bitmap const& image, int bSize)
{
    /* passo 1: extrair as caracteristicas dos blocos da imagem */
    logger("[MSG " << ++dbgmsg << "] Criando vetores de caracteristicas...");
    ListCharVect vList;
    charactVector(vList, image, bSize);
    if(!vList.size())
    {
        std::cout << "Nao foi possivel criar o vetor de caracteristicas." << std::endl;
        return false;
    }

    /* passo 2: buscar blocos similares */
    logger("[MSG " << ++dbgmsg << "] Buscando blocos similares...");
    ListSimilarBlocks simList;
    createSimilarBlockList(image, bSize, vList, simList);

    /* passo 3: */
    // Se nao ha blocos similares, a imagem nao foi adulterada por copy-move
    if(!simList.size())
        return false;

    logger("[MSG " << ++dbgmsg << "] Buscando deslocamento mais recorrente...");
    DeltaPos mainShift(getMainShiftVector(simList));

    logger("[MSG " << ++dbgmsg << "] Filtrando regioes espurias...");
    filterSpuriousRegions(simList, mainShift);

    /* passo 4: detectar adulteracao */
    logger("[MSG " << ++dbgmsg << "] Criando imagem com as areas similares...");
    Bitmap detectImage(image.getWidth(), image.getHeight());
    createImageWithSimilarAreas(detectImage, image, bSize, simList);

    logger("[MSG " << ++dbgmsg << "] Fazendo operacao de abertura na imagem...");
    detectImage = imageOpeningOperation(detectImage, bSize);

    logger("[MSG " << ++dbgmsg << "] Verificando se imagem foi alterada...");
    Bitmap forgedImage(image.getWidth(), image.getHeight());
    // salvar a imagem criada
    if(!isImageForged(image, detectImage, forgedImage))
        return false;

    logger("[MSG " << ++dbgmsg << "] Criando imagem forjada...");
    std::string path(ImgUtils::imgTrueName(image.getPath()));
    path.append(std::string("_detect.bmp"));
    ImgUtils::saveImageAs(forgedImage, path);

    logger("[MSG " << ++dbgmsg << "] Imagem forjada gravada.");

    return true;
}


/**
 * @func charactVector
 * @brief percorre a imagem em blocos e gera vetor de caracteristicas
 * @param image imagem verificada
 * @param bSize dimensao do bloco
 * @return vetor de caracteristicas

 Tipos de bloco:

 Bloco t1        Bloco t2
 _________      _________
 |    |    |    |   R1    |
 | R1 | R2 |    |_________|
 |    |    |    |         |
 |____|____|    |___R2____|

 Bloco t3        Bloco t4
 _______        _______
 |\   R1 |      | R1   /|
 |  \    |      |    /  |
 | R2 \  |      |  / R2 |
 |______\|      |/______|
 */

void ForgingDetector::charactVector(ListCharVect& listChar, Bitmap const& image, int bSize)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);

    listChar.clear();
    int width = image.getWidth();
    int height = image.getHeight();
    if(width < bSize || height < bSize)
        return;

    int bTotalX = width - bSize + 1;
    int bTotalY = height - bSize + 1;

    logger("A imagem possui " << bTotalX * bTotalY << " blocos.");

    // itera em todos os blocos
    for(int bx = 0; bx < bTotalX; bx++)
    {
        for(int by = 0; by < bTotalY; by++)
        {
            // criar vetor de caracteristicas
            CharVect charVect(bx, by);
            getCharVectListForBlock(charVect, image, bx, by, bSize);

            // adicionar o bloco lido ao conjunto de vetores de caracteristicas
            addVectLexOrder(listChar, charVect);
        }
    }
}

void ForgingDetector::charactVectorBySections(ListCharVect& listChar, Bitmap const& image, int bSize, unsigned int sections)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);

    listChar.clear();
    if(image.getWidth() < bSize || image.getHeight() < bSize)
        return;

    const int scope = (image.getHeight() - bSize) + 1;

    if(sections == 0)
        sections = 1;
    if(sections > scope)
        sections = scope;

    int sizeLast = (scope) % sections;
    int size = (scope) / sections;

    int bTotalXOrign = image.getWidth() - bSize + 1;
    int bTotalYOrign = image.getHeight() - bSize + 1;

    logger("A imagem possui " << bTotalXOrign * bTotalYOrign << " blocos.");

    for(int i=0; i<sections; i++)
    {
        Bitmap section;
        if(sizeLast!=0 && i == sections-1)
            section = image.getLines(size*i, size+sizeLast-1+bSize);
        else
            section = image.getLines(size*i, size-1+bSize);

        int bTotalX = section.getWidth() - bSize + 1;
        int bTotalY = section.getHeight() - bSize + 1;

        // itera em todos os blocos
        for(int bx = 0; bx < bTotalX; bx++)
        {
            for(int by = 0; by < bTotalY; by++)
            {
                // criar vetor de caracteristicas
                CharVect charVect(bx, by + size*i);
                getCharVectListForBlock(charVect, section, bx, by, bSize);

                // adicionar o bloco lido ao conjunto de vetores de caracteristicas
                addVectLexOrder(listChar, charVect);
            }
        }
    }
}

void ForgingDetector::getCharVectListForBlock(CharVect& charVect, Bitmap const& image, int blkPosX, int blkPosY, int blkSize)
{
    unsigned char red, green, blue, grey;
    int half = (int) blkSize / 2;
    double part[4][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

    // percorrer pixels do bloco na imagem original
    for(int x = 0; x < blkSize; x++)
    {
        for(int y = 0; y < blkSize; y++)
        {
            image.getPixel(x + blkPosX, y + blkPosY, red, green, blue);

            charVect.c[0] += (int) red;
            charVect.c[1] += (int) green;
            charVect.c[2] += (int) blue;

            // converter o pixel para escala de cinza conforme canal y
            grey = toUnsignedChar(0.299 * (int) red + 0.587 * (int) green + 0.114 * (int) blue);

            // para bloco tipo 1 | - |
            if(y < half)
                part[0][0] += grey;
            else
                part[0][1] += grey;

            // para bloco tipo 2 | | |
            if(x < half)
                part[1][0] += grey;
            else
                part[1][1] += grey;

            // para bloco tipo 3 | \ |
            if(x > y)
                part[2][0] += grey;
            else
                part[2][1] += grey;

            // para bloco tipo 4 | / |
            if(x + y < blkSize)
                part[3][0] += grey;
            else
                part[3][1] += grey;
        }
    }

    // calcular media RGB
    for(int i = 0; i < 3; i++)
        charVect.c[i] = (int) charVect.c[i] / (blkSize * blkSize);

    // soma das partes part[tipobloco][regiao]
    for(int i = 0; i < 4; i++)
        charVect.c[i + 3] = part[i][0] / (part[i][0] + part[i][1]);
}

void ForgingDetector::addVectLexOrder(ListCharVect& vecOrdered, CharVect& valToAdd)
{
    for(ListCharVect::iterator it = vecOrdered.begin(); it != vecOrdered.end(); it++)
    {
        if(valToAdd  <= (*it))
        {
            vecOrdered.insert(it, valToAdd);
            return;
        }
    }
    vecOrdered.push_back(valToAdd);
}

void ForgingDetector::createSimilarBlockList(Bitmap const& image, int bSize, ListCharVect const& vList, ListSimilarBlocks & simList)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    int width = image.getWidth();
    int height = image.getHeight();
    double diff[CharVect::CHARS_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };

    int vectOffsetSize(BASE_L);
    if(bSize + BASE_L >= width || bSize + BASE_L >= height)
        vectOffsetSize = bSize;

    // percorrer toda a lista de blocos; execucao em O(n)
    // somente sao comparados dois blocos consecutivos, pois ja estao ordenados

    CharVect prev;
    for(ListCharVect::const_iterator it = vList.begin(); it != vList.end(); prev = *(it++))
    {
        if(it == vList.begin())
            continue;

        // calcular diferencas
        bool diffVector = true;
        for(int i = 0; i < CharVect::CHARS_SIZE && diffVector; i++)
        {
            diff[i] = ABS((prev.c[i] - it->c[i]));
            diffVector = diffVector && (diff[i] < vectorP[i]);
        }

        if((diffVector)
                && (diff[0] + diff[1] + diff[2] < t1)
                && (diff[3] + diff[4] + diff[5] + diff[6] < t2)
                && ABS(getShift(prev.pos, it->pos)) > vectOffsetSize)
        {
            // blocos b1 e b2 sao similares
            simList.push_back(
                SimilarBlocks(
                    prev.pos,
                    it->pos));
        }
    }
}

bool ForgingDetector::isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift)
{
    return (ABS((current.dx - mainShift.dx)) > MAX_SHIFT || ABS((current.dy - mainShift.dy)) > MAX_SHIFT);
}

void ForgingDetector::filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    for (ListSimilarBlocks::iterator it = simList.begin(); it != simList.end();)
    {
    	if(isBlockSimilarSpurious(it->delta, mainShift))
    		it = simList.erase(it);
    	else
    		it++;
    }
}

/**
 * @func getShift
 * @brief calcula o comprimento do vetor de shift
 * @param x1 coordenada x do bloco 1
 * @param x2 coordenada x do bloco 2
 * @param y1 coordenada y do bloco 1
 * @param y2 coordenada y do bloco 2
 * @return comprimento
 */
int ForgingDetector::getShift(Pos const& pos1, Pos const& pos2)
{
    int h = pos1.x - pos2.x;
    int v = pos1.y - pos2.y;

    return (int) sqrt((h * h) + (v * v));
}

/**
 * @func getMainShiftVector
 * @brief verifica todos os pares de blocos similares e retorna o mais frequente
 *   vetor deslocamento entre eles
 * @param blocks lista de blocos
 * @return bloco que representa o principal shift
 */

DeltaPos ForgingDetector::getMainShiftVector(ListSimilarBlocks const& blocks)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    int count(0);
    DeltaPos main(0,0);
    std::map<DeltaPos, int> histograms;
    /* criar histograma de deltas */

    for(ListSimilarBlocks::const_iterator it = blocks.begin(); it!=blocks.end(); it++)
    {
        int& freq(histograms[it->delta]);
        if(++freq > count)
        {
            main = it->delta;
            count = freq;
        }
    }

    return main;
}

void ForgingDetector::createImageWithSimilarAreas(Bitmap& detectImage, Bitmap const& image, int bSize, ListSimilarBlocks const& simList)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    int width = image.getWidth();
    int height = image.getHeight();
    // criar imagem binaria com as areas similares encontradas
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
            detectImage.setPixel(i, j, 0, 0, 0);
    }

    for(ListSimilarBlocks::const_iterator it = simList.begin(); it!=simList.end(); it++)
    {
        int b1x = it->b1.x;
        int b1y = it->b1.y;
        int b2x = it->b2.x;
        int b2y = it->b2.y;

        for(int i = 0; i < bSize; i++)
        {
            for(int j = 0; j < bSize; j++)
            {
                detectImage.setPixel(i+b1x, j+b1y, 255, 255, 255);
                detectImage.setPixel(i+b2x, j+b2y, 255, 255, 255);
            }
        }
    }
}

/**
 * @func opening
 * @brief efetua operacao de abertura na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem tratada
 */
Bitmap ForgingDetector::imageOpeningOperation(Bitmap const& image, int bSize)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    /* operacao de erosao + dilatacao */
    Bitmap imageEroded(imageErosionOperation(image, bSize));
    Bitmap imageDilated(imageDilationOperation(imageEroded, bSize));
    return imageDilated;
}

/**
 * @func erosion
 * @brief efetua operacao de erosao na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem erodida
 */
Bitmap ForgingDetector::imageErosionOperation(Bitmap const& image, int bSize)
{
    Bitmap eroded(image);
    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char value, stValue;
    int origin = (int) bSize / 2 + 1;
    bool bPaint = true;
    int i, j, k, l;

    /* percorrer imagem com o elemento estruturante */
    for(i = 0; i < width; i++)
    {
        for(j = 0; j < height; j++)
        {
            image.getPixel(i, j, value, value, value);
            // pixel branco; aplicar elemento estruturante
            if(value != 0)
                continue;

            // verificar se a origem se encontra na regiao
            bPaint = true;
            for(k = i - origin; k < i + origin && k < width && bPaint; k++)
            {
                for(l = j - origin; l < j + origin && l < height && bPaint; l++)
                {
                    image.getPixel(k, l, value, value, value);
                    bPaint = bPaint & (value != 0);
                }
            }
            stValue = (bPaint ? 255 : 0);
            eroded.setPixel(i, j, stValue, stValue, stValue);
        }
    }

    return eroded;
}

/**
 * @func dilation
 * @brief efetua operacao de dilatacao na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem dilatada
 */
Bitmap ForgingDetector::imageDilationOperation(Bitmap const& image, int bSize)
{
    Bitmap dilated(image);

    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char value;
    int origin = (int) bSize / 2 + 1;
    int i, j, k, l;

    /* percorrer imagem com o elemento estruturante */
    for(i = 0; i < width; i++)
    {
        for(j = 0; j < height; j++)
        {
            //dilated.setPixel(i, j, 0, 0, 0);
            image.getPixel(i, j, value, value, value);
            // pixel branco; aplicar elemento estruturante
            if(value == 0)
                continue;

            // verificar se a origem se encontra na regiao
            for(k = i - origin; k < i + origin && k < width; k++)
            {
                for(l = j - origin; l < j + origin && l < height; l++)
                    dilated.setPixel(k, l, 255, 255, 255);
            }
        }
    }

    return dilated;
}

bool ForgingDetector::isImageForged(Bitmap const& image, Bitmap const& detectImage, Bitmap& mergedImage)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char red, green, blue, grey;
    bool bForged = false;

    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            detectImage.getPixel(i, j, grey, grey, grey);
            if(grey > 0)
            {
                mergedImage.setPixel(i, j, 0, 255, 0);
                bForged = true;
            }
            else
            {
                image.getPixel(i, j, red, green, blue);
                mergedImage.setPixel(i, j, red, green, blue);
            }
        }
    }

    return bForged;
}

