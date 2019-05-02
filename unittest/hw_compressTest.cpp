#include <cstdlib>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

//#include "sds_lib.h"

#include "../hw842.h"
#include "../settings.h"

bool test_hw842_compress_smallInput() {

//	void *inputMemory = malloc(BLOCK_SIZE * sizeof(ap_uint<8>) + sizeof(std::vector<ap_uint<8>>));
//	auto inputBuffer = *(new(inputMemory) std::vector<ap_uint<8>>(BLOCK_SIZE, 0));
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    inputBuffer[0] = 225;
    inputBuffer[1] = 33;
    inputBuffer[2] = 65;
    inputBuffer[3] = 97;
    inputBuffer[4] = 129;
    inputBuffer[5] = 161;
    inputBuffer[6] = 193;
    inputBuffer[7] = 226;


//	void *expectedResultMemory = malloc(BLOCK_SIZE * sizeof(ap_uint<8>) + sizeof(std::vector<ap_uint<8>>));
//	auto expectedResult = *(new(expectedResultMemory) std::vector<ap_uint<8>>(BLOCK_SIZE, 0));
    auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

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
    expectedResult[9] = 0;

//	void *outputBufferMemory = malloc(BLOCK_SIZE * sizeof(ap_uint<8>) + sizeof(std::vector<ap_uint<8>>));
//	auto outputBuffer = *(new(outputBufferMemory) std::vector<ap_uint<8>>(BLOCK_SIZE, 0));
    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

    for(int i = 0; i < 64; i++) {
    	outputBuffer[i] = 0;
    }

    hw842_compress(inputBuffer, outputBuffer, BLOCK_SIZE);

    bool result = true;
    for(int i = 0; i < 64; i++) {
    	if(outputBuffer[i] != expectedResult[i]) {
    		std::cout<<"array segments differ: output: "<<outputBuffer[i]<<" expected: "<<expectedResult[i]<<std::endl;
    		result = false;
    	}
    }
//    bool result = true;
//    bool result = std::equal(outputBuffer.begin(), outputBuffer.begin() + BLOCK_SIZE, expectedResult.begin());

    return result;
}

bool test_hw842_decompress_smallInput() {

	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
    // opcode|      chunk
	memset(inputBuffer, 0, sizeof(BLOCK_SIZE * sizeof(ap_uint<8>)));
    inputBuffer[0] = 7;
    inputBuffer[1] = 9;
    inputBuffer[2] = 10;
    inputBuffer[3] = 11;
    inputBuffer[4] = 12;
    inputBuffer[5] = 13;
    inputBuffer[6] = 14;
    inputBuffer[7] = 15;
    inputBuffer[8] = 16;

    auto expectedResult = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    memset(expectedResult, 0, BLOCK_SIZE * sizeof(ap_uint<8>));
    expectedResult[0] = 225;
    expectedResult[1] = 33;
    expectedResult[2] = 65;
    expectedResult[3] = 97;
    expectedResult[4] = 129;
    expectedResult[5] = 161;
    expectedResult[6] = 193;
    expectedResult[7] = 226;
    expectedResult[8] = 0;

    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    memset(outputBuffer, 0, BLOCK_SIZE * sizeof(ap_uint<8>));

    hw842_decompress(&inputBuffer[0], &outputBuffer[0], BLOCK_SIZE);

    bool result = true;
    for(int i = 0; i < 64; i++) {
    	if(outputBuffer[i] != expectedResult[i]) {
    		std::cout<<"array segment " << i << " differs: output: "<<outputBuffer[i]<<" expected: "<<expectedResult[i]<<std::endl;
    		result = false;
    	}
    }

    return result;
}

bool test_compress_decompress_withRandomData() {
	// declare databuffers
	auto inputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
	auto expectedResult = (ap_uint<8>*) malloc(2 * BLOCK_SIZE * sizeof(ap_uint<8>));
    auto intermediateBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));
    auto outputBuffer = (ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>));

    // zero initialise data buffers
    memset(inputBuffer, 0, BLOCK_SIZE * sizeof(ap_uint<8>));
    memset(outputBuffer, 0, BLOCK_SIZE * sizeof(ap_uint<8>));
    memset(intermediateBuffer, 0, BLOCK_SIZE * sizeof(ap_uint<8>));
    memset(expectedResult, 0, BLOCK_SIZE * sizeof(ap_uint<8>));

    srand (time(NULL));

    // set random input buffer
    for(int i = 0; i < BLOCK_SIZE; i++) {
    	inputBuffer[i] = expectedResult[i] = rand();
    }

    hw842_compress(inputBuffer, intermediateBuffer, BLOCK_SIZE);
    hw842_decompress(intermediateBuffer, outputBuffer, BLOCK_SIZE);

    bool result = true;
    for(int i = 0; i < 64; i++) {
    	if(expectedResult[i] != outputBuffer[i]) {
    		std::cout<<"array segment " << i << " differs: output: "<<outputBuffer[i]<<" expected: "<<expectedResult[i]<<std::endl;
    		result = false;
    	}
    }

    return result;
}

bool run_hw_compressTests() {
    return      test_hw842_compress_smallInput()
            &&  test_hw842_decompress_smallInput()
			&&	test_compress_decompress_withRandomData();
}
