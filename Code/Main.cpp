#include <iostream>
#include <vector>
using namespace std;

extern "C"
void ga(double *warray) {

    warray[0] = 64;
    cout << "Testing HERE" << endl;
}


int main() {
    cout << "Hello There stranger" << endl;
    return 0;
}
