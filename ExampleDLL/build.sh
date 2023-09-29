#!/bin/bash

clear
g++ -fPIC -shared -o Astar.so AStar.cpp -lpthread 
g++ -fPIC -shared -o Astar.dll AStar.cpp -lpthread 
