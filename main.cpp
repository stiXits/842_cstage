#include <iostream>
#include "unittest/test.h"

int main(int argc, const char* argv[]){

    if(! runTests())
    {
        std::cout << "tests failed!";
        return 1;
    }

    std::cout << "All tests passes, YAAAYYY!";
    return 0;
}
