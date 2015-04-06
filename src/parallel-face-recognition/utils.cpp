#include "utils.h"

void showDetectedFaceWithRectangle(CvSeq *rects, IplImage * inputImg)
{
    const char *winName = "Display Faces";
    cvNamedWindow(winName,CV_WINDOW_NORMAL);

    for ( int i = 0; i < (rects? rects->total:0); i++ )
    {
        CvRect *r = (CvRect*)cvGetSeqElem(rects,i);
        CvPoint p1 = { r->x, r->y };
        CvPoint p2 = { r->x + r->width, r->y + r->height };

        cvRectangle(inputImg,p1,p2,CV_RGB(0,255,0),1,4,0);
    }

    cvShowImage(winName, inputImg);
    cvWaitKey(0);
    // destroy the view window
    cvDestroyWindow(winName);
}

