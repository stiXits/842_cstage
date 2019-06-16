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

TEST_CASE_METHOD(Fixture, "Compress small input no index actions", "[Compress]" ) {
	std::cout<<"Compress small input no index actions"<<std::endl;
	auto inputBuffer = uncompressed;

	// break 8-byte repetition, see Fixture
	inputBuffer[10] = 66;


    auto expectedResult = compressed;

    auto outputBuffer = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    hw842_compress(inputBuffer, outputBuffer, BLOCK_SIZE);

    // don't verify the whole block, zeroes are beeing replaced with index actions
    // which will be subject to an explicit test
    bool arrayTest =  assertArraysAreEqual(outputBuffer, expectedResult, 17);

    free(outputBuffer);

    REQUIRE(arrayTest);
}

TEST_CASE_METHOD(Fixture, "Compress small input with I8 index actions", "[Compress]" ) {

	std::cout<<"Compress small input with I8 index actions"<<std::endl;
	auto inputBuffer = uncompressed;

    auto expectedResult = compressedWithIndex;

    auto outputBuffer = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    initArray(outputBuffer, BLOCK_SIZE, 0);

    std::cout<<"compressing";
    hw842_compress(inputBuffer, outputBuffer, BLOCK_SIZE);
    std::cout<<"done";

    // don't veerify whole block, zeroes are beeing replaced with index actions
    // which will be subject to an explicit test
    bool arrayTest =  assertArraysAreEqual(outputBuffer, expectedResult, 17);

    free(outputBuffer);

    REQUIRE(arrayTest);
}
