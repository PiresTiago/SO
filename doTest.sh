#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Incorrect number of arguments"
    exit
fi
exec &> $2.speedups.csv
#maxThreads=$1
#input=$2

CircuitRouter-SeqSolver/CircuitRouter-SeqSolver $2

seqTime=$(grep Elapsed $2.res | grep -o -E '[0-9.]+')

echo \#threads,exec_time,speedup
echo 1S,$seqTime,1

for ((i=1;i<=$1;i++))
do
	CircuitRouter-ParSolver/CircuitRouter-ParSolver -t $i $2
	parTime=$(grep Elapsed $2.res | grep -o -E '[0-9.]+')
	speedup=$(echo "scale=6; ${seqTime}/${parTime}" | bc)
	echo $i,$parTime,$speedup
done
