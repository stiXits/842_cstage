#include <stdint.h>

#include "ap_int.h"

//bool assertArraysAreEqual(uint8_t* array0, uint8_t* array1, int size) {
//
//    bool testResult = true;
//
//    for(int i = 0; i < size; i++) {
//        testResult &= array0[i] == array1[i];
//    }
//
//    return testResult;
//}

bool assertArraysAreEqual(ap_uint<8>* array0, ap_uint<8>* array1, int size) {

    bool testResult = true;

    for(int i = 0; i < size; i++) {
        testResult &= array0[i] == array1[i];
    }

    return testResult;
}
