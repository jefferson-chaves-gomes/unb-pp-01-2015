#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>

#define CHARACT_VECTOR      "-v"
#define MULT_CHARACT_VECTOR "-m"
#define GRAYSCALE           "-c"
#define EROSION             "-e"
#define DILATION            "-d"
#define OPENING             "-o"
#define HELP                "-help"

#define BLOCKSHIFT          1
#define BASE_L              50

#define ABS(X) ((X < 0) ? -X : X)

/* para a comparacao de shifts */
class MaxShifts
{
public:
    int dx1, dy1;
    int dx2, dy2;
    int dx3, dy3;
};

/* para o algoritmo de vetor de caracteristicas */
class CharVect
{
public:
    static const int CHARS_SIZE = 7;
    int x;       // coordenada inicial x do bloco
    int y;       // coordenada inicial y do bloco
    double c[CHARS_SIZE];
    //     c1:   // media R
    //     c2:   // media G
    //     c3:   // media B
    //     c4:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 1
    //     c5:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 2
    //     c6:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 3
    //     c7:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 4
    CharVect();
    CharVect(int x_, int y_);

    void setChars(int c1, int c2, int c3, int c4, int c5, int c6, int c7);
    bool operator <=(CharVect const& other);

private:
    void initChars();
};

class CharVectList
{
public:
    CharVect vect;
    CharVectList* next;
    CharVectList();
    CharVectList(int x_, int y_);
};

/* para os blocos similares */
class SimilarBlocks
{
public:
    // para blocos de tamanho fixo
    int b1x;    // coordenada x de inicio do bloco 1
    int b1y;    // coordenada y de inicio do bloco 1
    int b2x;    // coordenada x de inicio do bloco 2
    int b2y;    // coordenada y de inicio do bloco 2
    int dx;     // dx = |x1 - x2|
    int dy;     // dy = |y1 - y2|
    bool equal; // marcado se alem de similares, dois blocos sao iguais
    SimilarBlocks* next;

    SimilarBlocks();
    bool operator==(SimilarBlocks const& other);
    bool operator!=(SimilarBlocks const& other);
    void setValues(int b1x_, int b1y_, int b2x_, int b2y_, int dx_, int dy_, bool equal_);
};

class Histogram
{
public:
    int freq;            // contador de frequencia
    int dx;              // diferenca (com sinal) da componente horizontal
    int dy;              // diferenca (com sinal) da componente vertical
    SimilarBlocks* rep;  // par de blocos que representa o vetor
    Histogram* next;

    Histogram() :
        freq(0),
        dx(0),
        dy(0),
        rep(NULL),
        next(NULL){}
};

typedef struct bitmap_file_header
{
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int off_bits;

    unsigned int struct_size()
    {
        return sizeof(type) + sizeof(size) + sizeof(reserved1) + sizeof(reserved2) + sizeof(off_bits);
    }
} BMPFileHeader;

typedef struct bitmap_information_header
{
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bit_count;
    unsigned int compression;
    unsigned int size_image;
    unsigned int x_pels_per_meter;
    unsigned int y_pels_per_meter;
    unsigned int clr_used;
    unsigned int clr_important;

    unsigned int struct_size()
    {
        return sizeof(size) + sizeof(width) + sizeof(height) + sizeof(planes) + sizeof(bit_count) + sizeof(compression)
                + sizeof(size_image) + sizeof(x_pels_per_meter) + sizeof(y_pels_per_meter) + sizeof(clr_used)
                + sizeof(clr_important);
    }
} BMPInfoHeader;

#endif
