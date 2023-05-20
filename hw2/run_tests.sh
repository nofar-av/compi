#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

if [ ! -f hw2 ]; then
	echo "hw2 executable not found. Please put it in the same directory as this script."
	exit 1
fi

for f in *.in; do
	echo "Running test ${f%.in}"
	./hw2 < $f | diff - ${f%.in}.out
	if [ $? -eq 0 ]; then
		printf "Test ${f%.in} ${GREEN}passed${NC}\n"
	else
		printf "Test ${f%.in} ${RED}failed${NC}\n"
	fi
done
