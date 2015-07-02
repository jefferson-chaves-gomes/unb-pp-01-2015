#include "ForgingDetectorMPI.h"
#include "ForgingDetector.h"

#include "commons.h"
#include "ImgUtils.h"
#include "Timer.h"
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <mpi/mpi.h>
#include "MPISettings.h"

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
int ForgingDetectorMPI::dbgmsg(0);
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
bool ForgingDetectorMPI::byCharact(Bitmap const& image, int bSize)
{
    unsigned int width = image.getWidth();
    unsigned int height = image.getHeight();

    MPI_Bcast(&width, 1, MPI_INT, MPISettings::PROC_MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, MPISettings::PROC_MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&bSize, 1, MPI_INT, MPISettings::PROC_MASTER, MPI_COMM_WORLD);

    /* passo 1: extrair as caracteristicas dos blocos da imagem */
    logger("[MSG " << ++dbgmsg << "] Criando vetores de caracteristicas...");
    ListCharVectPtr vList;
    charactVector(vList, image, width, height, bSize);

    if(!vList.size() && MPISettings::IS_PROC_ID_MASTER())
    {
        std::cout << "Nao foi possivel criar o vetor de caracteristicas." << std::endl;
        return false;
    }

//    if(!MPISettings::IS_PROC_ID_MASTER())
//        return false;

    /* passo 2: buscar blocos similares */
    logger("[MSG " << ++dbgmsg << "] Buscando blocos similares...");
    ListSimilarBlocks simList;
    createSimilarBlockList(image, bSize, vList, simList, getVectOffsetSize(width, height, bSize));

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

void ForgingDetectorMPI::charactVector(ListCharVectPtr& listChar, Bitmap const& image, int imageWidth, int imageHeight, int bSize)
{
    listChar.clear();

    // bloco deve estar dentro dos limites da imagem
    if(imageWidth < bSize || imageHeight < bSize)
        return;

    if(MPISettings::IS_PROC_ID_MASTER())
    {
//        int bTotalXOrign = imageWidth - bSize + 1;
//        int bTotalYOrign = imageHeight - bSize + 1;
//
//        logger("A imagem possui " << bTotalXOrign * bTotalYOrign << " blocos.");
    }

    int sections = MPISettings::PROC_SIZE();

    const int scope = (imageHeight - bSize) + 1;

    if(sections > scope)
        sections = scope;

    if(MPISettings::PROC_ID() >= sections)
        return;

    int sizeLast = (scope) % sections;
    int size = (scope) / sections;

    int sectionWidth = imageWidth;
    int sectionHeight = size-1+bSize;

    Bitmap *section;
    // inicia a transferencia de secoes da imagem
    if(MPISettings::IS_PROC_ID_MASTER())
    {
        section = new Bitmap(sectionWidth, sectionHeight);
        memcpy(section->data_, image.data_, section->length_);

        int posStart = 0;
        int lengthToSend = 0;
        unsigned int bytesPerPixel = image.bytes_per_pixel_;
        for(int i=1; i<sections; i++)
        {
            if(sizeLast!=0 && i == sections-1)
                sectionHeight = sectionHeight + sizeLast;

            posStart = size*i * sectionWidth * bytesPerPixel;
            lengthToSend = sectionHeight * sectionWidth * bytesPerPixel;

            MPI_Send(&sectionHeight, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send((image.data_+posStart), lengthToSend, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&sectionHeight, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        section = new Bitmap(sectionWidth, sectionHeight);
        MPI_Recv(section->data_, section->length_, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int bTotalX = section->getWidth() - bSize + 1;
    int bTotalY = section->getHeight() - bSize + 1;
    int PROC_ID = MPISettings::PROC_ID();

    // itera em todos os blocos da secao
    for(int bx = 0; bx < bTotalX; bx++)
    {
        for(int by = 0; by < bTotalY; by++)
        {
            // criar vetor de caracteristicas
            CharVect * charVect = new CharVect(bx, by + size*PROC_ID);

            getCharVectListForBlock(*charVect, *section, bx, by, bSize);

            // adicionar o bloco lido ao conjunto de vetores de caracteristicas
            listChar.push_back(charVect);
        }
    }

    // Cada processo realiza um quick sort de seus elementos
    listChar.sort(CharVect::lessOrEqualsToPtr);
    delete section;

    int procToReceive(-1);
    int procToSend(-1);
    int vecSize = 0;

    CharVect * charVect;
    ListCharVectPtr::iterator
        itToMerge = listChar.begin();

    // Realiza uma redução dos vetores ordenados
    int PROC_SIZE = MPISettings::PROC_SIZE();
    for(int i=1; i<PROC_SIZE; i*=2)
    {
        // Loop que percorre processos
        for(int j=0; j<PROC_SIZE; j+=i)
        {
            // Só prossegue se a iteração atual pertencer ao processo
            if(j % PROC_SIZE != PROC_ID)
                continue;

            // Se a iteração atual do loop interno for múltipla
            // do passo da próxima iteração do loop externo, recebe valor
            if((j % (i*2)) == 0)
            {
                // Evita receber de um processo superior aos existentes
                if((j + i) >= PROC_SIZE)
                    continue;

                procToReceive = (j + i) % PROC_SIZE;

                MPI_Recv(&vecSize, 1, MPI_INT, procToReceive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for(; vecSize > 0; vecSize--)
                {
                    charVect = receiveCharVectFromProcess(procToReceive);

                    // Realiza o merge de elementos de dois processos ja ordenados
                    while(itToMerge!=listChar.end()
                            && CharVect::lessOrEqualsToPtr(*itToMerge, charVect))
                        itToMerge++;
                    listChar.insert(itToMerge, charVect);
                }
            }
            // Se não, envia valor
            else
            {
                // Evita enviar para um processo inferior aos existentes
                if((j - i) < 0)
                    continue;

                procToSend = (j - i) % PROC_SIZE;

                vecSize = listChar.size();
                MPI_Send(&vecSize, 1, MPI_INT, procToSend, 0, MPI_COMM_WORLD);
                for(ListCharVectPtr::iterator it = listChar.begin(); it!=listChar.end(); it++)
                    sendCharVectToProcess(*it, procToSend);

                // Evitar perder tempo deletendo os objetos dentro do envio de elementos
                for(ListCharVectPtr::iterator it = listChar.begin(); it!=listChar.end(); it++)
                    delete *it;
                listChar.clear();
            }
        }
    }
}

void ForgingDetectorMPI::createSimilarBlockList(
        Bitmap const& image,
        int bSize,
        ListCharVectPtr & vList,
        ListSimilarBlocks & simList,
        int vectOffsetSize)
{
    Timer time(PRINT_TIME, __PRETTY_FUNCTION__, __LINE__);
    double diff[CharVect::CHARS_SIZE] = { 0, 0, 0, 0, 0, 0, 0 };

    ListCharVectPtr::iterator itBegin = vList.begin();
    ListCharVectPtr::iterator itEnd = vList.end();

    int sectionLenght = 0;

    // inicia a transferencia de secoes de caracteristicas
    if(MPISettings::IS_PROC_ID_MASTER())
    {
        // Divisao de lista de caracteristicas em secoes
        int sections = MPISettings::PROC_SIZE();

        const int scope = vList .size()-1;

        if(sections > scope)
            sections = scope;

        int size = (scope) / sections;
        int sizeLast = size + ((scope) % sections);
        sectionLenght = size+1;

        // Processo 0 fica com o resto da lista
        for(int i=1; i<MPISettings::PROC_SIZE() ; i++)
        {
            if(i >= sections)
            {
                sectionLenght = 0;
                MPI_Send(&sectionLenght, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                continue;
            }

            sectionLenght = size+1;
            MPI_Send(&sectionLenght, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            for(; itBegin!=itEnd && sectionLenght > 1; itBegin++, sectionLenght--)
            {
                sendCharVectToProcess(*itBegin, i);
                delete *itBegin;
                itBegin = vList.erase(itBegin);
                itBegin--;
            }
            sendCharVectToProcess(*itBegin, i);
        }
    }
    else
    {
        CharVect * charVect;
        MPI_Recv(&sectionLenght, 1, MPI_INT, MPISettings::PROC_MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(; sectionLenght > 0; sectionLenght--)
        {
            charVect = receiveCharVectFromProcess(MPISettings::PROC_MASTER);
            vList.push_back(charVect);
        }

        itBegin = vList.begin();
        itEnd = vList.end();
    }

    // percorrer toda a lista de blocos; execucao em O(n)
    // somente sao comparados dois blocos consecutivos, pois ja estao ordenados

    CharVect* prev;
    for(; itBegin != vList.end(); prev = *(itBegin++))
    {
        if(itBegin == vList.begin())
            continue;

        // calcular diferencas
        bool diffVector = true;
        for(int i = 0; i < CharVect::CHARS_SIZE && diffVector; i++)
        {
            diff[i] = ABS((prev->c[i] - (*itBegin)->c[i]));
            diffVector = diffVector && (diff[i] < vectorP[i]);
        }

        if((diffVector)
                && (diff[0] + diff[1] + diff[2] < t1)
                && (diff[3] + diff[4] + diff[5] + diff[6] < t2)
                && ABS(getShift(prev->pos, (*itBegin)->pos)) > vectOffsetSize)
        {
            // blocos b1 e b2 sao similares
            simList.push_back(
                SimilarBlocks(
                    prev->pos,
                    (*itBegin)->pos));
        }
    }
}

void ForgingDetectorMPI::getCharVectListForBlock(CharVect& charVect, Bitmap const& image, int blkPosX, int blkPosY, int blkSize)
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

CharVect* ForgingDetectorMPI::receiveCharVectFromProcess(int procToReceive)
{
    CharVect * charVect = new CharVect(0,0);

    MPI_Recv(charVect->c, CharVect::CHARS_SIZE, MPI_DOUBLE, procToReceive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&charVect->pos.x, 1, MPI_INT, procToReceive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&charVect->pos.y, 1, MPI_INT, procToReceive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return charVect;
}

void ForgingDetectorMPI::sendCharVectToProcess(CharVect* charVect, int procToSend)
{
    MPI_Send(&charVect->c, CharVect::CHARS_SIZE, MPI_DOUBLE, procToSend, 0, MPI_COMM_WORLD);
    MPI_Send(&charVect->pos.x, 1, MPI_INT, procToSend, 0, MPI_COMM_WORLD);
    MPI_Send(&charVect->pos.y, 1, MPI_INT, procToSend, 0, MPI_COMM_WORLD);
}

int ForgingDetectorMPI::getVectOffsetSize(int imageWidth, int imageHeight, int bSize)
{
    int vectOffsetSize(BASE_L);
    if(bSize + BASE_L >= imageWidth || bSize + BASE_L >= imageHeight)
        vectOffsetSize = bSize;
    return vectOffsetSize;
}

bool ForgingDetectorMPI::isBlockSimilarSpurious(DeltaPos const& current, DeltaPos const& mainShift)
{
    return (ABS((current.dx - mainShift.dx)) > MAX_SHIFT || ABS((current.dy - mainShift.dy)) > MAX_SHIFT);
}

void ForgingDetectorMPI::filterSpuriousRegions(ListSimilarBlocks& simList, DeltaPos const& mainShift)
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
int ForgingDetectorMPI::getShift(Pos const& pos1, Pos const& pos2)
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

DeltaPos ForgingDetectorMPI::getMainShiftVector(ListSimilarBlocks const& blocks)
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

void ForgingDetectorMPI::createImageWithSimilarAreas(Bitmap& detectImage, Bitmap const& image, int bSize, ListSimilarBlocks const& simList)
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
Bitmap ForgingDetectorMPI::imageOpeningOperation(Bitmap const& image, int bSize)
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
Bitmap ForgingDetectorMPI::imageErosionOperation(Bitmap const& image, int bSize)
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
Bitmap ForgingDetectorMPI::imageDilationOperation(Bitmap const& image, int bSize)
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

bool ForgingDetectorMPI::isImageForged(Bitmap const& image, Bitmap const& detectImage, Bitmap& mergedImage)
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

