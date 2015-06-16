#!/bin/bash
SELF_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR=${SELF_DIR}

mkdir -p ${ROOT_DIR}/build

if [ "$1" != "-a" ]
then
	echo "Skipping Dependencies" 
else
	echo "Installing Dependencies"
	cd ${ROOT_DIR}/share/system

        ./build_gtest.sh            
fi

cd ${ROOT_DIR}