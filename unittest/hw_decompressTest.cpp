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

TEST_CASE_METHOD(Fixture, "Decompress small input", "[Decompress]" ) {
	std::cout<<"Decompress small input"<<std::endl;
	auto inputBuffer = compressed;
    // adapt expected sequence to repetition free input
	inputBuffer[12] = 88;

    auto expectedResult = uncompressed;

    auto outputBuffer = (ap_uint<8>*) malloc(2 *BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    // To the moment there is some bit garbage at the end of the block due to mismatching strides
    // TODO: #1
    hw842_decompress(&inputBuffer[0], &outputBuffer[0], BLOCK_SIZE);

    //debug
    bool arrayTest = assertArraysAreEqual(outputBuffer, expectedResult, BLOCK_SIZE - 5);

    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE_METHOD(Fixture, "Decompress small input with I8 index actions", "[Decompress]" ) {
	std::cout<<"Decompress small input with I8 index actions"<<std::endl;
	auto inputBuffer = compressedWithIndex;

    auto expectedResult = uncompressed;

    auto outputBuffer = (ap_uint<8>*) malloc(2 *BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    // To the moment there is some bit garbage at the end of the block due to mismatching strides
    // TODO: #1
    hw842_decompress(&inputBuffer[0], &outputBuffer[0], BLOCK_SIZE);

    //debug
    bool arrayTest = assertArraysAreEqual(outputBuffer, expectedResult, BLOCK_SIZE - 5);

    free(outputBuffer);

    REQUIRE(arrayTest);
}
