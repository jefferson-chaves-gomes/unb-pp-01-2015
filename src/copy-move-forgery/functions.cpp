
#include "functions.h"

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>

#define _DEBUG_

#ifdef _DEBUG_
int dbgmsg = 0;
#endif

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


/**
* @func imgTrueName
* @brief recupera o path da imagem sem a extensao de arquivo
* @param file path completo
* @return nome do arquivo, sem extensao
*/
std::string imgTrueName(std::string file)
{
    int aux_pos = file.find_last_of(".");
    std::string trueName(file.substr(0, aux_pos));
    return trueName;
}

/**
* @func saveImageAs
* @brief salva imagem como novo arquivo
* @param image imagem bitmap
* @param imgPath novo path da imagem
*/
void saveImageAs(Bitmap image, std::string imgPath)
{
    Bitmap newImage(image);
    std::stringstream newFile;
    newFile << imgPath;
    newImage.saveImage(newFile.str());

    std::cout << "New image generated: " << newFile.str() << std::endl;
}

/**
* @func randAlpha
* @brief obtem numero randomico dentro de um intervalo especificado
* @param range magnitude do intervalo (-range <= val <= range)
* @return numero pseudoaleatorio
*/
int randAlpha(int range)
{
    int got = 0;
    int val;

    got = std::rand();
    val = got % (range*2 + 1);
    val -= range;

    return val;
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
int getShift(int x1, int x2, int y1, int y2)
{
    int h = x1 - x2;
    int v = y1 - y2;

    return (int)sqrt((h*h) + (v*v));
}

bool isGreaterShift(SimilarBlocks* simBlock, MaxShifts maxSh, int maxShift)
{
    if((ABS((simBlock->dx - maxSh.dx1)) > maxShift || ABS((simBlock->dy - maxSh.dy1)) > maxShift)
      && (ABS((simBlock->dx - maxSh.dx2)) > maxShift || ABS((simBlock->dy - maxSh.dy2)) > maxShift)
      && (ABS((simBlock->dx - maxSh.dx3)) > maxShift || ABS((simBlock->dy - maxSh.dy3)) > maxShift))
      return true;
    return false;
}

/**
* @func imgGrayScale
* @brief converte uma imagem para escala de cinza
* @param image imagem original
* @param fromfile informa se a imagem foi lida de um arquivo
* @return imagem em 256 niveis de cinza
*/
Bitmap imgGrayScale(Bitmap image, bool fromfile)
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

/******************************************************************************/
/*************************   Metodos de deteccao   ****************************/
/******************************************************************************/

/**
* @func newCharVect
* @brief aloca memoria para novo vetor de caracteristicas
* @return vetor criado
*/
CharVectList* newCharVect()
{
    CharVectList* vetor = new CharVectList;

    vetor->vect.c1 = 0;
    vetor->vect.c2 = 0;
    vetor->vect.c3 = 0;
    vetor->vect.c4 = 0.0;
    vetor->vect.c5 = 0.0;
    vetor->vect.c6 = 0.0;
    vetor->vect.c7 = 0.0;
    vetor->next = NULL;

    return vetor;
}

/**
* @func clearVectors
* @brief limpa a lista de vetores de caracteristicas
* @param start ponteiro para o inicio da lista
*/
void clearCharVectors(CharVectList* start)
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
* @func addVectLexOrder
* @brief adiciona aa lista um vetor de caracteristicas em ordem lexicografica
* @param start ponteiro para o inicio da lista
* @param vetor vetor a ser adicionado
* @return ponteiro inicial da lista
*/
CharVectList* addVectLexOrder(CharVectList* start, CharVectList* vetor)
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

        if(vetor->vect.c1 > aux->vect.c1)
            greater = true;
        else if(vetor->vect.c1 == aux->vect.c1)
        {
            if(vetor->vect.c2 > aux->vect.c2)
                greater = true;
            else if(vetor->vect.c2 == aux->vect.c2)
            {
                if(vetor->vect.c3 > aux->vect.c3)
                    greater = true;
                else if(vetor->vect.c3 == aux->vect.c3)
                {
                    if(vetor->vect.c4 > aux->vect.c4)
                        greater = true;
                    else if(vetor->vect.c4 == aux->vect.c4)
                    {
                        if(vetor->vect.c5 > aux->vect.c5)
                            greater = true;
                        else if(vetor->vect.c5 == aux->vect.c5)
                        {
                            if(vetor->vect.c6 > aux->vect.c6)
                                greater = true;
                            else if(vetor->vect.c6 == aux->vect.c6)
                            {
                                if(vetor->vect.c7 > aux->vect.c7)
                                    greater = true;
                                else if(vetor->vect.c7 < aux->vect.c7)
                                    smaller = true;
                            }
                            else
                                smaller = true;
                        }
                        else
                            smaller = true;
                    }
                    else
                        smaller = true;
                }
                else
                    smaller = true;
            }
            else
                smaller = true;
        }
        else
            smaller = true;

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
* @func newSimilarBlock
* @brief aloca memoria para novo bloco similar
* @param x1 coordenada x do bloco 1
* @param x2 coordenada x do bloco 2
* @param y1 coordenada y do bloco 1
* @param y2 coordenada y do bloco 2
* @return bloco criado
*/
SimilarBlocks* newSimilarBlock(int x1, int x2, int y1, int y2, bool equal)
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
* @func getMainShiftVector
* @brief verifica todos os pares de blocos similares e retorna o mais frequente
*   vetor deslocamento entre eles
* @param blocks lista de blocos
* @return bloco que representa o principal shift
*/
SimilarBlocks* getMainShiftVector(SimilarBlocks* blocks)
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

/**
* @func getMainShifts
* @brief verifica todos os pares de blocos similares e retorna os mais frequentes shifts
*   vetor deslocamento entre eles
* @param blocks lista de blocos
* @return shifts mais frequentes
*/
MaxShifts getMainShifts(SimilarBlocks* blocks)
{
    int dx, dy, count = 0;
    SimilarBlocks* main = NULL;
    SimilarBlocks* auxBlock = blocks;
    Histogram* hist = NULL;
    Histogram* hTrace = NULL;
    Histogram* hLast = NULL;
    MaxShifts maxSh;
    int state = 0;

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
* @func clearSimilarVectors
* @brief limpa a lista de vetores de caracteristicas
* @param start ponteiro para o inicio da lista
*/
void clearSimilarBlocks(SimilarBlocks* start)
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
* @func newHistogram
* @brief cria uma nova entrada para o histograma
* @param dx deslocamento horizontal
* @param dy deslocamento vertical
* @param rep representante do vetor
* @return elemento do histograma
*/
Histogram* newHistogram(int dx, int dy, SimilarBlocks* rep)
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
void clearHistogram(Histogram* start)
{
    Histogram* aux = start;
    while(start != NULL)
    {
        aux = start->next;
        delete start;
        start = aux;
    }
}

/********************************/

/**
* @func forgeringByCharact
* @brief algoritmo de deteccao por vetor de caracteristicas
* @param image imagem verificada
* @param multiregion informa se multiplas regioes devem ser pesquisadas
* @param bSize dimensao do bloco
* @return true, foi detectada adulteracao; false, se imagem eh original
*/
bool forgeringByCharact(Bitmap image, bool multiregion, int bSize)
{
    /* parametros pre-definidos */
    const double alpha = 0.5;              // 0.45 <= alpha < 1
    const double Tr = 12.5/100;            // 12.5%
    const double t1 = 2.8, t2 = 0.02;      // t1 e t2
    const double vectorP[7] = {11.8,       // P(1)
                         11.8,        // P(2)
                         11.8,        // P(3)
                         0.0125,     // P(4)
                         0.0125,     // P(5)
                         0.0125,     // P(6)
                         0.0125};    // P(7)
    const int maxShift = 2;
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
    int dx = 0, dy = 0;
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
    double diff1, diff2, diff3, diff4, diff5, diff6, diff7;

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
            diff1 = ABS((b1Vector->vect.c1 - b2Vector->vect.c1));
            diff2 = ABS((b1Vector->vect.c2 - b2Vector->vect.c2));
            diff3 = ABS((b1Vector->vect.c3 - b2Vector->vect.c3));
            diff4 = ABS((b1Vector->vect.c4 - b2Vector->vect.c4));
            diff5 = ABS((b1Vector->vect.c5 - b2Vector->vect.c5));
            diff6 = ABS((b1Vector->vect.c6 - b2Vector->vect.c6));
            diff7 = ABS((b1Vector->vect.c7 - b2Vector->vect.c7));

            if((diff1 < vectorP[0]) &&
                (diff2 < vectorP[1]) &&
                (diff3 < vectorP[2]) &&
                (diff4 < vectorP[3]) &&
                (diff5 < vectorP[4]) &&
                (diff6 < vectorP[5]) &&
                (diff7 < vectorP[6]) &&
                (diff1 + diff2 + diff3 < t1) &&
                (diff4 + diff5 + diff6 + diff7 < t2))
            {
                if(ABS(getShift(b1Vector->vect.x, b2Vector->vect.x,
                      b1Vector->vect.y, b2Vector->vect.y)) > L)
                {
                    bool equal = (diff1 == diff2 == diff3 == diff4
                                   == diff5 == diff6 == diff7 == 0);
                    equal = false;

                    // blocos b1 e b2 sao similares
                    simBlock = newSimilarBlock(b1Vector->vect.x,
                                               b2Vector->vect.x,
                                               b1Vector->vect.y,
                                               b2Vector->vect.y,
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
            bRegions = ((ABS((simBlock->dx - mainShift->dx)) > maxShift ||
                         ABS((simBlock->dy - mainShift->dy)) > maxShift) &&
                         simBlock->equal == false);
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
        path.append(imgTrueName(image.getPath()));
        path.append(std::string("_detect.bmp"));
        saveImageAs(forgedImage, path);

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
CharVectList* charactVector(Bitmap image, int bSize)
{
    int width = image.getWidth();
    int height = image.getHeight();
    Bitmap block(bSize, bSize);
    unsigned char red, green, blue, grey;

    CharVectList* vList = NULL;
    CharVectList* vetor = NULL;
    double part[4][2]; // soma das partes part[tipobloco][regiao]
    int dx = 0, dy = 0;
    int half = (int)bSize/2;;
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
        vetor = newCharVect();
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

                vetor->vect.c1 += (int)red;
                vetor->vect.c2 += (int)green;
                vetor->vect.c3 += (int)blue;

                // converter o pixel para escala de cinza conforme canal y
                grey = toUnsignedChar(0.299*(int)red + 0.587*(int)green + 0.114*(int)blue);
                block.setPixel(i-dx, j-dy, grey, grey, grey);
            }
        }

        // calcular media RGB
        vetor->vect.c1 = (int)vetor->vect.c1/(bSize*bSize);
        vetor->vect.c2 = (int)vetor->vect.c2/(bSize*bSize);
        vetor->vect.c3 = (int)vetor->vect.c3/(bSize*bSize);

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

        vetor->vect.c4 = part[0][0]/(part[0][0] + part[0][1]);
        vetor->vect.c5 = part[1][0]/(part[1][0] + part[1][1]);
        vetor->vect.c6 = part[2][0]/(part[2][0] + part[2][1]);
        vetor->vect.c7 = part[3][0]/(part[3][0] + part[3][1]);

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
            if(height < dy+bSize)
                loop = false;
        }
        iCount++;
    }

#ifdef _DEBUG_
    std::cout << "A imagem possui " << iCount << " blocos." << std::endl;
#endif

    return vList;
}


/******************************************************************************/
/*** Operacoes morfologicas ***/
/******************************************************************************/

/**
* @func erosion
* @brief efetua operacao de erosao na imagem
* @param image imagem binaria
* @param bSize dimensao do elemento estruturante, que eh um quadrado
* @return imagem erodida
*/
Bitmap erosion(Bitmap image, int bSize)
{
    Bitmap eroded(image);
    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char value, stValue;
    int origin = (int)bSize/2 + 1;
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

/**
* @func dilation
* @brief efetua operacao de dilatacao na imagem
* @param image imagem binaria
* @param bSize dimensao do elemento estruturante, que eh um quadrado
* @return imagem dilatada
*/
Bitmap dilation(Bitmap image, int bSize)
{
    Bitmap dilated(image);

    int width = image.getWidth();
    int height = image.getHeight();
    unsigned char value;
    int origin = (int)bSize/2 + 1;
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
* @func opening
* @brief efetua operacao de abertura na imagem
* @param image imagem binaria
* @param bSize dimensao do elemento estruturante, que eh um quadrado
* @return imagem tratada
*/
Bitmap opening(Bitmap image, int bSize)
{
    /* operacao de erosao + dilatacao */
    return dilation(erosion(image, bSize), bSize);
}


/* Funcoes uteis para manipulacao do stream de bytes */
bool big_endian()
{
    unsigned int v = 0x01;
    return (1 != reinterpret_cast<char*>(&v)[0]);
}

unsigned short flip(const unsigned short& v)
{
    return ((v >> 8) | (v << 8));
}

unsigned int flip(const unsigned int& v)
{
    return (((v & 0xFF000000) >> 0x18) | ((v & 0x000000FF) << 0x18) |
        ((v & 0x00FF0000) >> 0x08) | ((v & 0x0000FF00) << 0x08));
}

/* Funcoes utilizadas para tratamento de arquivo */

template<typename T>
void read_from_stream(std::ifstream& stream,T& t)
{
    stream.read(reinterpret_cast<char*>(&t),sizeof(T));
}


template<typename T>  void write_to_stream(std::ofstream& stream,const T& t)
{
    stream.write(reinterpret_cast<const char*>(&t),sizeof(T));
}

/* Le cabecalho do arquivo em memoria */
void readFileHeader(std::ifstream& stream, BMPFileHeader& fileHeader)
{
    read_from_stream(stream,fileHeader.type);
    read_from_stream(stream,fileHeader.size);
    read_from_stream(stream,fileHeader.reserved1);
    read_from_stream(stream,fileHeader.reserved2);
    read_from_stream(stream,fileHeader.off_bits);

    if(big_endian())
    {
        flip(fileHeader.type);
        flip(fileHeader.size);
        flip(fileHeader.reserved1);
        flip(fileHeader.reserved2);
        flip(fileHeader.off_bits);
    }
}

/* Escreve cabecalho do arquivo em memoria */
void writeFileHeader(std::ofstream& stream, const BMPFileHeader& fileHeader)
{
    if(big_endian())
    {
        flip(fileHeader.type);
        flip(fileHeader.size);
        flip(fileHeader.reserved1);
        flip(fileHeader.reserved2);
        flip(fileHeader.off_bits);
    }
    write_to_stream(stream,fileHeader.type);
    write_to_stream(stream,fileHeader.size);
    write_to_stream(stream,fileHeader.reserved1);
    write_to_stream(stream,fileHeader.reserved2);
    write_to_stream(stream,fileHeader.off_bits);
}

/* Le informacoes do arquivo em memoria */
void readInfoHeader(std::ifstream& stream, BMPInfoHeader& infoHeader)
{
    read_from_stream(stream,infoHeader.size);
    read_from_stream(stream,infoHeader.width);
    read_from_stream(stream,infoHeader.height);
    read_from_stream(stream,infoHeader.planes);
    read_from_stream(stream,infoHeader.bit_count);
    read_from_stream(stream,infoHeader.compression);
    read_from_stream(stream,infoHeader.size_image);
    read_from_stream(stream,infoHeader.x_pels_per_meter);
    read_from_stream(stream,infoHeader.y_pels_per_meter);
    read_from_stream(stream,infoHeader.clr_used);
    read_from_stream(stream,infoHeader.clr_important);

    if(big_endian())
    {
        flip(infoHeader.size);
        flip(infoHeader.width);
        flip(infoHeader.height);
        flip(infoHeader.planes);
        flip(infoHeader.bit_count);
        flip(infoHeader.compression);
        flip(infoHeader.size_image);
        flip(infoHeader.x_pels_per_meter);
        flip(infoHeader.y_pels_per_meter);
        flip(infoHeader.clr_used);
        flip(infoHeader.clr_important);
    }
}

/* Escreve informacoes do arquivo em memoria */
void writeInfoHeader(std::ofstream& stream, const BMPInfoHeader& infoHeader)
{
    if(big_endian())
    {
        flip(infoHeader.size);
        flip(infoHeader.width);
        flip(infoHeader.height);
        flip(infoHeader.planes);
        flip(infoHeader.bit_count);
        flip(infoHeader.compression);
        flip(infoHeader.size_image);
        flip(infoHeader.x_pels_per_meter);
        flip(infoHeader.y_pels_per_meter);
        flip(infoHeader.clr_used);
        flip(infoHeader.clr_important);
    }

    write_to_stream(stream,infoHeader.size);
    write_to_stream(stream,infoHeader.width);
    write_to_stream(stream,infoHeader.height);
    write_to_stream(stream,infoHeader.planes);
    write_to_stream(stream,infoHeader.bit_count);
    write_to_stream(stream,infoHeader.compression);
    write_to_stream(stream,infoHeader.size_image);
    write_to_stream(stream,infoHeader.x_pels_per_meter);
    write_to_stream(stream,infoHeader.y_pels_per_meter);
    write_to_stream(stream,infoHeader.clr_used);
    write_to_stream(stream,infoHeader.clr_important);
}
