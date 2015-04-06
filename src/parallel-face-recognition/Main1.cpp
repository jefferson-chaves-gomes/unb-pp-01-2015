
#include "main.h"

// Perform face detection on the input image, using the given Haar Cascade.
// Returns a rectangle for the detected region in the given image.
CvRect detectFaceInImage(IplImage *inputImg, CvHaarClassifierCascade* cascade)
{
    IplImage *detectImg;
    IplImage *greyImg = 0;
    CvRect rc;
    double t;
    CvSeq* rects;
    CvSize size;

    CvMemStorage *storage = cvCreateMemStorage(0);
    cvClearMemStorage( storage );

    // If the image is color, use a greyscale copy of the image.
    detectImg = (IplImage*)inputImg;
    if (inputImg->nChannels > 1) {
        size = cvSize(inputImg->width, inputImg->height);
        greyImg = cvCreateImage(size, IPL_DEPTH_8U, 1 );
        cvCvtColor( inputImg, greyImg, CV_BGR2GRAY );
        detectImg = greyImg;    // Use the greyscale image.
    }

    // Detect all the faces in the greyscale image.
    t = (double)cvGetTickCount();

    rects = cvHaarDetectObjects(
            detectImg,
            cascade,
            storage,
            1.1f, // How detailed should the search be.
            3,
            CV_HAAR_DO_CANNY_PRUNING,//CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH, // Only search for 1 face.
            cvSize(20, 20)// Smallest face size.
    );

    showDetectedFaceWithRectangle(rects, detectImg);

    t = (double)cvGetTickCount() - t;

    std::cout << "Face Detection found [" << rects->total << "] objects" << std::endl;

    // Get the first detected face (the biggest).
    if (rects->total > 0)
        rc = *(CvRect*)cvGetSeqElem( rects, 0 );
    else
        rc = cvRect(-1,-1,-1,-1);   // Couldn't find the face.

    if (greyImg)
        cvReleaseImage( &greyImg );
    cvReleaseMemStorage( &storage );

    return rc;  // Return the biggest face found, or (-1,-1,-1,-1).
}

void DetectMyFace1 (std::string const& imgFile)
{
    // Load the HaarCascade classifier for face detection.
    CvHaarClassifierCascade* faceCascade =
            (CvHaarClassifierCascade*)cvLoad(faceCascadeFilenameAlt.c_str(), 0, 0, 0);

    IplImage* inputImg  =
            cvLoadImage(imgFile.c_str(), 1);

    if ( !inputImg || !faceCascade )
    {
        std::cout << "Initilization error: " <<
                (!inputImg)? "cant load image" : (!faceCascade)? "cant load haar cascade" : "unable to locate memory storage";

        return;
    }

    // Perform face detection on the input image, using the given Haar classifier
    CvRect faceRect = detectFaceInImage(inputImg, faceCascade);

    // Make sure a valid face was detected.
    if (faceRect.width > 0)
        std::cout << "Detected a face at (" << faceRect.x << "," << faceRect.y << ")" << std::endl;
    else
        std::cout << "No face detected" << std::endl;

    // Free the Face Detector resources when the program is finished
    cvReleaseHaarClassifierCascade( &faceCascade );
}
