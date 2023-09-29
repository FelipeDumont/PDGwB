#include "AStar.h"

#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



using namespace std;




extern "C"{

	int EXPORT SimpleTypeReturnFun(){
		return 10;
	}

	int EXPORT SimpleTypeArgFun(int n){
		return n * 2;
	}

	void EXPORT ReferenceArgumentFun(int &n){
		n = 25;
	}

	void EXPORT SimpleCallBackFun(SimpleCallback c){
		c();
	}

	int EXPORT SimpleReturnCallbackFun(SimpleReturnCallback c){
		return c();
	}

	void EXPORT SimpleArgCallbackFun(SimpleArgCallback c){
		return c(6);
	}
	
	int EXPORT TestSum(){

	    
	    return 20;
	}
}


int main(int argc, char *argv[]) {
	
  	return 0;
}

