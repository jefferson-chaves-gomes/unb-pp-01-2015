#!/bin/bash

echo
echo "STARTING...!"
echo
./meter-serial.sh
echo
./meter-mpi.sh
echo
./meter-omp.sh
echo
echo "FINISHED!"
echo
