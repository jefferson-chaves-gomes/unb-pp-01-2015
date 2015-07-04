#include "utils.h"

SimilarBlocks::SimilarBlocks() :
        b1(),
        b2(),
        delta(0, 0)
{
}

SimilarBlocks::SimilarBlocks(Pos const& b1_, Pos const& b2_) :
        b1(b1_),
        b2(b2_),
        delta(b1_, b2_)
{
}

void SimilarBlocks::setValues(Pos const& b1_, Pos const& b2_)
{
    b1 = b1_;
    b2 = b2_;
    delta = DeltaPos(b1_, b2_);
}

bool SimilarBlocks::operator==(SimilarBlocks const& other)
{
    if(b1 != other.b1) return false;
    if(b2 != other.b2) return false;
    if(delta != other.delta) return false;
    return true;
}

bool SimilarBlocks::operator!=(SimilarBlocks const& other)
{
    return !((*this) == other);
}

bool SimilarBlocks::lessOrEqualsToPtr(SimilarBlocks const* obj1, SimilarBlocks const* obj2)
{
    int diff;

    diff = obj1->b1.x - obj2->b1.x;
    if(diff) return diff > 0;
    diff = obj1->b1.y - obj2->b1.y;
    if(diff) return diff > 0;
    diff = obj1->b2.x - obj2->b2.x;
    if(diff) return diff > 0;
    diff = obj1->b2.y - obj2->b2.y;
    if(diff) return diff > 0;
    diff = obj1->delta.dx - obj2->delta.dx;
    if(diff) return diff > 0;
    diff = obj1->delta.dy - obj2->delta.dy;
    if(diff) return diff > 0;

    return true;
}

bool SimilarBlocks::lessOrEqualsTo(SimilarBlocks const& obj1, SimilarBlocks const& obj2)
{
    return SimilarBlocks::lessOrEqualsToPtr(&obj1, &obj2);
}
