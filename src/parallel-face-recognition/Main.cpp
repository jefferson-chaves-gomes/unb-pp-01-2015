#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

// Perform face detection on the input image, using the given Haar Cascade.
// Returns a rectangle for the detected region in the given image.
CvRect detectFaceInImage(IplImage *inputImg, CvHaarClassifierCascade* cascade)
{
    // Smallest face size.
    CvSize minFeatureSize = cvSize(20, 20);
    // Only search for 1 face.
    int flags = CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_DO_ROUGH_SEARCH;
    // How detailed should the search be.
    float search_scale_factor = 1.1f;
    IplImage *detectImg;
    IplImage *greyImg = 0;
    CvMemStorage* storage;
    CvRect rc;
    double t;
    CvSeq* rects;
    CvSize size;
    int ms, nFaces;

    storage = cvCreateMemStorage(0);
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
    rects = cvHaarDetectObjects( detectImg, cascade, storage,
            search_scale_factor, 3, flags, minFeatureSize);
    t = (double)cvGetTickCount() - t;
    ms = cvRound( t / ((double)cvGetTickFrequency() * 1000.0) );
    nFaces = rects->total;

    std::cout << "Face Detection took [" << ms << "] ms and found [" << nFaces << "] objects" << std::endl;

    // Get the first detected face (the biggest).
    if (nFaces > 0)
        rc = *(CvRect*)cvGetSeqElem( rects, 0 );
    else
        rc = cvRect(-1,-1,-1,-1);   // Couldn't find the face.

    if (greyImg)
        cvReleaseImage( &greyImg );
    cvReleaseMemStorage( &storage );

    return rc;  // Return the biggest face found, or (-1,-1,-1,-1).
}

const std::string haarcascade_dir("../../system/share/OpenCV/haarcascades");

int main (int argc, char **argv)
{
    // Haar Cascade file, used for Face Detection.
    std::string faceCascadeFilename = haarcascade_dir + "/haarcascade_frontalface_alt.xml";
    // Load the HaarCascade classifier for face detection.
    CvHaarClassifierCascade* faceCascade;
    faceCascade = (CvHaarClassifierCascade*)cvLoad(faceCascadeFilename.c_str(), 0, 0, 0);
    if( !faceCascade ) {
        std::cout << "Couldnt load Face detector '" << faceCascadeFilename << "'"  << std::endl;
        exit(1);
    }

    std::string file("resource/people.jpg");

    IplImage* inputImg  = cvLoadImage(file.c_str(), 1);

    if(!inputImg)
    {
        std::cout << "Image not loaded" << std::endl;
        exit(1);
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
