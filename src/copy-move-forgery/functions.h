#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "utils.h"
#include "Bitmap.h"

CharVectList* newCharVect();
CharVectList* addVectLexOrder(CharVectList* start, CharVectList* vetor);
void clearCharVectors(CharVectList* start);

SimilarBlocks* newSimilarBlock(int x1, int x2, int y1, int y2, bool equal = false);
SimilarBlocks* getMainShiftVector(SimilarBlocks* blocks);
MaxShifts getMainShifts(SimilarBlocks* blocks);
void clearSimilarBlocks(SimilarBlocks* start);

Histogram* newHistogram(int dx, int dy, SimilarBlocks* rep);
void clearHistogram(Histogram* start);

std::string imgTrueName(std::string file);
void saveImageAs(Bitmap image, std::string imgPath);
int randAlpha(int range = 1);
int getShift(int x1, int x2, int y1, int y2);
bool isGreaterShift(SimilarBlocks* block, MaxShifts maxSh, int maxShift);
void printUsage(void);

Bitmap imgGrayScale(Bitmap image, bool fromfile = false);

/* metodos de deteccao */
bool forgeringByCharact(Bitmap image, bool multiregion = false, int bSize = 16);
CharVectList* charactVector(Bitmap image, int bSize);

/* operacoes morfologicas */
Bitmap erosion(Bitmap image, int bSize);
Bitmap dilation(Bitmap image, int bSize);
Bitmap opening(Bitmap image, int bSize);

void readInfoHeader(std::ifstream& stream,BMPInfoHeader& bih);
void readFileHeader(std::ifstream& stream, BMPFileHeader& bfh);
void writeInfoHeader(std::ofstream& stream, const BMPInfoHeader& bih);
void writeFileHeader(std::ofstream& stream, const BMPFileHeader& bfh);

#endif
