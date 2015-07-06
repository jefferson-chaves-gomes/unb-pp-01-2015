#!/bin/bash

filesArray=(./resource/meter/*mb.bmp)

echo " ------------------------------------ Serial Process ----------------------------------- "
echo
for f in "${filesArray[@]}"
do
	#echo "./src/copy-move-forgery/Debug/copy-move-forgery $f 16"
	./src/copy-move-forgery/Debug/copy-move-forgery $f 16
done
echo


