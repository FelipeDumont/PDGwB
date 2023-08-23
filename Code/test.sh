#!/bin/bash

directory="../sf/PDGwB/Code"
clear

# Update the files in the Local Git Files

# Remove Old Files directory
if [ -d "$directory" ]; then 
 rm -f -R $directory
 fi
mkdir "$directory"

# Compile with the main file
# Executable file and then
# .so Compilation (shared library)
g++ -fPIC -O3 -o PDGwB Main.cpp 
g++ -fPIC -O3 -shared -o test.so Main.cpp 


# Move the current version to the Git location
files=`ls`
for eachFile in $files
do
 cp $eachFile "$directory"
done

# Finally run it locally with the showing of results
# with some data !!!
./PDGwB $0
