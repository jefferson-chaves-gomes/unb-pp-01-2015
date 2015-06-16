#!/bin/bash


SELF_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GTEST_SRC_NAME=gtest-1.7.0
ROOT_DIR=${SELF_DIR}/../..

BUILD_DIR=${ROOT_DIR}/build/${GTEST_SRC_NAME}
GTEST_SRC_DIR=${BUILD_DIR}
DEST_DIR=${ROOT_DIR}/system
LOG_FILE=${ROOT_DIR}/build/build_gtest.log

cat /dev/null > ${LOG_FILE}

if [ ! -d ${ROOT_DIR}/build ]
  then mkdir -p ${ROOT_DIR}/build
fi

if [ -d ${BUILD_DIR} ]
  echo "Removing old Google Test installation"
  then rm -rf ${BUILD_DIR}
fi

cd ${ROOT_DIR}/build
echo "Uncompressing ${GTEST_SRC_NAME}"
tar -jxf ${SELF_DIR}/${GTEST_SRC_NAME}.tar.bz2
cd ${GTEST_SRC_NAME}

echo "Invoking Google Test's \"configure\" script"
MAKEFLAGS=-j4 ./configure \
            --prefix ${DEST_DIR} \
            --libdir ${DEST_DIR}/lib/gtest \
            --enable-shared \
	2>> ${LOG_FILE} >> ${LOG_FILE}

if [ -f Makefile ]
then
  echo "Starting the build process"
  if(make -j8 2>> ${LOG_FILE} >> ${LOG_FILE})
  then
  
     mkdir -p $DEST_DIR
     cd $DEST_DIR
     
     if [ -d ${DEST_DIR}/include/gtest ] ;then
        rm -rf $DEST_DIR/include/gtest
     fi
     
     if [ -d ${DEST_DIR}/lib/gtest ] ;then
        rm -rf $DEST_DIR/lib/gtest
     fi
     
     mkdir -p $DEST_DIR/include/gtest/
     mkdir -p $DEST_DIR/lib/gtest/
     
     cp -rf ${BUILD_DIR}/include/gtest/*    $DEST_DIR/include/gtest/
     cp -rf ${BUILD_DIR}/lib/.libs/*        $DEST_DIR/lib/gtest/

     echo "Google Test was built successfully. Cleaning up" && \
     rm -rf ${BUILD_DIR}
  fi
fi

cd ${SELF_DIR}