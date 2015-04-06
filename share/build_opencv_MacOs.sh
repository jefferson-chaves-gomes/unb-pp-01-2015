#!/bin/bash

OPEN_CV_FILE_NAME=opencv-2.4.10

OPEN_CV_FILE=${OPEN_CV_FILE_NAME}.zip
SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

cd $SCRIPT_DIR

unzip $OPEN_CV_FILE -d $SCRIPT_DIR

cd $SCRIPT_DIR/$OPEN_CV_FILE_NAME

brew install cmake

cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$SCRIPT_DIR/../system && \
make && \
make install

rm -rf $OPEN_CV_DIR


