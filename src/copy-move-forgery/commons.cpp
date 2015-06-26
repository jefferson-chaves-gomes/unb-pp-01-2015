#include "commons.h"

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

bool CharVect::operator <= (CharVect const& other) const
{
    for(int i = 0; i < CharVect::CHARS_SIZE; i++)
    {
        if(this->c[i] == other.c[i])
            continue;

        if(c[i] < other.c[i])
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
