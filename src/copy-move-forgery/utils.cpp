#include "utils.h"

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

CharVectList::CharVectList(CharVectList const& other)
{
    this->vect = other.vect;
    this->next = NULL;
}

CharVectList& CharVectList::operator =(CharVectList const& other)
{
    this->vect = other.vect;
    return *this;
}

/********************************************************************************/

SimilarBlocks::SimilarBlocks() :
        b1(),
        b2(),
        dx(0),
        dy(0),
        next(NULL)
{
}

SimilarBlocks::SimilarBlocks(Pos const& b1, Pos const& b2) :
        next(NULL)
{
    setValues(b1, b2);
}

void SimilarBlocks::setValues(Pos const& b1_, Pos const& b2_)
{
    b1 = b1_;
    b2 = b2_;
    dx = b1.x - b2.x;
    dy = b1.y - b2.y;
}

bool SimilarBlocks::operator==(SimilarBlocks const& other)
{
    if(b1 != other.b1) return false;
    if(b2 != other.b2) return false;
    if(dx != other.dx) return false;
    if(dy != other.dy) return false;
    return true;
}

bool SimilarBlocks::operator!=(SimilarBlocks const& other)
{
    return !((*this) == other);
}
