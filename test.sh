#!/bin/bash

clear

# Clean and make the new File
make clean
make

# Test and play
echo "------------------------------------------------------------------"
echo "./PDGWB"
./PDGwB
# Update the files in the Local Git Files
# Remove Old Files
rm ../sf/PDGwB/*
# Replace them ???
files=`ls`
for eachFile in $files
do
 cp $eachFile ../sf/PDGwB/
done
