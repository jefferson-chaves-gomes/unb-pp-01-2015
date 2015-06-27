#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>
#include <list>
#include "commons.h"

#ifdef MPI_ENABLED
int PROC_SIZE(0);
int PROC_ID(0);
#endif

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
};

typedef std::list<SimilarBlocks>  ListSimilarBlocks;

typedef std::list<CharVect> ListCharVect;

#endif
