#include "test.h"

#include <iostream>
#include "unittest/test.h"

int main(int argc, const char* argv[]){

    if(! runTests())
    {
        std::cout << "tests failed!";
        return 1;
    }

    return runTest(argc, argv);
}
