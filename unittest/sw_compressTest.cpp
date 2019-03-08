#include <cstdlib>
#include <vector>
#include "../sw842.h"

#include "tools.h"

#include "../settings.h"

bool test_sw842_compress_smallInput() {

	std::vector<ap_uint<8>> inputBuffer(BLOCK_SIZE);

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    inputBuffer[0] = 225;
    inputBuffer[1] = 33;
    inputBuffer[2] = 65;
    inputBuffer[3] = 97;
    inputBuffer[4] = 129;
    inputBuffer[5] = 161;
    inputBuffer[6] = 193;
    inputBuffer[7] = 226;

    std::vector<ap_uint<8>> expectedResult(BLOCK_SIZE);

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

    std::vector<ap_uint<8>> outputBuffer(BLOCK_SIZE);

    sw842_compress(&inputBuffer[0], &outputBuffer[0], 3007);

    bool result = std::equal(outputBuffer.begin(), outputBuffer.begin() + 3007, expectedResult.begin());

    return result;
}

bool test_sw842_decompress_smallInput() {

	std::vector<ap_uint<8>> inputBuffer(BLOCK_SIZE);
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
    // opcode|      chunk
    inputBuffer[0] = 7;
    inputBuffer[1] = 9;
    inputBuffer[2] = 10;
    inputBuffer[3] = 11;
    inputBuffer[4] = 12;
    inputBuffer[5] = 13;
    inputBuffer[6] = 14;
    inputBuffer[7] = 15;
    inputBuffer[8] = 16;

    std::vector<ap_uint<8>> expectedResult(BLOCK_SIZE);
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    expectedResult[0] = 225;
    expectedResult[1] = 33;
    expectedResult[2] = 65;
    expectedResult[3] = 97;
    expectedResult[4] = 129;
    expectedResult[5] = 161;
    expectedResult[6] = 193;
    expectedResult[7] = 226;
    expectedResult[8] = 0;

    std::vector<ap_uint<8>> outputBuffer(BLOCK_SIZE);

    sw842_decompress(&inputBuffer[0], &outputBuffer[0], 3007);

    bool result = std::equal(outputBuffer.begin(), outputBuffer.begin() + 3007, expectedResult.begin());

    return result;
}

bool run_sw_compressTests() {
    return      test_sw842_compress_smallInput()
            &&  test_sw842_decompress_smallInput();
}
