#!/bin/bash

filesArray=(./resource/meter/*mb.bmp)
processArray=(2 4 6 8 10)

for i in "${processArray[@]}"
do
	echo
	echo " ----------------------------- MPI with $i Process ----------------------------- "
	echo
	for f in "${filesArray[@]}"
	do
		#echo "mpirun -n $i src/copy-move-forgery-mpi/Debug/copy-move-forgery-mpi $f 16"
		mpirun -n $i src/copy-move-forgery-mpi/Debug/copy-move-forgery-mpi $f 16
	done
	echo
done

