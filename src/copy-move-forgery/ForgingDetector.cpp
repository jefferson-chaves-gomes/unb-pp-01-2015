#include "ForgingDetector.h"

#include "functions.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

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
bool ForgingDetector::byCharact(Bitmap const& image, bool multiregion, int bSize)
{
    /* passo 1: extrair as caracteristicas dos blocos da imagem */
    logger("[MSG " << ++dbgmsg << "] Criando vetores de caracteristicas...");
    CharVectList* vList = charactVector(image, bSize);
    if(vList == NULL)
    {
        std::cout << "Nao foi possivel criar o vetor de caracteristicas." << std::endl;
        return false;
    }

    /* passo 2: buscar blocos similares */
    logger("[MSG " << ++dbgmsg << "] Buscando blocos similares...");
    SimilarBlocks* simList = createSimilarBlockList(image, bSize, vList);

    /* passo 3: */
    // Se nao ha blocos similares, a imagem nao foi adulterada por copy-move
    if(simList == NULL)
        return false;

    logger("[MSG " << ++dbgmsg << "] Analisando shifts de deslocamento...");
    filterSpuriousRegions(simList, multiregion);

    /* passo 4: detectar adulteracao */
    logger("[MSG " << ++dbgmsg << "] Pesquisando adulteracao...");

    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char red, green, blue, grey;
    Bitmap forgedImage(width, height);
    Bitmap detectImage(width, height);
    // criar imagem binaria com as areas similares encontradas
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            image.getPixel(i, j, red, green, blue);
            forgedImage.setPixel(i, j, red, green, blue);
            detectImage.setPixel(i, j, 0, 0, 0);
        }
    }

    SimilarBlocks*simBlock = simList;
    while(simBlock != NULL)
    {
        int b1x = simBlock->b1.x;
        int b1y = simBlock->b1.y;
        int b2x = simBlock->b2.x;
        int b2y = simBlock->b2.y;
        for(int i = b1x; i < b1x + bSize; i++)
        {
            for(int j = b1y; j < b1y + bSize; j++)
            {
                detectImage.setPixel(i, j, 255, 255, 255);
                detectImage.setPixel(b2x, b2y++, 255, 255, 255);
            }
            b2x++;
            b2y = simBlock->b2.y;
        }
        simBlock = simBlock->next;
    }

    // operacao de abertura na imagem
    detectImage = opening(detectImage, bSize);

    bool bForged = false;
    // mergear com imagem original
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            detectImage.getPixel(i, j, grey, grey, grey);
            if(grey > 0)
            {
                forgedImage.setPixel(i, j, 0, 255, 0);
                bForged = true;
            }
        }
    }

    // salvar a imagem criada
    if(bForged == true)
    {
        std::string path;
        path.append(ImgUtils::imgTrueName(image.getPath()));
        path.append(std::string("_detect.bmp"));
        ImgUtils::saveImageAs(forgedImage, path);

        logger("[MSG " << ++dbgmsg << "] Imagem forjada gravada.");
    }
    /***/

    LinkedListCleaner::clear(vList);
    LinkedListCleaner::clear(simList);

    return bForged;
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

CharVectList* ForgingDetector::charactVector(Bitmap const& image, int bSize)
{
    int width = image.getWidth();
    int height = image.getHeight();
    if(width < bSize || height < bSize)
        return NULL;

    CharVectList* vList = NULL;
    CharVectList* charVecList = NULL;

    int bTotalX = width - bSize + 1;
    int bTotalY = height - bSize + 1;

    logger("A imagem possui " << bTotalX * bTotalY << " blocos.");

    // itera em todos os blocos
    for(int bx = 0; bx < bTotalX; bx++)
    {
        for(int by = 0; by < bTotalY; by++)
        {
            // criar vetor de caracteristicas
            charVecList = getCharVectListForBlock(image, bx, by, bSize);

            // adicionar o bloco lido ao conjunto de vetores de caracteristicas
            vList = addVectLexOrder(vList, charVecList);
        }
    }

    return vList;
}

CharVectList* ForgingDetector::getCharVectListForBlock(Bitmap const& image, int blkPosX, int blkPosY, int blkSize)
{
    int width = image.getWidth();
    int height = image.getHeight();

    if(width < blkPosX + blkSize || height < blkPosY + blkSize)
        return NULL;

    unsigned char red, green, blue, grey;
    int half = (int) blkSize / 2;
    CharVectList* charVecList = new CharVectList(blkPosX, blkPosY);
    double part[4][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

    // percorrer pixels do bloco na imagem original
    for(int x = 0; x < blkSize; x++)
    {
        for(int y = 0; y < blkSize; y++)
        {
            image.getPixel(x + blkPosX, y + blkPosY, red, green, blue);

            charVecList->vect.c[0] += (int) red;
            charVecList->vect.c[1] += (int) green;
            charVecList->vect.c[2] += (int) blue;

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
        charVecList->vect.c[i] = (int) charVecList->vect.c[i] / (blkSize * blkSize);

    // soma das partes part[tipobloco][regiao]
    for(int i = 0; i < 4; i++)
        charVecList->vect.c[i + 3] = part[i][0] / (part[i][0] + part[i][1]);

    return charVecList;
}

/**
 * @func addVectLexOrder
 * @brief adiciona aa lista um vetor de caracteristicas em ordem lexicografica
 * @param start ponteiro para o inicio da lista
 * @param vetor vetor a ser adicionado
 * @return ponteiro inicial da lista
 */

CharVectList* ForgingDetector::addVectLexOrder(CharVectList* vecOrdered, CharVectList* valToAdd)
{
    CharVectList ** head_ref = &vecOrdered;
    /* Adiciona antes da cabeca */
    if(*head_ref == NULL || valToAdd->vect <= (*head_ref)->vect)
    {
        valToAdd->next = *head_ref;
        *head_ref = valToAdd;
    }
    else
    {
        /* Adiciona entre o atual e o proximo */
        CharVectList * current = *head_ref;
        while(current->next != NULL && current->next->vect <= valToAdd->vect)
            current = current->next;
        valToAdd->next = current->next;
        current->next = valToAdd;
    }
    return *head_ref;
}

SimilarBlocks* ForgingDetector::createSimilarBlockList(Bitmap const& image, int bSize, CharVectList* vList)
{
    int width = image.getWidth();
    int height = image.getHeight();
    SimilarBlocks* simList = NULL;
    SimilarBlocks* simBlock = NULL;
    SimilarBlocks* simEnd = NULL;
    double diff[CharVect::CHARS_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };

    int vectOffsetSize(BASE_L);
    if(bSize + BASE_L >= width || bSize + BASE_L >= height)
        vectOffsetSize = bSize;

    // percorrer toda a lista de blocos; execucao em O(n)
    // somente sao comparados dois blocos consecutivos, pois ja estao ordenados
    CharVectList* iterator = vList;
    while(iterator != NULL && iterator->next != NULL)
    {
        // calcular diferencas
        bool diffVector = true;
        for(int i = 0; i < CharVect::CHARS_SIZE && diffVector; i++)
        {
            diff[i] = ABS((iterator->vect.c[i] - iterator->next->vect.c[i]));
            diffVector = diffVector && (diff[i] < vectorP[i]);
        }

        if((diffVector)
                && (diff[0] + diff[1] + diff[2] < t1)
                && (diff[3] + diff[4] + diff[5] + diff[6] < t2)
                && ABS(getShift(iterator->vect.pos, iterator->next->vect.pos)) > vectOffsetSize)
        {
            // blocos b1 e b2 sao similares
            simBlock = new SimilarBlocks(
                    iterator->vect.pos,
                    iterator->next->vect.pos);

            if(simList == NULL)
            {
                simList = simBlock;
                simEnd = simList;
            }
            else
            {
                simEnd->next = simBlock;
                simEnd = simBlock;
            }
        }

        iterator = iterator->next;
    }

    return simList;
}

void ForgingDetector::filterSpuriousRegions(SimilarBlocks* simList, bool multiregion)
{
    MaxShifts maxSh;
    DeltaPos mainShift(0,0);

    if(multiregion)
        maxSh = getMainShifts(simList);
    else
        mainShift = getMainShiftVector(simList);

    SimilarBlocks* simTrace = simList;
    SimilarBlocks* simBlock = simList;
    bool bRegions = false;

    while(simBlock != NULL)
    {
        if(multiregion)
            bRegions = isGreaterShift(simBlock, maxSh, MAX_SHIFT);
        else
            bRegions = (ABS((simBlock->delta.dx - mainShift.dx)) > MAX_SHIFT || ABS((simBlock->delta.dy - mainShift.dy)) > MAX_SHIFT);

        if(!bRegions)
            simTrace = simBlock;
        else
        {
            if(simBlock != simList)
                simTrace->next = simBlock->next;
            else
            {
                simList = simList->next;
                simTrace = simList;
            }
            delete simBlock;
        }

        simBlock = simTrace->next;
    }
}

bool ForgingDetector::isGreaterShift(SimilarBlocks* simBlock, MaxShifts const& maxSh, int maxShift)
{
    bool shift1 = (ABS((simBlock->delta.dx - maxSh.delta1.dx)) > maxShift) || (ABS((simBlock->delta.dy - maxSh.delta1.dy)) > maxShift);
    bool shift2 = (ABS((simBlock->delta.dx - maxSh.delta2.dx)) > maxShift) || (ABS((simBlock->delta.dy - maxSh.delta2.dy)) > maxShift);
    bool shift3 = (ABS((simBlock->delta.dx - maxSh.delta3.dx)) > maxShift) || (ABS((simBlock->delta.dy - maxSh.delta3.dy)) > maxShift);

    if(shift1 && shift2 && shift3)
        return true;
    return false;
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
 * @func getMainShifts
 * @brief verifica todos os pares de blocos similares e retorna os mais frequentes shifts
 *   vetor deslocamento entre eles
 * @param blocks lista de blocos
 * @return shifts mais frequentes
 */
MaxShifts ForgingDetector::getMainShifts(SimilarBlocks* blocks)
{
    int dx, dy, count = 0;
    DeltaPos main;
    SimilarBlocks* auxBlock = blocks;
    Histogram* hist = NULL;
    Histogram* hTrace = NULL;
    Histogram* hLast = NULL;
    MaxShifts maxSh;

    /* criar histograma de vetores */
    while(auxBlock != NULL)
    {
        dx = auxBlock->delta.dx;
        dy = auxBlock->delta.dy;
        if(hist == NULL)
        {
            hist = new Histogram(dx, dy);
            hLast = hist;
        }

        /* procurar por entrada no histograma */
        hTrace = hist;
        while(hTrace != NULL)
        {
            if(hTrace->delta.dx == dx && hTrace->delta.dy == dy)
                break;
            hTrace = hTrace->next;
        }
        if(hTrace == NULL)
        {
            hTrace = new Histogram(dx, dy);
            hLast->next = hTrace;
            hLast = hTrace;
        }

        /* buscar maior frequencia */
        hTrace->freq++;
        if(hTrace->freq > count)
		{
			count = hTrace->freq;
			main = hTrace->delta;

			/* atualizar frequencias */

			maxSh.delta3 = maxSh.delta2;
            maxSh.delta2 = maxSh.delta1;
            maxSh.delta1 = main;
		}

        auxBlock = auxBlock->next;
    }

    LinkedListCleaner::clear(hist);

    return maxSh;
}

/**
 * @func getMainShiftVector
 * @brief verifica todos os pares de blocos similares e retorna o mais frequente
 *   vetor deslocamento entre eles
 * @param blocks lista de blocos
 * @return bloco que representa o principal shift
 */
DeltaPos ForgingDetector::getMainShiftVector(SimilarBlocks* blocks)
{
    int dx, dy, count = 0;
    DeltaPos main(0,0);
    SimilarBlocks* auxBlock = blocks;
    Histogram* hist = NULL;
    Histogram* hTrace = NULL;
    Histogram* hLast = NULL;

    /* criar histograma de vetores */
    while(auxBlock != NULL)
    {
        dx = auxBlock->delta.dx;
        dy = auxBlock->delta.dy;
        if(hist == NULL)
        {
            hist = new Histogram(dx, dy);
            hLast = hist;
        }

        /* procurar por entrada no histograma */
        hTrace = hist;
        while(hTrace != NULL)
        {
            if(hTrace->delta.dx == dx && hTrace->delta.dy == dy)
                break;
            hTrace = hTrace->next;
        }

        if(hTrace == NULL)
        {
            hTrace = new Histogram(dx, dy);
            hLast->next = hTrace;
            hLast = hTrace;
        }

        /* buscar maior frequencia */
        hTrace->freq++;
        if(hTrace->freq > count)
		{
			count = hTrace->freq;
			main = hTrace->delta;
		}

        auxBlock = auxBlock->next;
    }

    LinkedListCleaner::clear(hist);

    return main;
}

/**
 * @func opening
 * @brief efetua operacao de abertura na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem tratada
 */
Bitmap ForgingDetector::opening(Bitmap const& image, int bSize)
{
    /* operacao de erosao + dilatacao */
    return dilation(erosion(image, bSize), bSize);
}

/**
 * @func erosion
 * @brief efetua operacao de erosao na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem erodida
 */
Bitmap ForgingDetector::erosion(Bitmap const& image, int bSize)
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
            if(value == 0)
            {
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
Bitmap ForgingDetector::dilation(Bitmap const& image, int bSize)
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
            if(value != 0)
            {
                // verificar se a origem se encontra na regiao
                for(k = i - origin; k < i + origin && k < width; k++)
                {
                    for(l = j - origin; l < j + origin && l < height; l++)
                        dilated.setPixel(k, l, 255, 255, 255);
                }
            }
        }
    }

    return dilated;
}

