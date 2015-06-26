#include <gtest/gtest.h>
#include <ForgingDetector.h>
#include <ForgingDetectorOld.h>
#include <utils.h>

#include <iostream>
#include <string>
#include "Timer.h"

const int BLOCK_SIZE = 16;
const std::string IMG_PATH("../copy-move-forgery/resource/publico.bmp");
const Bitmap BITMAP(IMG_PATH);
