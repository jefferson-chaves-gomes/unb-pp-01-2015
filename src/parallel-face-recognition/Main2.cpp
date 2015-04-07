
#include "main.h"

void DetectMyFace2 (std::string const& imgFile)
{
    // face detector classifer
    CvHaarClassifierCascade *faceCascade =
            (CvHaarClassifierCascade *) cvLoad(faceCascadeFilenameDefault.c_str(), 0, 0, 0);
    // image structure in opencv
    IplImage * inputImg =
            cvLoadImage(imgFile.c_str());

    if ( !inputImg || !faceCascade )
    {
        std::cout << "Initilization error: " <<
                (!inputImg)? "cant load image" : (!faceCascade)? "cant load haar cascade" : "unable to locate memory storage";

        return;
    }

    // face detector classifer
    CvMemStorage *mStorage =
            cvCreateMemStorage(0);

    CvSeq *rects = cvHaarDetectObjects(
            inputImg,
            faceCascade,
            mStorage,
            1.2,
            3,
            CV_HAAR_DO_CANNY_PRUNING,
            cvSize(25,25)
    );
    std::cout << "Face Detection found [" << rects->total << "] objects" << std::endl;

    showDetectedFaceWithRectangle(rects, inputImg);

    // release the variables
    cvReleaseImage(&inputImg);
    if(faceCascade) cvReleaseHaarClassifierCascade(&faceCascade);
    if(mStorage)  cvReleaseMemStorage(&mStorage);
}

