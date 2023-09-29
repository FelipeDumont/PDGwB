// #include <iostream>
#include "TestDLL.h"
#include <random>
#include <algorithm>
#include <string.h>
#include <cmath> //absolute (abs())

extern "C" {
	int DLL_EXPORT SimpleTypeReturnFun(){
		return 10;
	}

	int DLL_EXPORT SimpleTypeArgFun(int n){
		return n * 2;
	}

	void DLL_EXPORT ReferenceArgumentFun(int &n){
		n = 25;
	}

	void DLL_EXPORT SimpleCallBackFun(SimpleCallback c){
		c();
	}
}

int main(int argc, char *argv[]) {
	return 0;
}
