
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

    CvMemStorage *storage = cvCreateMemStorage(0);
    cvClearMemStorage( storage );

    // If the image is color, use a greyscale copy of the image.
    detectImg = (IplImage*)inputImg;
    if (inputImg->nChannels > 1) {
        greyImg = cvCreateImage(cvGetSize(inputImg), IPL_DEPTH_8U, 1 );
        cvCvtColor( inputImg, greyImg, CV_BGR2GRAY );
        detectImg = greyImg;    // Use the greyscale image.
    }

    rects = cvHaarDetectObjects(
            detectImg,
            cascade,
            storage,
            1.1f, // How detailed should the search be.
            3,
            CV_HAAR_DO_CANNY_PRUNING,//CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH, // Only search for 1 face.
            cvSize(20, 20)// Smallest face size.
    );
    std::cout << "Face Detection found [" << rects->total << "] objects" << std::endl;

//    showDetectedFaceWithRectangle(rects, detectImg);


    // Resize the image to be a consistent size, even if the aspect ratio changes.
    IplImage *imageProcessed;
    imageProcessed = cvCreateImage(cvGetSize(inputImg), IPL_DEPTH_8U, 1);
    // Make the image a fixed size.
    // CV_INTER_CUBIC or CV_INTER_LINEAR is good for enlarging, and
    // CV_INTER_AREA is good for shrinking / decimation, but bad at enlarging.
    cvResize(greyImg, imageProcessed, CV_INTER_LINEAR);

    // Give the image a standard brightness and contrast.
    cvEqualizeHist(imageProcessed, imageProcessed);

    rects = cvHaarDetectObjects(
            detectImg,
            cascade,
            storage,
            1.1f, // How detailed should the search be.
            3,
            CV_HAAR_DO_CANNY_PRUNING,//CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH, // Only search for 1 face.
            cvSize(20, 20)// Smallest face size.
    );

    showDetectedFaceWithRectangle(rects, imageProcessed);

    if (imageProcessed)
        cvReleaseImage(&imageProcessed);



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
