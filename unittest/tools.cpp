#include <stdint.h>

#include "ap_int.h"

bool assertArraysAreEqual(ap_uint<8> *toCheck, ap_uint<8> *reference, int size) {

    bool result = true;
    for(int i = 0; i < size; i++) {
    	if(reference[i] != toCheck[i]) {
    		std::cout<<"array segment " << i << " differs: output: "
    				 << toCheck[i] << " expected: " << reference[i] << std::endl;
    		result = false;
    	}
    }

    return result;
}

void initArray(ap_uint<8> *array, uint32_t length, ap_uint<8> initValue) {
	for(uint32_t i = 0; i < length; i++) {
		array[i] = initValue;
	}
}
