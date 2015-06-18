#include "utils.h"

CharVect::CharVect() :
        x(0), y(0)
{
    initChars();
}

CharVect::CharVect(int x_, int y_) :
        x(x_), y(y_)
{
    initChars();
}

void CharVect::setChars(int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    c[3] = c4;
    c[4] = c5;
    c[5] = c6;
    c[6] = c7;
}

bool CharVect::operator <= (CharVect const& other)
{
    for(int i = 0; i < CHARS_SIZE; i++)
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
    for(int i = 0; i < CHARS_SIZE; i++)
        c[i] = 0;
}
