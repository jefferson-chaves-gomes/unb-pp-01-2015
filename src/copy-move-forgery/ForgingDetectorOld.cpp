#include "ForgingDetectorOld.h"

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
int ForgingDetectorOld::dbgmsg(0);
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
bool ForgingDetectorOld::byCharact(Bitmap image, int bSize)
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
    SimilarBlocksOld* simList = createSimilarBlockList(image, bSize, vList);

    /* passo 3: */
    // Se nao ha blocos similares, a imagem nao foi adulterada por copy-move
    if(simList == NULL)
        return false;

    logger("[MSG " << ++dbgmsg << "] Analisando shifts de deslocamento...");
    filterSpuriousRegions(simList);

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

    SimilarBlocksOld*simBlock = simList;
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
CharVectList* ForgingDetectorOld::charactVector(Bitmap image, int bSize)
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
        vetor->vect.pos.x = dx;
        vetor->vect.pos.y = dy;
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

    logger("A imagem possui " << iCount << " blocos.");

    return vList;
}

SimilarBlocksOld* ForgingDetectorOld::createSimilarBlockList(Bitmap const& image, int bSize, CharVectList* vList)
{
    SimilarBlocksOld* simList = NULL;
    SimilarBlocksOld* simBlock = NULL;
    int width = image.getWidth();
    int height = image.getHeight();

    SimilarBlocksOld* simEnd = NULL;
    CharVectList* b1Vector = vList;
    CharVectList* b2Vector = NULL;
    double diff[CharVect::CHARS_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };
    int L;                           // comprimento do vetor deslocamento

    // percorrer toda a lista de blocos; execucao em O(n)
    // somente sao comparados dois blocos consecutivos, pois ja estao ordenados
    if(bSize + BASE_L >= width || bSize + BASE_L >= height)
        L = bSize;
    else
        L = BASE_L;
    while(b1Vector != NULL)
    {
        b2Vector = b1Vector->next;
        if(b2Vector != NULL)
        {
            // calcular diferencas

            bool diffVector = true;

            for(int i = 0; i < CharVect::CHARS_SIZE && diffVector; i++)
            {
                diff[i] = ABS((b1Vector->vect.c[i] - b2Vector->vect.c[i]));
                diffVector = diffVector && (diff[i] < vectorP[i]);
            }

            if(diffVector && (diff[0] + diff[1] + diff[2] < t1) && (diff[3] + diff[4] + diff[5] + diff[6] < t2))
            {
                if(ABS(getShift(b1Vector->vect.pos.x, b2Vector->vect.pos.x, b1Vector->vect.pos.y, b2Vector->vect.pos.y)) > L)
                {
                    // blocos b1 e b2 sao similares
                    simBlock = newSimilarBlock(b1Vector->vect.pos.x, b2Vector->vect.pos.x, b1Vector->vect.pos.y, b2Vector->vect.pos.y);
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

    return simList;
}

void ForgingDetectorOld::filterSpuriousRegions(SimilarBlocksOld* simList)
{
    SimilarBlocksOld* simTrace = simList;
    SimilarBlocksOld* simBlock = simList;
    SimilarBlocksOld* mainShift = getMainShiftVector(simList);
    while(simBlock != NULL)
    {
        if(ABS((simBlock->dx - mainShift->dx)) > MAX_SHIFT || ABS((simBlock->dy - mainShift->dy)) > MAX_SHIFT)
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
int ForgingDetectorOld::getShift(int x1, int x2, int y1, int y2)
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
SimilarBlocksOld* ForgingDetectorOld::newSimilarBlock(int x1, int x2, int y1, int y2)
{
    SimilarBlocksOld* block = new SimilarBlocksOld;

    block->b1.x = x1;
    block->b2.x = x2;
    block->b1.y = y1;
    block->b2.y = y2;
    block->dx = x1 - x2;
    block->dy = y1 - y2;
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
SimilarBlocksOld* ForgingDetectorOld::getMainShiftVector(SimilarBlocksOld* blocks)
{
    int dx, dy, count = 0;
    SimilarBlocksOld* main = NULL;
    SimilarBlocksOld* auxBlock = blocks;
    HistogramOld* hist = NULL;
    HistogramOld* hTrace = NULL;
    HistogramOld* hLast = NULL;

    /* criar HistogramOlda de vetores */
    while(auxBlock != NULL)
    {
        dx = auxBlock->dx;
        dy = auxBlock->dy;
        if(hist == NULL)
        {
            hist = newHistogramOld(dx, dy, auxBlock);
            hLast = hist;
        }

        /* procurar por entrada no HistogramOlda */
        hTrace = hist;
        while(hTrace != NULL)
        {
            if(hTrace->dx == dx && hTrace->dy == dy)
                break;
            hTrace = hTrace->next;
        }
        if(hTrace == NULL)
        {
            hTrace = newHistogramOld(dx, dy, auxBlock);
            hLast->next = hTrace;
            hLast = hTrace;
        }

        /* buscar maior frequencia */
        hTrace->freq++;
        if(hTrace->freq > count)
		{
			count = hTrace->freq;
			main = hTrace->rep;
		}

        auxBlock = auxBlock->next;
    }

    clearHistogramOld(hist);

    return main;
}

/**
 * @func opening
 * @brief efetua operacao de abertura na imagem
 * @param image imagem binaria
 * @param bSize dimensao do elemento estruturante, que eh um quadrado
 * @return imagem tratada
 */
Bitmap ForgingDetectorOld::opening(Bitmap image, int bSize)
{
    /* operacao de erosao + dilatacao */
    return dilation(erosion(image, bSize), bSize);
}

/**
 * @func clearVectors
 * @brief limpa a lista de vetores de caracteristicas
 * @param start ponteiro para o inicio da lista
 */
void ForgingDetectorOld::clearCharVectors(CharVectList* start)
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
void ForgingDetectorOld::clearSimilarBlocks(SimilarBlocksOld* start)
{
    SimilarBlocksOld* aux = start;
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

CharVectList* ForgingDetectorOld::addVectLexOrder(CharVectList* start, CharVectList* vetor)
{
    if(start == NULL)
        return vetor;

    CharVectList* aux = start;
    CharVectList* trace = start;
    bool isGreater = false;
    bool isSmaller = false;

    while(aux != NULL)
    {
        isGreater = false;
        isSmaller = false;

        for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        {
            if(vetor->vect.c[i] == aux->vect.c[i])
                continue;
            if(vetor->vect.c[i] > aux->vect.c[i])
                isGreater = true;
            else
                isSmaller = true;

            break;
        }

        if(isGreater)
        {
            trace = aux;
            aux = aux->next;
            continue;
        }
        if(isSmaller)
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

    if(isGreater)
        trace->next = vetor;

    return start;
}


/**
 * @func newHistogramOld
 * @brief cria uma nova entrada para o HistogramOlda
 * @param dx deslocamento horizontal
 * @param dy deslocamento vertical
 * @param rep representante do vetor
 * @return elemento do HistogramOlda
 */
HistogramOld* ForgingDetectorOld::newHistogramOld(int dx, int dy, SimilarBlocksOld* rep)
{
    HistogramOld* hist = new HistogramOld;

    hist->dx = dx;
    hist->dy = dy;
    hist->rep = rep;
    hist->freq = 0;
    hist->next = NULL;

    return hist;
}

/**
 * @func clearHistogramOld
 * @brief limpa o HistogramOlda
 * @param start ponteiro para o inicio da lista
 */
void ForgingDetectorOld::clearHistogramOld(HistogramOld* start)
{
    HistogramOld* aux = start;
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
Bitmap ForgingDetectorOld::dilation(Bitmap image, int bSize)
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
Bitmap ForgingDetectorOld::erosion(Bitmap image, int bSize)
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
