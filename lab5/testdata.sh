#!/bin/bash

DATA=$1

# Check argument is right 
if [ ! -f "$DATA" ]; then
    echo "The data is not found, please check the argument for this script"
    echo "e.g. ./testdata.sh <Your Data>"
    exit 1
fi

echo "Delete input"
rm $PWD/input

# mkfifo input
mkfifo input

# link data to input & run 
cat $DATA > input & ./histogram


echo "Program finish"
name=$(echo $DATA |  cut -d/ -f 5 | cut -d- -f 2)


# diff output
echo "Start diff"
diff ./0556148.out  /home/data/OpenCL/ReferenceOutput/output-$name
echo "diff finish"
