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
