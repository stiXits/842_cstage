#include <stdint.h>

bool assertArraysAreEqual(uint8_t* array0, uint8_t* array1, int size) {

    bool testResult = true;

    for(int i = 0; i < size; i++) {
        testResult &= array0[i] == array1[i];
    }

    return testResult;
}