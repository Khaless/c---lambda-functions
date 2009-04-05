#!/bin/bash

OPT_LVL="0 1 2 3" 
RUNS=1000
RB="ruby1.8"

printf "" > plot.dat
for opt in $OPT_LVL; do
	printf "" > results-O$opt.csv 
	for ((i = 0; i < $RUNS; i++)); do
		../lambda-timing-O$opt >> results-O$opt.csv
	done
	printf "$opt " >> plot.dat
  ${RB} ./process.rb results-O${opt}.csv >> plot.dat
done

