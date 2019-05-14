#include "ap_int.h"

bool assertArraysAreEqual(ap_uint<8> *toCheck, ap_uint<8> *reference, int size);
void initArray(ap_uint<8> *array, uint32_t length, ap_uint<8> initValue);
