// testing framework
#include "catch.hpp"

// module to test
#include "../io.h"

// fundamentals
#include <stdint.h>
#include "ap_int.h"

// helpers
#include "tools.h"

TEST_CASE( "Append compressed chunk", "[IO]" ) {
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b1110000100100001010000010110000110000001101000011100000111100010;

    ap_uint<CHUNK_SIZE_BITS> high = 0b1110000100100001010000010110000110000001101000011100000111100010;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 0;

    appendWord(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

	REQUIRE(writeHead->low == low);
	REQUIRE(writeHead->high == high);
	REQUIRE(offsetti == CHUNK_SIZE_BITS);
}

TEST_CASE( "Append compressed chunk with offset", "[IO]" ) {
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b1110000100100001010000010110000110000001101000011100000111100010;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b1111111100001001000010100000101100001100000011010000111000001111;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0001000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0b1111111111111111111111111111111111111111111111111111111111111111;
    *offset = 5;

    appendWord(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

    REQUIRE(writeHead->low == low);
    REQUIRE(writeHead->high == high);
    REQUIRE(offsetti == CHUNK_SIZE_BITS + 5);
}

TEST_CASE( "Append compressed chunk low word", "[IO]" ) {
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b1110000100100001010000010110000110000001101000011100000111100010;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b1110000100100001010000010110000110000001101000011100000111100010;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = CHUNK_SIZE_BITS;

    appendWord(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == low;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == CHUNK_SIZE_BITS + CHUNK_SIZE_BITS;

	REQUIRE(writeHead->low == low);
	REQUIRE(writeHead->high == high);
	REQUIRE(offsetti == CHUNK_SIZE_BITS + CHUNK_SIZE_BITS);
}

TEST_CASE( "Append opcode with no offset", "[IO]" ) {
	auto payload = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b11111;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b1111100000000000000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 0;

	#pragma SDS async(1)
    appendOpcode(payload, writeHead, offset);
	#pragma SDS wait(1)

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == 0;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == OPCODE_SIZE;

    REQUIRE(writeHead->low == 0);
    REQUIRE(writeHead->high == high);
    REQUIRE(offsetti == OPCODE_SIZE);
}

TEST_CASE( "Append opcode with offset", "[IO]" ) {
	auto payload = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b11111;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000111110000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 10;

    appendOpcode(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == 0;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == 10 + OPCODE_SIZE;

    REQUIRE(writeHead->low == 0);
    REQUIRE(writeHead->high == high);
    REQUIRE(offsetti == 10 + OPCODE_SIZE);
}

TEST_CASE( "Append opcode with overlapping offset", "[IO]" ) {
	auto payload = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b11111;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000111;
    ap_uint<CHUNK_SIZE_BITS> low =  0b1100000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 61;

    appendOpcode(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == low;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == 61 + OPCODE_SIZE;

    REQUIRE(writeHead->low == low);
	REQUIRE(writeHead->high == high);
	REQUIRE(offsetti == 61 + OPCODE_SIZE);
}

TEST_CASE( "Append opcode chunk low word", "[IO]" ) {
	auto payload = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b11111;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b1111100000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = CHUNK_SIZE_BITS;

    appendOpcode(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == low;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == CHUNK_SIZE_BITS + OPCODE_SIZE;

    REQUIRE(writeHead->low == low);
    REQUIRE(writeHead->high == high);
    REQUIRE(offsetti == CHUNK_SIZE_BITS + OPCODE_SIZE);
}

TEST_CASE( "Append opcode with offset in low word", "[IO]" ) {
	auto payload = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b11111;
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000111110000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 74;

    appendOpcode(payload, writeHead, offset);

    uint64_t lowi = writeHead->low;
    uint64_t highi = writeHead->high;
    uint64_t offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == low;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == 74 + OPCODE_SIZE;

    REQUIRE(writeHead->low == low);
    REQUIRE(writeHead->high == high);
    REQUIRE(offsetti == 74 + OPCODE_SIZE);
}

TEST_CASE( "Append opcode and compressed", "[IO]" ) {
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto opcode = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	//          11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
	*payload = 0b1110000100100001010000010110000110000001101000011100000111100010;
	*opcode = 0b11111;

	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

    ap_uint<CHUNK_SIZE_BITS> high = 0b1111111100001001000010100000101100001100000011010000111000001111;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0001000000000000000000000000000000000000000000000000000000000000;

    auto writeHead = (outputChunk*) malloc(sizeof(outputChunk));
    writeHead->low = 0;
    writeHead->high = 0;
    *offset = 0;

	#pragma SDS async(3)
	appendOpcode(opcode, writeHead, offset);
	#pragma SDS wait(3)

	uint64_t lowi = writeHead->low;
	uint64_t highi = writeHead->high;
	uint64_t offsetti = *offset;

	#pragma SDS async(2)
	appendWord(payload, writeHead, offset);
	#pragma SDS wait(2)

    lowi = writeHead->low;
    highi = writeHead->high;
    offsetti = *offset;

//    debug
//    bool lowtest = writeHead->low == low;
//    bool highTest = writeHead->high == high;
//	  bool offsetTest = offsetti == CHUNK_SIZE_BITS + 5;

	REQUIRE(writeHead->low == low);
	REQUIRE(writeHead->high == high);
	REQUIRE(offsetti == CHUNK_SIZE_BITS + 5);
}

TEST_CASE( "Read compressed chunk with no offset", "[IO]" ) {
	auto payload = (ap_uint<64>*) malloc(2 * sizeof(ap_uint<64>));
	auto chunk = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto opcode = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

	//    11111|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
	payload[0] = 0b1111111100001001000010100000101100001100000011010000111000001111;
	payload[1] = 0b0001000000000000000000000000000000000000000000000000000000000000;
	*opcode = 0;
	*offset = 0;
	*chunk = 0;

	ap_uint<64> expectedResult = 0b1110000100100001010000010110000110000001101000011100000111100010;
	readCompressedChunk(payload, chunk, opcode, offset);

	// debug
	uint64_t chunki = *chunk;
	uint8_t offsetti = *offset;
	uint8_t opcodei = *opcode;

//	  debug
//    bool chunkTest = *chunk == expectedResult;
//    bool opcodeTest = *opcode == 0b11111;
//	  bool offsetTest = *offset == OPCODE_SIZE;

	REQUIRE(*chunk == expectedResult);
	REQUIRE(*opcode == 0b11111);
	REQUIRE(*offset == OPCODE_SIZE);
}

TEST_CASE( "Read compressed chunk with offset", "[IO]" ) {
	auto payload = (ap_uint<64>*) malloc(2 * sizeof(ap_uint<64>));
	auto chunk = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto opcode = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

	//    11111|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
	payload[0] = 0b1111111111111100001001000010100000101100001100000011010000111000;
	payload[1] = 0b0011110001000000000000000000000000000000000000000000000000000000;
	*opcode = 0;
	*offset = 6;
	*chunk = 0;

	ap_uint<64> expectedResult = 0b1110000100100001010000010110000110000001101000011100000111100010;
	readCompressedChunk(payload, chunk, opcode, offset);

	// debug
	uint64_t chunki = *chunk;
	uint8_t offsetti = *offset;
	uint8_t opcodei = *opcode;

//	  debug
    bool chunkTest = *chunk == expectedResult;
    bool opcodeTest = *opcode == 0b11111;
	bool offsetTest = *offset == 6 + OPCODE_SIZE;

	REQUIRE(*chunk == expectedResult);
	REQUIRE(*opcode == 0b11111);
	REQUIRE(*offset == 6 + OPCODE_SIZE);
}

TEST_CASE( "Read compressed chunk with large offset", "[IO]" ) {
	auto payload = (ap_uint<64>*) malloc(2 * sizeof(ap_uint<64>));
	auto chunk = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto opcode = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

	//    11111|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
	payload[0] = 0b1111111111111100001001000010100000101100001100000011010000111011;
	payload[1] = 0b1110000100100001010000010110000110000001101000011100000111100010;
	*opcode = 0;
	*offset = 64 - OPCODE_SIZE;
	*chunk = 0;

	ap_uint<64> expectedResult = 0b1110000100100001010000010110000110000001101000011100000111100010;
	readCompressedChunk(payload, chunk, opcode, offset);

	// debug
	uint64_t chunki = *chunk;
	uint8_t offsetti = *offset;
	uint8_t opcodei = *opcode;

//	  debug
//    bool chunkTest = *chunk == expectedResult;
//    bool opcodeTest = *opcode == 0b11111;
//	  bool offsetTest = *offset == 64;

    REQUIRE(*chunk == expectedResult);
    REQUIRE(*opcode == 0b11011);
    REQUIRE(*offset == 64);
}
