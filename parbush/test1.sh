#!/bin/bash

dorun() {
	echo "starting dorun $np"
	mpirun -machinefile $HOME/mpi32 -np $1  /home/hines/neuron/mpi*6/i686/bin/nrniv batch_.hoc >& temp
	sort -k 1n,1n -k 2n,2n out.dat > out.$1
	mv temp stdout.$1
	echo "completed dorun $np"
}

for np in 25 20 10 5 2 1 ; do
	dorun $np
done
