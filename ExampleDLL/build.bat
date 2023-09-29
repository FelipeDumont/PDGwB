@echo off
gcc -c PDGwB.cpp 
gcc -shared -o PDGwB.dll PDGwB.o
echo AStart.dll  created ! ! !
pause
