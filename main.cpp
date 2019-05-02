#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <vector>

#include "settings.h"
#include "unittest/test.h"
//#define BLOCK_SIZE 32768
// #define BLOCK_SIZE 4096
#define STRLEN 4096

//#define USEHW


long long currentTimestamp() {
    return 1000000000;
}

int nextMultipleOfBlockSize(unsigned int input) {
    return (input + (BLOCK_SIZE-1)) & ~(BLOCK_SIZE-1);
}

int main(int argc, const char* argv[]){

    if(! runTests())
    {
        std::cout << "tests failed!";
        return 1;
    }
    else {
    	std::cout << "unittests successfull!";
    }

    std::cout << "All tests passed, YAAAYYY!";
    return 0;
}
