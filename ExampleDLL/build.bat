@echo off
gcc -c Astar.cpp 
gcc -shared -o Astar.dll Astar.o
echo AStart.dll  created ! ! !
pause
