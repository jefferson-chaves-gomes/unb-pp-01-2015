#include "ForgingDetector.h"

#include "functions.h"
#include "ImgUtils.h"
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

#ifdef _DEBUG_
int dbgmsg = 0;
#endif

/* parametros pre-definidos */
const double t1 = 2.8, t2 = 0.02;      // t1 e t2
const double vectorP[CHARS_SIZE] = {
        11.8,       // P(1)
        11.8,       // P(2)
        11.8,       // P(3)
        0.0125,     // P(4)
        0.0125,     // P(5)
        0.0125,     // P(6)
        0.0125};    // P(7)
const int maxShift = 2;

/**
 * @func forgeringByCharact
 * @brief algoritmo de deteccao por vetor de caracteristicas
 * @param image imagem verificada
 * @param multiregion informa se multiplas regioes devem ser pesquisadas
 * @param bSize dimensao do bloco
 * @return true, foi detectada adulteracao; false, se imagem eh original
 */
bool ForgingDetector::byCharact(Bitmap image, bool multiregion, int bSize)
{
    int L;                           // comprimento do vetor deslocamento

    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char red, green, blue, grey;
    Bitmap forgedImage(width, height);
    Bitmap detectImage(width, height);
    bool bForged = false;

    CharVectList* vList = NULL;
    SimilarBlocks* simList = NULL;
    SimilarBlocks* simEnd = NULL;
    SimilarBlocks* simBlock = NULL;
    int iCount = 0;

    /* passo 1: extrair as caracteristicas dos blocos da imagem */

#ifdef _DEBUG_
    std::cout << "[MSG " << ++dbgmsg << "] Criando vetores de caracteristicas..." << std::endl;
#endif
    vList = charactVector(image, bSize);
    if(vList == NULL)
    {
        std::cout << "Nao foi possivel criar o vetor de caracteristicas." << std::endl;
        return false;
    }

    /* passo 2: buscar blocos similares */

#ifdef _DEBUG_
    std::cout << "[MSG " << ++dbgmsg << "] Buscando blocos similares..." << std::endl;
#endif

    CharVectList* b1Vector = vList;
    CharVectList* b2Vector = NULL;
    double diff[CHARS_SIZE] = {
            0,
            0,
            0,
            0,
            0,
            0,
            0};

    // percorrer toda a lista de blocos; execucao em O(n)
    // somente sao comparados dois blocos consecutivos, pois ja estao ordenados
    if(bSize + BASE_L >= width || bSize + BASE_L >= height)
        L = bSize;
    else
        L = BASE_L;
    while(b1Vector != NULL)
    {
        iCount++;
        b2Vector = b1Vector->next;
        if(b2Vector != NULL)
        {
            // calcular diferencas

            bool diffVector = true;

            for(int i = 0; i < CHARS_SIZE && diffVector; i++)
            {
                diff[i] = ABS((b1Vector->vect.c[i] - b2Vector->vect.c[i]));
                diffVector = diffVector && (diff[i] < vectorP[i]);
            }

            if(diffVector && (diff[0] + diff[1] + diff[2] < t1) && (diff[3] + diff[4] + diff[5] + diff[6] < t2))
            {
                if(ABS(getShift(b1Vector->vect.x, b2Vector->vect.x, b1Vector->vect.y, b2Vector->vect.y)) > L)
                {
                    // nao e necessario
//                    bool equal = (diff1 == diff2 == diff3 == diff4
//                                   == diff5 == diff6 == diff7 == 0);
                    bool equal = false;

                    // blocos b1 e b2 sao similares
                    simBlock = newSimilarBlock(b1Vector->vect.x, b2Vector->vect.x, b1Vector->vect.y, b2Vector->vect.y,
                            equal);
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
            }
        }
        else
            break;

        b1Vector = b1Vector->next;
    }

    /***********/

#ifdef _DEBUG_
    std::cout << "[MSG " << ++dbgmsg << "] Lista de " << iCount << " blocos foi percorrida." << std::endl;
#endif

    /* passo 3: */
    // Se nao ha blocos similares, a imagem nao foi adulterada por copy-move
    if(simList == NULL)
        return false;

#ifdef _DEBUG_
    std::cout << "[MSG " << ++dbgmsg << "] Analisando shifts de deslocamento..." << std::endl;
#endif

    // buscar principal vetor deslocamento e eliminar regioes espurias
    //*
    MaxShifts maxSh;
    SimilarBlocks* mainShift;
    SimilarBlocks* simTrace = simList;
    simBlock = simList;
    bool bRegions = false;

    if(multiregion)
        maxSh = getMainShifts(simList);
    else
        mainShift = getMainShiftVector(simList);

    while(simBlock != NULL)
    {
        if(multiregion)
            bRegions = isGreaterShift(simBlock, maxSh, maxShift);
        else
            bRegions = ((ABS((simBlock->dx - mainShift->dx)) > maxShift
                    || ABS((simBlock->dy - mainShift->dy)) > maxShift) && simBlock->equal == false);
        if(bRegions == true)
        {
            if(simBlock == simList)
            {
                simList = simList->next;
                simTrace = simList;
            }
            else
                simTrace->next = simBlock->next;
            delete simBlock;
            simBlock = simTrace->next;
        }
        else
        {
            simTrace = simBlock;
            simBlock = simBlock->next;
        }
    }
    // */

    /* passo 4: detectar adulteracao */
#ifdef _DEBUG_
    std::cout << "[MSG " << ++dbgmsg << "] Pesquisando adulteracao..." << std::endl;
#endif

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
    simBlock = simList;
    while(simBlock != NULL)
    {
        int b1x = simBlock->b1x;
        int b1y = simBlock->b2x;
        int b2x = simBlock->b1y;
        int b2y = simBlock->b2y;
        for(int i = b1x; i < b1x + bSize; i++)
        {
            for(int j = b1y; j < b1y + bSize; j++)
            {
                detectImage.setPixel(i, j, 255, 255, 255);
                detectImage.setPixel(b2x, b2y++, 255, 255, 255);
            }
            b2x++;
            b2y = simBlock->b2y;
        }
        simBlock = simBlock->next;
    }

    // operacao de abertura na imagem
    detectImage = opening(detectImage, bSize);

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

#ifdef _DEBUG_
        std::cout << "[MSG " << ++dbgmsg << "] Imagem forjada gravada." << std::endl;
#endif
    }
    /***/

    clearCharVectors(vList);
    clearSimilarBlocks(simList);

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
CharVectList* ForgingDetector::charactVector(Bitmap image, int bSize)
{
    int width = image.getWidth();
    int height = image.getHeight();
    Bitmap block(bSize, bSize);
    unsigned char red, green, blue, grey;

    CharVectList* vList = NULL;
    CharVectList* vetor = NULL;
    double part[4][2];    // soma das partes part[tipobloco][regiao]
    int dx = 0, dy = 0;
    int half = (int) bSize / 2;
    ;
    int dd = BLOCKSHIFT;

    bool loop = true;

    int iCount = 0;
    while(loop)
    {
        // inicializar
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 2; j++)
                part[i][j] = 0.0;

        // criar vetor de caracteristicas
        vetor = new CharVectList;
        vetor->next = NULL;
        if(!vetor)
        {
            clearCharVectors(vList);
            return NULL;
        }

        // percorrer bloco da imagem original
        vetor->vect.x = dx;
        vetor->vect.y = dy;
        for(int i = dx; i < dx + bSize && i < width; i++)
        {
            for(int j = dy; j < dy + bSize && j < height; j++)
            {
                image.getPixel(i, j, red, green, blue);

                vetor->vect.c[0] += (int) red;
                vetor->vect.c[1] += (int) green;
                vetor->vect.c[2] += (int) blue;

                // converter o pixel para escala de cinza conforme canal y
                grey = toUnsignedChar(0.299 * (int) red + 0.587 * (int) green + 0.114 * (int) blue);
                block.setPixel(i - dx, j - dy, grey, grey, grey);
            }
        }

        // calcular media RGB
        for(int i = 0; i < 3; i++)
            vetor->vect.c[i] = (int) vetor->vect.c[i] / (bSize * bSize);

        // percorrer bloco no canal Y
        for(int i = 0; i < bSize; i++)
        {
            for(int j = 0; j < bSize; j++)
            {
                block.getPixel(i, j, grey, grey, grey);

                // para cada tipo de bloco, identificar se o pixel estah em R1 ou R2

                // para bloco tipo 1 | - |
                if(j < half)
                    part[0][0] += grey;
                else
                    part[0][1] += grey;

                // para bloco tipo 2 | | |
                if(i < half)
                    part[1][0] += grey;
                else
                    part[1][1] += grey;

                // para bloco tipo 3 | \ |
                if(i > j)
                    part[2][0] += grey;
                else
                    part[2][1] += grey;

                // para bloco tipo 4 | / |
                if(i + j < bSize)
                    part[3][0] += grey;
                else
                    part[3][1] += grey;
            }
        }

        for(int i = 0; i < 4; i++)
            vetor->vect.c[i + 3] = part[i][0] / (part[i][0] + part[i][1]);

        // adicionar o bloco lido ao conjunto de vetores de caracteristicas
        if(vList == NULL)
            vList = vetor;
        else
            vList = addVectLexOrder(vList, vetor);

        dx += dd;
        if(width < dx + bSize)
        {
            dx = 0;
            dy += dd;
            if(height < dy + bSize)
                loop = false;
        }
        iCount++;
    }

#ifdef _DEBUG_
    std::cout << "A imagem possui " << iCount << " blocos." << std::endl;
#endif

    return vList;
}

CharVectList* ForgingDetector::charactVectorNew(Bitmap const& image, int bSize)
{
    int width = image.getWidth();
    int height = image.getHeight();
    Bitmap block(bSize, bSize);
    unsigned char red, green, blue, grey;

    CharVectList* vList = NULL;
    CharVectList* charVecList = NULL;
    int dx = 0, dy = 0;
    int half = (int) bSize / 2;
    int dd = BLOCKSHIFT;

    bool loop = true;

    int iCount = 0;
    while(loop)
    {
        // inicializar
        double part[4][2] = {
                0,0,0,0,
                0,0,0,0};    // soma das partes part[tipobloco][regiao]

        // criar vetor de caracteristicas
        charVecList = new CharVectList(dx, dy);

        // percorrer bloco da imagem original
        for(int i = dx; i < dx + bSize && i < width; i++)
        {
            for(int j = dy; j < dy + bSize && j < height; j++)
            {
                image.getPixel(i, j, red, green, blue);

                charVecList->vect.c[0] += (int) red;
                charVecList->vect.c[1] += (int) green;
                charVecList->vect.c[2] += (int) blue;

                // converter o pixel para escala de cinza conforme canal y
                grey = toUnsignedChar(0.299 * (int) red + 0.587 * (int) green + 0.114 * (int) blue);
                block.setPixel(i - dx, j - dy, grey, grey, grey);
            }
        }

        // calcular media RGB
        for(int i = 0; i < 3; i++)
            charVecList->vect.c[i] = (int) charVecList->vect.c[i] / (bSize * bSize);

        // percorrer bloco no canal Y
        for(int i = 0; i < bSize; i++)
        {
            for(int j = 0; j < bSize; j++)
            {
                block.getPixel(i, j, grey, grey, grey);

                // para cada tipo de bloco, identificar se o pixel estah em R1 ou R2

                // para bloco tipo 1 | - |
                if(j < half)
                    part[0][0] += grey;
                else
                    part[0][1] += grey;

                // para bloco tipo 2 | | |
                if(i < half)
                    part[1][0] += grey;
                else
                    part[1][1] += grey;

                // para bloco tipo 3 | \ |
                if(i > j)
                    part[2][0] += grey;
                else
                    part[2][1] += grey;

                // para bloco tipo 4 | / |
                if(i + j < bSize)
                    part[3][0] += grey;
                else
                    part[3][1] += grey;
            }
        }

        for(int i = 0; i < 4; i++)
            charVecList->vect.c[i + 3] = part[i][0] / (part[i][0] + part[i][1]);

        // adicionar o bloco lido ao conjunto de vetores de caracteristicas
        if(vList == NULL)
            vList = charVecList;
        else
            vList = addVectLexOrder(vList, charVecList);

        dx += dd;
        if(width < dx + bSize)
        {
            dx = 0;
            dy += dd;
            if(height < dy + bSize)
                loop = false;
        }
        iCount++;
    }

#ifdef _DEBUG_
    std::cout << "A imagem possui " << iCount << " blocos." << std::endl;
#endif

    return vList;
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
int ForgingDetector::getShift(int x1, int x2, int y1, int y2)
{
    int h = x1 - x2;
    int v = y1 - y2;

    return (int) sqrt((h * h) + (v * v));
}

/**
 * @func newSimilarBlock
 * @brief aloca memoria para novo bloco similar
 * @param x1 coordenada x do bloco 1
 * @param x2 coordenada x do bloco 2
 * @param y1 coordenada y do bloco 1
 * @param y2 coordenada y do bloco 2
 * @return bloco criado
 */
SimilarBlocks* ForgingDetector::newSimilarBlock(int x1, int x2, int y1, int y2, bool equal)
{
    SimilarBlocks* block = new SimilarBlocks;

    block->b1x = x1;
    block->b1y = x2;
    block->b2x = y1;
    block->b2y = y2;
    block->dx = x1 - x2;
    block->dy = y1 - y2;
    block->equal = equal;
    block->next = NULL;

    return block;
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
    SimilarBlocks* main = NULL;
    SimilarBlocks* auxBlock = blocks;
    Histogram* hist = NULL;
    Histogram* hTrace = NULL;
    Histogram* hLast = NULL;
    MaxShifts maxSh;

    /* criar histograma de vetores */
    while(auxBlock != NULL)
    {
        dx = auxBlock->dx;
        dy = auxBlock->dy;
        if(hist == NULL)
        {
            hist = newHistogram(dx, dy, auxBlock);
            hLast = hist;
        }

        /* procurar por entrada no histograma */
        hTrace = hist;
        while(hTrace != NULL)
        {
            if(hTrace->dx == dx && hTrace->dy == dy)
                break;
            hTrace = hTrace->next;
        }
        if(hTrace == NULL)
        {
            hTrace = newHistogram(dx, dy, auxBlock);
            hLast->next = hTrace;
            hLast = hTrace;
        }
        hTrace->freq++;

        auxBlock = auxBlock->next;
    }

    /* buscar maior frequencia */
    hTrace = hist;
    while(hTrace != NULL)
    {
        if(hTrace->freq > count)
        {
            count = hTrace->freq;
            main = hTrace->rep;

            /* atualizar frequencias */
            maxSh.dx3 = maxSh.dx2;
            maxSh.dx2 = maxSh.dx1;
            maxSh.dx1 = main->dx;

            maxSh.dy3 = maxSh.dy2;
            maxSh.dy2 = maxSh.dy1;
            maxSh.dy1 = main->dy;
        }
        hTrace = hTrace->next;
    }

    clearHistogram(hist);

    return maxSh;
}

/**
 * @func getMainShiftVector
 * @brief verifica todos os pares de blocos similares e retorna o mais frequente
 *   vetor deslocamento entre eles
 * @param blocks lista de blocos
 * @return bloco que representa o principal shift
 */
SimilarBlocks* ForgingDetector::getMainShiftVector(SimilarBlocks* blocks)
{
    int dx, dy, count = 0;
    SimilarBlocks* main = NULL;
    SimilarBlocks* auxBlock = blocks;
    Histogram* hist = NULL;
    Histogram* hTrace = NULL;
    Histogram* hLast = NULL;

    /* criar histograma de vetores */
    while(auxBlock != NULL)
    {
        dx = auxBlock->dx;
        dy = auxBlock->dy;
        if(hist == NULL)
        {
            hist = newHistogram(dx, dy, auxBlock);
            hLast = hist;
        }

        /* procurar por entrada no histograma */
        hTrace = hist;
        while(hTrace != NULL)
        {
            if(hTrace->dx == dx && hTrace->dy == dy)
                break;
            hTrace = hTrace->next;
        }
        if(hTrace == NULL)
        {
            hTrace = newHistogram(dx, dy, auxBlock);
            hLast->next = hTrace;
            hLast = hTrace;
        }
        hTrace->freq++;

        auxBlock = auxBlock->next;
    }

    /* buscar maior frequencia */
    hTrace = hist;
    while(hTrace != NULL)
    {
        if(hTrace->freq > count)
        {
            count = hTrace->freq;
            main = hTrace->rep;
        }
        hTrace = hTrace->next;
    }

    clearHistogram(hist);

    return main;
}

bool ForgingDetector::isGreaterShift(SimilarBlocks* simBlock, MaxShifts maxSh, int maxShift)
{
    if((ABS((simBlock->dx - maxSh.dx1)) > maxShift || ABS((simBlock->dy - maxSh.dy1)) > maxShift)
            && (ABS((simBlock->dx - maxSh.dx2)) > maxShift || ABS((simBlock->dy - maxSh.dy2)) > maxShift)
            && (ABS((simBlock->dx - maxSh.dx3)) > maxShift || ABS((simBlock->dy - maxSh.dy3)) > maxShift))
        return true;
    return false;
}

/**
 * @func opening
 * @brief efetua operacao de abertura na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem tratada
 */
Bitmap ForgingDetector::opening(Bitmap image, int bSize)
{
    /* operacao de erosao + dilatacao */
    return dilation(erosion(image, bSize), bSize);
}

/**
 * @func clearVectors
 * @brief limpa a lista de vetores de caracteristicas
 * @param start ponteiro para o inicio da lista
 */
void ForgingDetector::clearCharVectors(CharVectList* start)
{
    CharVectList* aux = start;

    while(start != NULL)
    {
        aux = start->next;
        delete start;
        start = aux;
    }
}

/**
 * @func clearSimilarVectors
 * @brief limpa a lista de vetores de caracteristicas
 * @param start ponteiro para o inicio da lista
 */
void ForgingDetector::clearSimilarBlocks(SimilarBlocks* start)
{
    SimilarBlocks* aux = start;
    while(start != NULL)
    {
        aux = start->next;
        delete start;
        start = aux;
    }
}

/**
 * @func addVectLexOrder
 * @brief adiciona aa lista um vetor de caracteristicas em ordem lexicografica
 * @param start ponteiro para o inicio da lista
 * @param vetor vetor a ser adicionado
 * @return ponteiro inicial da lista
 */
CharVectList* ForgingDetector::addVectLexOrder(CharVectList* start, CharVectList* vetor)
{
    if(start == NULL)
        return vetor;

    CharVectList* aux = start;
    CharVectList* trace = start;
    bool greater = false;
    bool smaller = false;

    while(aux != NULL)
    {
        greater = false;
        smaller = false;

        for(int i = 0; i < CHARS_SIZE; i++)
        {
            if(vetor->vect.c[i] == aux->vect.c[i])
                continue;
            else if(vetor->vect.c[i] > aux->vect.c[i])
                greater = true;
            else
                smaller = true;

            break;
        }

        if(greater == true)
        {
            trace = aux;
            aux = aux->next;
            continue;
        }
        else if(smaller == true)
        {
            vetor->next = aux;
            if(aux == start)
                start = vetor;
            else
                trace->next = vetor;
        }
        else
        {
            vetor->next = aux->next;
            aux->next = vetor;
        }
        break;
    }

    if(greater == true)
        trace->next = vetor;

    return start;
}

/**
 * @func newHistogram
 * @brief cria uma nova entrada para o histograma
 * @param dx deslocamento horizontal
 * @param dy deslocamento vertical
 * @param rep representante do vetor
 * @return elemento do histograma
 */
Histogram* ForgingDetector::newHistogram(int dx, int dy, SimilarBlocks* rep)
{
    Histogram* hist = new Histogram;

    hist->dx = dx;
    hist->dy = dy;
    hist->rep = rep;
    hist->freq = 0;
    hist->next = NULL;

    return hist;
}

/**
 * @func clearHistogram
 * @brief limpa o histograma
 * @param start ponteiro para o inicio da lista
 */
void ForgingDetector::clearHistogram(Histogram* start)
{
    Histogram* aux = start;
    while(start != NULL)
    {
        aux = start->next;
        delete start;
        start = aux;
    }
}

/**
 * @func dilation
 * @brief efetua operacao de dilatacao na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem dilatada
 */
Bitmap ForgingDetector::dilation(Bitmap image, int bSize)
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

/**
 * @func erosion
 * @brief efetua operacao de erosao na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem erodida
 */
Bitmap ForgingDetector::erosion(Bitmap image, int bSize)
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
