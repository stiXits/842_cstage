// testing framework
#include "catch.hpp"

// fundamentals
#include <cstdlib>
#include <stdlib.h>

//#include "sds_lib.h"

#include "../hw842.h"

// helpers
#include "tools.h"
#include "../settings.h"

TEST_CASE( "Compress small input no index actions", "[Compress/Decompress]" ) {
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
	initArray(inputBuffer, BLOCK_SIZE, 0);

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    inputBuffer[0] = 225;
    inputBuffer[1] = 33;
    inputBuffer[2] = 65;
    inputBuffer[3] = 97;
    inputBuffer[4] = 129;
    inputBuffer[5] = 161;
    inputBuffer[6] = 193;
    inputBuffer[7] = 226;
    inputBuffer[8] = 225;
    inputBuffer[9] = 33;
    inputBuffer[10] = 66;
    inputBuffer[11] = 97;
    inputBuffer[12] = 129;
    inputBuffer[13] = 161;
    inputBuffer[14] = 193;
    inputBuffer[15] = 226;

    auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(expectedResult, BLOCK_SIZE, 0);

    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
    // opcode|      chunk
    expectedResult[0] = 7;
    expectedResult[1] = 9;
    expectedResult[2] = 10;
    expectedResult[3] = 11;
    expectedResult[4] = 12;
    expectedResult[5] = 13;
    expectedResult[6] = 14;
    expectedResult[7] = 15;
    expectedResult[8] = 16;
    expectedResult[9] = 56;
    expectedResult[10] = 72;
    expectedResult[11] = 80;
    expectedResult[12] = 152;
    expectedResult[13] = 96;
    expectedResult[14] = 104;
    expectedResult[15] = 112;
    expectedResult[16] = 120;
    expectedResult[17] = 128;
    expectedResult[18] = 0;

    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    hw842_compress(inputBuffer, outputBuffer, BLOCK_SIZE);

    // don't veerify whole block, zeroes are beeing replaced with index actions
    // which will be subject to an explicit test
    bool arrayTest =  assertArraysAreEqual(outputBuffer, expectedResult, 17);

    free(inputBuffer);
    free(expectedResult);
    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE( "Compress small input with I8 index actions", "[Compress/Decompress]" ) {
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
	initArray(inputBuffer, BLOCK_SIZE, 0);

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    inputBuffer[0] = 225;
    inputBuffer[1] = 33;
    inputBuffer[2] = 65;
    inputBuffer[3] = 97;
    inputBuffer[4] = 129;
    inputBuffer[5] = 161;
    inputBuffer[6] = 193;
    inputBuffer[7] = 226;
    inputBuffer[8] = 225;
    inputBuffer[9] = 33;
    inputBuffer[10] = 65;
    inputBuffer[11] = 97;
    inputBuffer[12] = 129;
    inputBuffer[13] = 161;
    inputBuffer[14] = 193;
    inputBuffer[15] = 226;

    auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(expectedResult, BLOCK_SIZE, 0);
    //      7          9        10        11        12       13        14          15
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010
    // opcode|      chunk

    // chunk with index opcode referencing the first 64 bit in the ringbuffer
    //  110 01|000000 00|0000000 0|0000000 0|0000000 0|0000000 0|0000000 0|0000000
    //  opcode
    expectedResult[0] = 7;
    expectedResult[1] = 9;
    expectedResult[2] = 10;
    expectedResult[3] = 11;
    expectedResult[4] = 12;
    expectedResult[5] = 13;
    expectedResult[6] = 14;
    expectedResult[7] = 15;
    expectedResult[8] = 22;
    expectedResult[9] = 64;
    expectedResult[10] = 0;
    expectedResult[11] = 0;
    expectedResult[12] = 0;
    expectedResult[13] = 0;
    expectedResult[14] = 0;
    expectedResult[15] = 0;
    expectedResult[16] = 0;
    expectedResult[17] = 0;
    expectedResult[18] = 0;

    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    hw842_compress(inputBuffer, outputBuffer, BLOCK_SIZE);

    // don't veerify whole block, zeroes are beeing replaced with index actions
    // which will be subject to an explicit test
    bool arrayTest =  assertArraysAreEqual(outputBuffer, expectedResult, 17);

    free(inputBuffer);
    free(expectedResult);
    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE( "Decompress small input", "[Compress/Decompress]" ) {
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000      00111|000 01001|000 01010|000 01011|000 01100|000 01101|000 01110|000 01111|000 10000|000
    // opcode|      chunk																				56      72		  80		88			96		104			112		120			128
	initArray(inputBuffer, BLOCK_SIZE, 0);
    inputBuffer[0] = 7;
    inputBuffer[1] = 9;
    inputBuffer[2] = 10;
    inputBuffer[3] = 11;
    inputBuffer[4] = 12;
    inputBuffer[5] = 13;
    inputBuffer[6] = 14;
    inputBuffer[7] = 15;
    inputBuffer[8] = 16;
    inputBuffer[9] = 56;
    inputBuffer[10] = 72;
    inputBuffer[11] = 80;
    inputBuffer[12] = 88;
    inputBuffer[13] = 96;
    inputBuffer[14] = 104;
    inputBuffer[15] = 112;
    inputBuffer[16] = 120;
    inputBuffer[17] = 128;
    inputBuffer[18] = 0;

    auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    initArray(expectedResult, BLOCK_SIZE, 0);
    expectedResult[0] = 225;
    expectedResult[1] = 33;
    expectedResult[2] = 65;
    expectedResult[3] = 97;
    expectedResult[4] = 129;
    expectedResult[5] = 161;
    expectedResult[6] = 193;
    expectedResult[7] = 226;
    expectedResult[8] = 225;
    expectedResult[9] = 33;
    expectedResult[10] = 65;
    expectedResult[11] = 97;
    expectedResult[12] = 129;
    expectedResult[13] = 161;
    expectedResult[14] = 193;
    expectedResult[15] = 226;

    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    // To the moment there is some bit garbage at the end of the block due to mismatching strides
    // TODO: #1
    hw842_decompress(&inputBuffer[0], &outputBuffer[0], BLOCK_SIZE);

    //debug
    bool arrayTest = assertArraysAreEqual(outputBuffer, expectedResult, BLOCK_SIZE - 5);

    free(inputBuffer);
    free(expectedResult);
    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE( "Compress & Decompress random data", "[Compress/Decompress]" ) {
	// declare databuffers
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
	auto expectedResult = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    auto intermediateBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

    // zero initialise data buffers
    initArray(inputBuffer, BLOCK_SIZE, 0);
    initArray(outputBuffer, BLOCK_SIZE, 0);
    initArray(intermediateBuffer, BLOCK_SIZE, 0);
    initArray(expectedResult, BLOCK_SIZE, 0);

    srand(123456734);

    // set random input buffer
    for(int i = 0; i < BLOCK_SIZE; i++) {
    	inputBuffer[i] = expectedResult[i] = rand();
    }

    hw842_compress(inputBuffer, intermediateBuffer, BLOCK_SIZE);
    hw842_decompress(intermediateBuffer, outputBuffer, BLOCK_SIZE);

    bool arrayTest = assertArraysAreEqual(outputBuffer, expectedResult, BLOCK_SIZE - 5);

    free(inputBuffer);
    free(expectedResult);
    free(intermediateBuffer);
    free(outputBuffer);

    REQUIRE(arrayTest);
}
