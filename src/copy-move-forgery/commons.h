#ifndef _COMMONS_H_
#define _COMMONS_H_

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

#define MASTER_THREAD       0

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
    bool operator <=(CharVect const& other) const;
    bool operator ==(CharVect const& other) const;

    void setChars(double c1, double c2, double c3, double c4, double c5, double c6, double c7);

private:
    void initChars();
};

#endif
