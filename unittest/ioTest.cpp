#include "../io.h"

#include <cstdlib>
#include <stdint.h>

#include "ap_int.h"
#include "tools.h"

bool test_extractOpcode_noOffset() {
    // binary: 11111000 00000000
	ap_uint<16> payload = 63488;

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    ap_uint<8> opcode = extractOpcode(0, payload);

    // binary: 00011111
    return opcode == 31;
}

bool test_extractOpcode_offset() {
    // binary: 00111110 00000000
    ap_uint<16> payload = 15872;

    ap_uint<8> opcode = extractOpcode(2, payload);

    // binary: 00011111
    return opcode == 31;
}

bool test_extractOpcode_overlapping() {
    // binary: 00000011 11100000
    ap_uint<16> payload = 992;

    ap_uint<8> opcode0 = extractOpcode(6, payload);
    ap_uint<8> opcode1 = extractOpcode(11, payload);

    // binary: 00011111
    return opcode0 == 31 && opcode1 == 0;
}

bool test_appendUncompressedByte_noOffset_lsB() {
    // 10101010
    const ap_uint<8> source = 170;

    ap_uint<8> lsB = 0;
    ap_uint<8> msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 0);

    return lsB == 170 && msB == 0;
}

bool test_appendUncompressedByte_offset() {
    // 10101010
    const ap_uint<8> source = 170;

    ap_uint<8> lsB = 0;
    ap_uint<8> msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 3);

    // destinations shoul be
    // 00010101 01000000

    return lsB == 21 && msB == 64;
}

bool test_appendUncompressedByte_offset_msB() {
    // 10101010
    const ap_uint<8> source = 170;

    ap_uint<8> lsB = 0;
    ap_uint<8> msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 8);

    // destinations should be
    // 00000000 10101010

    return lsB == 0 && msB == 170;
}

bool test_readNextCompressedByte_noOffset() {

    // binary: 11000000 11100000 1111111
    ap_uint<8> payload[] = {192, 224, 255, 192};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    ap_uint<8> output0 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment(8);
    ap_uint<8> output1 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment(8);
    ap_uint<8> output2 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment(8);
    ap_uint<8> output3 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment(8);

    return output0 == 192 && output1 == 224 && output2 == 255;
}

bool test_readNextCompressedByte_offset() {

    // binary: 0000|0011 1110|0000 1111|1111 1100|0000
    ap_uint<8> payload[] = {3, 224, 255, 192};


    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 4;

    ap_uint<8> output0 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment();
    ap_uint<8> output1 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment();
    ap_uint<8> output2 = readNextCompressedByte(readHead, (payload[readHead.lsB()], payload[readHead.msB()]));
    readHead.increment();

    return output0 == 62 && output1 == 15 && output2 == 252;
}

bool test_appendCompressedChunk_noOffset() {

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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == CHUNK_SIZE_BITS;

    return lowtest && highTest && offsetTest;
}

bool test_appendCompressedChunk_offset() {
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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == CHUNK_SIZE_BITS + 5;

    return lowtest && highTest && offsetTest;
}

bool test_appendCompressedChunk_onlyLow() {
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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == CHUNK_SIZE_BITS + CHUNK_SIZE_BITS;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcode_noOffset() {

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

    bool lowtest = writeHead->low == 0;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == OPCODE_SIZE;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcode_offset() {

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

    bool lowtest = writeHead->low == 0;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == 10 + OPCODE_SIZE;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcode_overlappingOffset() {

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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == 61 + OPCODE_SIZE;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcode_onlyLow() {
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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == CHUNK_SIZE_BITS + OPCODE_SIZE;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcode_offsetLow() {
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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == 74 + OPCODE_SIZE;

    return lowtest && highTest && offsetTest;
}

bool test_appendOpcodeAndChunk() {
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

    bool lowtest = writeHead->low == low;
    bool highTest = writeHead->high == high;
	bool offsetTest = offsetti == CHUNK_SIZE_BITS + 5;


    return lowtest && highTest && offsetTest;
}

//bool test_readNextCompressedChunk_noOffset() {
//    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
//    // opcode|      chunk
//    ap_uint<8> payload[10] = {7, 9, 10, 11 , 12, 13, 14, 15, 16, 0};
//
//    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
//    ap_uint<8> expected_result[8] = {225, 33, 65, 97, 129, 161, 193, 226};
//
//    inputChunkPointer readHead;
//    readHead.byteIndex = 0;
//    readHead.offset = 0;
//
//    struct chunk chunk;
//
//    readNextCompressedChunk(readHead, payload, chunk);
//
//    return assertArraysAreEqual(expected_result, chunk.data, 8);
//}

//bool test_readNextCompressedChunk_offset() {
//    //   0000;0000 0|0001001 0|0001010 0|0001011 0|0001100 0|0001101 0|0001110 0|0001111 0|0010000  0|0010000
//    // offset|opcode|     chunk
//    ap_uint<8> payload[10] = {0, 9, 10, 11 , 12, 13, 14, 15, 16, 16};
//
//    //               00010010  00010100  00010110  00011000  00011010  00011100  00011110  00100000
//    ap_uint<8> expected_result[8] = {18, 20, 22, 24, 26, 28, 30, 32};
//
//    inputChunkPointer readHead;
//    readHead.byteIndex = 0;
//    readHead.offset = 4;
//
//    struct chunk chunk;
//
//    readNextCompressedChunk(readHead, payload, chunk);
//
//    return assertArraysAreEqual(expected_result, chunk.data, 8);
//}

bool test_readCompressedChunk_noOffset() {
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

    bool chunkTest = *chunk == expectedResult;
    bool opcodeTest = *opcode == 0b11111;
	bool offsetTest = *offset == OPCODE_SIZE;


    return chunkTest && opcodeTest && offsetTest;

}

bool test_readCompressedChunk_offset() {
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

    bool chunkTest = *chunk == expectedResult;
    bool opcodeTest = *opcode == 0b11111;
	bool offsetTest = *offset == 6 + OPCODE_SIZE;


    return chunkTest && opcodeTest && offsetTest;

}

bool test_readCompressedChunk_largeOffset() {
	auto payload = (ap_uint<64>*) malloc(2 * sizeof(ap_uint<64>));
	auto chunk = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto opcode = (ap_uint<OPCODE_SIZE>*) malloc(sizeof(ap_uint<OPCODE_SIZE>));
	auto offset = (uint8_t*) malloc(sizeof(uint8_t));

	//    11111|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
	payload[0] = 0b1111111111111100001001000010100000101100001100000011010000111111;
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

    bool chunkTest = *chunk == expectedResult;
    bool opcodeTest = *opcode == 0b11111;
	bool offsetTest = *offset == 64;

    return chunkTest && opcodeTest && offsetTest;
}

bool run_IoTests() {
    return test_extractOpcode_noOffset()
           && test_extractOpcode_offset()
           && test_extractOpcode_overlapping()

           && test_appendUncompressedByte_noOffset_lsB()
           && test_appendUncompressedByte_offset()
           && test_appendUncompressedByte_offset_msB()

           && test_readNextCompressedByte_noOffset()
           && test_readNextCompressedByte_offset()

//           && test_readNextCompressedChunk_noOffset()
//           && test_readNextCompressedChunk_offset()

           && test_appendCompressedChunk_noOffset()
           && test_appendCompressedChunk_offset()
		   && test_appendCompressedChunk_onlyLow()

		   && test_appendOpcode_noOffset()
		   && test_appendOpcode_offset()
		   && test_appendOpcode_overlappingOffset()
		   && test_appendOpcode_onlyLow()
		   && test_appendOpcode_offsetLow()
		   && test_appendOpcodeAndChunk()

		   && test_readCompressedChunk_noOffset()
		   && test_readCompressedChunk_offset();

}
