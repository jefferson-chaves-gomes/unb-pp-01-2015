#ifndef _UTILSOLD_H_
#define _UTILSOLD_H_

#include "commons.h"

class LinkedListCleaner
{
public:
    template<class T>
    static void clear(T* start)
    {
        T* aux = start;
        while(start != NULL)
        {
            aux = start->next;
            delete start;
            start = aux;
        }
    }
};

class CharVectListOld
{
public:
    CharVect vect;
    CharVectListOld* next;
    CharVectListOld();
    CharVectListOld(CharVect const&);
    CharVectListOld(int x_, int y_);
    CharVectListOld(CharVectListOld const&);
    CharVectListOld& operator =(CharVectListOld const& other);
};

/* para os blocos similares */
class SimilarBlocksOld
{
public:
    // para blocos de tamanho fixo
    Pos b1;
    Pos b2;
    int dx;
    int dy;
    SimilarBlocksOld* next;

    SimilarBlocksOld();
    SimilarBlocksOld(Pos const& b1, Pos const& b2);
    bool operator==(SimilarBlocksOld const& other);
    bool operator!=(SimilarBlocksOld const& other);
    void setValues(Pos const& b1, Pos const& b2);
};

class HistogramOld
{
public:
    int freq;            // contador de frequencia
    int dx;              // diferenca (com sinal) da componente horizontal
    int dy;              // diferenca (com sinal) da componente vertical
    SimilarBlocksOld* rep;  // par de blocos que representa o vetor
    HistogramOld* next;

    HistogramOld() :
        freq(0),
        dx(0),
        dy(0),
        rep(NULL),
        next(NULL){}

    HistogramOld(
            int dx_,
            int dy_) :
        freq(0),
        dx(dx_),
        dy(dy_),
        rep(NULL),
        next(NULL){}

    void setRep(SimilarBlocksOld* rep)
    {
        this->rep = rep;
    }
};

#endif
