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
    for(int i = 0; i < CharVect::CHARS_SIZE; i++)
        c[i] = 0;
}

/********************************************************************************/
CharVectList::CharVectList() :
        vect(), next(NULL)
{
}

CharVectList::CharVectList(int x_, int y_) :
        vect(x_, y_), next(NULL)
{
}

/********************************************************************************/

SimilarBlocks::SimilarBlocks() :
        b1x(0),
        b1y(0),
        b2x(0),
        b2y(0),
        dx(0),
        dy(0),
        equal(false),
        next(NULL)
{
}

void SimilarBlocks::setValues(int b1x_,
            int b1y_,
            int b2x_,
            int b2y_,
            int dx_,
            int dy_,
            bool equal_)
{
    b1x = b1x_;
    b1y = b1y_;
    b2x = b2x_;
    b2y = b2y_;
    dx = dx_;
    dy = dy_;
    equal = equal_;
}

bool SimilarBlocks::operator==(SimilarBlocks const& other)
{
    if(b1x != other.b1x) return false;
    if(b1y != other.b1y) return false;
    if(b2x != other.b2x) return false;
    if(b2y != other.b2y) return false;
    if(dx != other.dx) return false;
    if(dy != other.dy) return false;
    if(equal != other.equal) return false;

    return true;
}

bool SimilarBlocks::operator!=(SimilarBlocks const& other)
{
    return !((*this) == other);
}
