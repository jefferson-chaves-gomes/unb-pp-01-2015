#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>
#include <list>

#define CHARACT_VECTOR      "-v"
#define GRAYSCALE           "-c"
#define EROSION             "-e"
#define DILATION            "-d"
#define OPENING             "-o"
#define HELP                "-help"

#define BLOCKSHIFT          1
#define BASE_L              50

#define ABS(X) ((X < 0) ? -X : X)

class LinkedListCleaner
{
public:
    template<class T>
    static void clear(T* start)
    {
        T* aux = start;
        while(start != NULL)
        {
            aux = start->next;
            delete start;
            start = aux;
        }
    }
};

class Pos
{
public:
    int x;       // coordenada inicial x do bloco
    int y;       // coordenada inicial y do bloco
    Pos() : x(0), y(0){}
    Pos(int x_, int y_) : x(x_), y(y_){}
    Pos(Pos const& other) : x(other.x), y(other.y){}
    Pos& operator=(Pos const& other)
    {
        x = other.x;
        y = other.y;
        return *(this);
    }

    bool operator==(Pos const& other) const
    {
        if(x != other.x) return false;
        if(y != other.y) return false;
        return true;
    }

    bool operator!=(Pos const& other) const
    {
        return !((*this) == other);
    }
};

class DeltaPos
{
public:
    int dx;
    int dy;

    DeltaPos() :
        dx(0),
        dy(0)
    {
    }

    DeltaPos(Pos const& pos1, Pos const& pos2) :
        dx(pos1.x-pos2.x),
        dy(pos1.y-pos2.y)
    {
    }

    DeltaPos(int dx_, int dy_) :
        dx(dx_),
        dy(dy_)
    {
    }

    DeltaPos& operator=(DeltaPos const& other)
    {
        dx = other.dx;
        dy = other.dy;
        return *(this);
    }

    bool operator==(DeltaPos const& other) const
    {
        if(dx != other.dx) return false;
        if(dy != other.dy) return false;
        return true;
    }

    bool operator!=(DeltaPos const& other) const
    {
        return !((*this) == other);
    }

    bool operator<(DeltaPos const& other) const
    {
        return (dx < other.dx) || ( (dx == other.dx)&&(dy < other.dy) );
    }
};

/* para o algoritmo de vetor de caracteristicas */
class CharVect
{
public:
    static const int CHARS_SIZE = 7;
    Pos pos;
    double c[CHARS_SIZE];
    //     c1:   // media R
    //     c2:   // media G
    //     c3:   // media B
    //     c4:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 1
    //     c5:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 2
    //     c6:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 3
    //     c7:   // soma(part(1))/soma(part(1)+part(2)) regiao tipo 4
    CharVect();
    CharVect(CharVect const&);
    CharVect(int x_, int y_);

    CharVect& operator =(CharVect const& other);
    bool operator <=(CharVect const& other);

    void setChars(int c1, int c2, int c3, int c4, int c5, int c6, int c7);

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
    CharVectList(CharVectList const&);
    CharVectList& operator =(CharVectList const& other);
};

/* para os blocos similares */
class SimilarBlocksOld
{
public:
    // para blocos de tamanho fixo
    Pos b1;
    Pos b2;
    int dx;
    int dy;
    SimilarBlocksOld* next;

    SimilarBlocksOld();
    SimilarBlocksOld(Pos const& b1, Pos const& b2);
    bool operator==(SimilarBlocksOld const& other);
    bool operator!=(SimilarBlocksOld const& other);
    void setValues(Pos const& b1, Pos const& b2);
};

/* para os blocos similares */
class SimilarBlocks
{
public:
    // para blocos de tamanho fixo
    Pos b1;
    Pos b2;
    DeltaPos delta;

    SimilarBlocks();
    SimilarBlocks(Pos const& b1, Pos const& b2);
    bool operator==(SimilarBlocks const& other);
    bool operator!=(SimilarBlocks const& other);
    void setValues(Pos const& b1, Pos const& b2);
};

typedef std::list<SimilarBlocks>  ListSimilarBlocks;

class HistogramOld
{
public:
    int freq;            // contador de frequencia
    int dx;              // diferenca (com sinal) da componente horizontal
    int dy;              // diferenca (com sinal) da componente vertical
    SimilarBlocksOld* rep;  // par de blocos que representa o vetor
    HistogramOld* next;

    HistogramOld() :
        freq(0),
        dx(0),
        dy(0),
        rep(NULL),
        next(NULL){}

    HistogramOld(
            int dx_,
            int dy_) :
        freq(0),
        dx(dx_),
        dy(dy_),
        rep(NULL),
        next(NULL){}

    void setRep(SimilarBlocksOld* rep)
    {
        this->rep = rep;
    }
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
