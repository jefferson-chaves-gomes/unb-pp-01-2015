#!/bin/bash

filesArray=(./resource/meter/*mb.bmp)
processArray=(2 4 6 8 10)

for i in "${processArray[@]}"
do
	echo
	echo " ----------------------------- OMP with $i Threads ----------------------------- "
	echo
	for f in "${filesArray[@]}"
	do
		#echo "./src/copy-move-forgery-omp/Debug/copy-move-forgery-omp $f 16 $i dynamic"
		./src/copy-move-forgery-omp/Debug/copy-move-forgery-omp $f 16 $i dynamic
	done
	echo
done

