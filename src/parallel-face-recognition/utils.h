/*
 * utils.h
 *
 *  Created on: 06/04/2015
 *      Author: felipe
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

void showDetectedFaceWithRectangle(CvSeq *rects, IplImage * inputImg);

#endif /* UTILS_H_ */
