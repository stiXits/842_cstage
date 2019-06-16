// testing framework
#include "../Catch2/single_include/catch2/catch.hpp"

// fundamentals
#include <cstdlib>
#include <stdlib.h>

//#include "sds_lib.h"

#include "../hw842.h"

// helpers
#include "tools.h"
#include "../settings.h"

#include "compressDecompressTestFixture.h"

TEST_CASE( "Compress & Decompress random data", "[Compress/Decompress]" ) {
	std::cout<< "Compress & Decompress random data"<<std::endl;
	// declare databuffers
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
	auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    auto intermediateBuffer = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

    // zero initialise data buffers
    initArray(inputBuffer, BLOCK_SIZE, 0);
    initArray(outputBuffer, BLOCK_SIZE, 0);
    initArray(intermediateBuffer, BLOCK_SIZE, 0);
    initArray(expectedResult, BLOCK_SIZE, 0);

    srand(234898);

    // set random input buffer
    for(int i = 0; i < BLOCK_SIZE; i++) {
    	inputBuffer[i] = expectedResult[i] = rand();
    }

    hw842_compress(inputBuffer, intermediateBuffer, BLOCK_SIZE);
    hw842_decompress(intermediateBuffer, outputBuffer, BLOCK_SIZE);

    // skip last byte, overflowhandling is not implemented, yet
    bool arrayTest = assertArraysAreEqual(outputBuffer, expectedResult, BLOCK_SIZE - 8);

    free(inputBuffer);
    free(expectedResult);
    free(intermediateBuffer);
    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE_METHOD(Fixture, "Compress & Decompress small input with I8 index actions", "[Compress/Decompress]" ) {
	std::cout<<"Compress & Decompress small input with I8 index actions"<<std::endl;
	auto inputBuffer = uncompressed;

    auto intermediateBuffer = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(intermediateBuffer, BLOCK_SIZE, 0);

    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    hw842_compress(inputBuffer, intermediateBuffer, BLOCK_SIZE);
    hw842_decompress(intermediateBuffer, outputBuffer, BLOCK_SIZE);

    // don't veerify whole block, zeroes are beeing replaced with index actions
    // which will be subject to an explicit test
    bool arrayTest =  assertArraysAreEqual(outputBuffer, inputBuffer, 16);

    free(intermediateBuffer);
    free(outputBuffer);

    REQUIRE(arrayTest);
}
