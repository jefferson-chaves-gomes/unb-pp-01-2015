#ifndef _COMMONS_H_
#define _COMMONS_H_

#include <stddef.h>
#include <list>

#define BLOCKSHIFT          1
#define BASE_L              50
#define MASTER_THREAD       0

#define ABS(X) ((X < 0) ? -X : X)

unsigned char toUnsignedChar(double value);

class Pos
{
public:
    int x;       // coordenada inicial x do bloco
    int y;       // coordenada inicial y do bloco

    Pos();
    Pos(int x_, int y_);
    Pos(Pos const& other);
    Pos& operator=(Pos const& other);
    bool operator==(Pos const& other) const;
    bool operator!=(Pos const& other) const;
};

class DeltaPos
{
public:
    int dx;
    int dy;

    DeltaPos();
    DeltaPos(Pos const& pos1, Pos const& pos2);
    DeltaPos(int dx_, int dy_);
    DeltaPos& operator=(DeltaPos const& other);
    bool operator==(DeltaPos const& other) const;
    bool operator!=(DeltaPos const& other) const;
    bool operator<(DeltaPos const& other) const;
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
    //     c4:   // soma(part(0)) / soma(part(0) + part(01)) regiao tipo 1
    //     c5:   // soma(part(1)) / soma(part(1) + part(11)) regiao tipo 2
    //     c6:   // soma(part(2)) / soma(part(2) + part(21)) regiao tipo 3
    //     c7:   // soma(part(3)) / soma(part(3) + part(31)) regiao tipo 4

    CharVect();
    CharVect(CharVect const&);
    CharVect(int x_, int y_);
    CharVect& operator =(CharVect const& other);
    bool operator ==(CharVect const& other) const;

    void setChars(double c1, double c2, double c3, double c4, double c5, double c6, double c7);

    static bool lessOrEqualsToPtr(CharVect const* obj1, CharVect const* obj2);
    static bool lessOrEqualsTo(CharVect const& obj1, CharVect const& obj2);
private:
    void initChars();
};

#endif
