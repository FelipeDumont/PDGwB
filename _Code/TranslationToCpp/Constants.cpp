#include "Constants.h"
#include<iostream>

int Constants::ID = 0;
int Constants::MAX_DEPTH = 10;
int Constants::POP_SIZE = 100;
int Constants::GENERATIONS = 150;
int Constants::nV = 1;
int Constants::nK = 1;
int Constants::nL = 1;
float Constants::lCoef = 1;

int Constants::seed = 100;
std::mt19937 Constants::randGen = std::mt19937(seed);
std::uniform_int_distribution<int> Constants::prob100 = std::uniform_int_distribution<int>(0,99);   // 0 and 99 inclusive
std::uniform_int_distribution<int> Constants::prob101 = std::uniform_int_distribution<int>(0,100);  // 0 and 100 inclusive
std::uniform_int_distribution<int> Constants::prob3 = std::uniform_int_distribution<int>(0,2); 		// 0 and 2 inclusive
std::uniform_int_distribution<int> Constants::prob1To3 = std::uniform_int_distribution<int>(1,2); 		// 0 and 2 inclusive
std::vector<int> Constants::randomSequence;
bool Constants::isForced = false;
std::vector<int> Constants::forcedRandomSequence;
int currentValue = 0;

int Constants::getNextId(){
	return ID++;
}

int Constants::SelectRandom(int value){
	randomSequence.push_back(value);
	if(isForced){
		value =forcedRandomSequence[currentValue];
		currentValue += 1;
	}
	// std::cout << randomSequence.size() << ": " << value << "\n";
	return value;
}

int Constants::Next100(){
	int result = prob100(Constants::randGen);
	result = SelectRandom(result);
	return result;
}


int Constants::Next101(){
	int result = prob101(Constants::randGen);
	result = SelectRandom(result);
	return result;
}

int Constants::Next3(){
	int result = prob3(Constants::randGen);
	result = SelectRandom(result);
	return result;
}

int Constants::Next1To3(){
	int result = prob1To3(Constants::randGen);
	result = SelectRandom(result);
	return result;
}
	
