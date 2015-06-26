#include "utils_old.h"

CharVectListOld::CharVectListOld() :
        vect(), next(NULL)
{
}

CharVectListOld::CharVectListOld(CharVect const& charVect) :
    vect(charVect), next(NULL)
{
}

CharVectListOld::CharVectListOld(int x_, int y_) :
        vect(x_, y_), next(NULL)
{
}

CharVectListOld::CharVectListOld(CharVectListOld const& other)
{
    this->vect = other.vect;
    this->next = NULL;
}

CharVectListOld& CharVectListOld::operator =(CharVectListOld const& other)
{
    this->vect = other.vect;
    return *this;
}

/********************************************************************************/

SimilarBlocksOld::SimilarBlocksOld() :
        b1(),
        b2(),
        dx(0),
        dy(0),
        next(NULL)
{
}

SimilarBlocksOld::SimilarBlocksOld(Pos const& b1, Pos const& b2) :
        next(NULL)
{
    setValues(b1, b2);
}

void SimilarBlocksOld::setValues(Pos const& b1_, Pos const& b2_)
{
    b1 = b1_;
    b2 = b2_;
    dx = b1.x - b2.x;
    dy = b1.y - b2.y;
}

bool SimilarBlocksOld::operator==(SimilarBlocksOld const& other)
{
    if(b1 != other.b1) return false;
    if(b2 != other.b2) return false;
    if(dx != other.dx) return false;
    if(dy != other.dy) return false;
    return true;
}

bool SimilarBlocksOld::operator!=(SimilarBlocksOld const& other)
{
    return !((*this) == other);
}
