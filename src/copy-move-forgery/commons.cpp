#include "commons.h"

#include <cmath>

/**
 * @func toUnsignedChar
 * @brief converte um valor numerico para sua representacao como pixel de uma imagem
 * @param value valor numerico
 * @return valor do pixel (8 bits)
 */
unsigned char toUnsignedChar(double value)
{
    if(value > 255.0)
        return 255;
    else if(value < 0.0)
        return 0;
    else
        return static_cast<unsigned char>(floor(value + 0.5));
}

/******************************************************************************/

Pos::Pos() :
        x(0), y(0)
{
}

Pos::Pos(int x_, int y_) :
        x(x_), y(y_)
{
}

Pos::Pos(Pos const& other) :
        x(other.x), y(other.y)
{
}

Pos& Pos::operator=(Pos const& other)
{
    x = other.x;
    y = other.y;
    return *(this);
}

bool Pos::operator==(Pos const& other) const
{
    if(x != other.x)
        return false;
    if(y != other.y)
        return false;
    return true;
}

bool Pos::operator!=(Pos const& other) const
{
    return !((*this) == other);
}

/******************************************************************************/

CharVect::CharVect() :
    pos()
{
    initChars();
}

CharVect::CharVect(int x_, int y_) :
            pos(x_, y_)
{
    initChars();
}

CharVect& CharVect::operator =(CharVect const& other)
{
    pos = other.pos;
    setChars(
        other.c[0],
        other.c[1],
        other.c[2],
        other.c[3],
        other.c[4],
        other.c[5],
        other.c[6]);
    return *this;
}

CharVect::CharVect(CharVect const& other)
{
    pos = other.pos;
    setChars(
        other.c[0],
        other.c[1],
        other.c[2],
        other.c[3],
        other.c[4],
        other.c[5],
        other.c[6]);
}

void CharVect::setChars(double c1, double c2, double c3, double c4, double c5, double c6, double c7)
{
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    c[3] = c4;
    c[4] = c5;
    c[5] = c6;
    c[6] = c7;
}

bool CharVect::operator == (CharVect const& other) const
{
    for(int i = 0; i < CharVect::CHARS_SIZE; i++)
    {
        if(this->c[i] != other.c[i])
            return false;
    }

    return true;
}

bool CharVect::lessOrEqualsToPtr(CharVect const* obj1, CharVect const* obj2)
{
    return CharVect::lessOrEqualsTo(*obj1, *obj2);
}

bool CharVect::lessOrEqualsTo(CharVect const& obj1, CharVect const& obj2)
{
    for(int i = 0; i < CharVect::CHARS_SIZE; i++)
    {
        if(obj1.c[i] == obj2.c[i])
            continue;

        if(obj1.c[i] < obj2.c[i])
            return true;
        else
            return false;
    }

    return true;
}

void CharVect::initChars()
{
    setChars(0,0,0,0,0,0,0);
}

/******************************************************************************/

DeltaPos::DeltaPos() :
    dx(0),
    dy(0)
{
}

DeltaPos::DeltaPos(Pos const& pos1, Pos const& pos2) :
    dx(pos1.x-pos2.x),
    dy(pos1.y-pos2.y)
{
}

DeltaPos::DeltaPos(int dx_, int dy_) :
    dx(dx_),
    dy(dy_)
{
}

DeltaPos& DeltaPos::operator=(DeltaPos const& other)
{
    dx = other.dx;
    dy = other.dy;
    return *(this);
}

bool DeltaPos::operator==(DeltaPos const& other) const
{
    if(dx != other.dx) return false;
    if(dy != other.dy) return false;
    return true;
}

bool DeltaPos::operator!=(DeltaPos const& other) const
{
    return !((*this) == other);
}

bool DeltaPos::operator<(DeltaPos const& other) const
{
    return (dx < other.dx) || ( (dx == other.dx)&&(dy < other.dy) );
}
