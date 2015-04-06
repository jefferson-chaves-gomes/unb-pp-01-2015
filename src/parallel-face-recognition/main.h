
#ifndef MAIN_H_
#define MAIN_H_

#include <string>
#include "utils.h"

const std::string haarcascade_dir("../../system/share/OpenCV/haarcascades");

// Haar Cascade file, used for Face Detection.
const std::string faceCascadeFilenameAlt = haarcascade_dir + "/haarcascade_frontalface_alt.xml";
const std::string faceCascadeFilenameDefault = haarcascade_dir + "/haarcascade_frontalface_default.xml";
const std::string image_file("resource/image2.jpg");

void DetectMyFace1 (std::string const& imgFile);
void DetectMyFace2 (std::string const& imgFile);

#endif /* MAIN_H_ */
