#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>
#include <list>
#include "commons.h"

/* para os blocos similares */
class SimilarBlocks
{
public:
    // para blocos de tamanho fixo
    Pos b1;
    Pos b2;
    DeltaPos delta;

    SimilarBlocks();
    SimilarBlocks(Pos const& b1, Pos const& b2);
    bool operator==(SimilarBlocks const& other);
    bool operator!=(SimilarBlocks const& other);
    void setValues(Pos const& b1, Pos const& b2);
    static bool lessOrEqualsToPtr(SimilarBlocks const* obj1, SimilarBlocks const* obj2);
    static bool lessOrEqualsTo(SimilarBlocks const& obj1, SimilarBlocks const& obj2);
};

typedef std::list<SimilarBlocks>  ListSimilarBlocks;

typedef std::list<CharVect> ListCharVect;
typedef std::list<CharVect*> ListCharVectPtr;

#endif
