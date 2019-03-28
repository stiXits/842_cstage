#include "../io.h"

#include <cstdlib>

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

bool test_readNextCompressedChunk_noOffset() {
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000|000
    // opcode|      chunk
    ap_uint<8> payload[10] = {7, 9, 10, 11 , 12, 13, 14, 15, 16, 0};

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    ap_uint<8> expected_result[8] = {225, 33, 65, 97, 129, 161, 193, 226};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    struct chunk chunk;

    readNextCompressedChunk(readHead, payload, chunk);

    return assertArraysAreEqual(expected_result, chunk.data, 8);
}

bool test_readNextCompressedChunk_offset() {
    //   0000;0000 0|0001001 0|0001010 0|0001011 0|0001100 0|0001101 0|0001110 0|0001111 0|0010000  0|0010000
    // offset|opcode|     chunk
    ap_uint<8> payload[10] = {0, 9, 10, 11 , 12, 13, 14, 15, 16, 16};

    //               00010010  00010100  00010110  00011000  00011010  00011100  00011110  00100000
    ap_uint<8> expected_result[8] = {18, 20, 22, 24, 26, 28, 30, 32};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 4;

    struct chunk chunk;

    readNextCompressedChunk(readHead, payload, chunk);

    return assertArraysAreEqual(expected_result, chunk.data, 8);
}

bool test_appendCompressedChunk_noOffset() {

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    const ap_uint<CHUNK_SIZE_BITS> payload = 0b1110000100100001010000010110000110000001101000011100000111100010;

    ap_uint<CHUNK_SIZE_BITS> high = 0b1110000100100001010000010110000110000001101000011100000111100010;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;

    outputChunk result;
    appendCompressedChunk(&result, payload, writeHead);

    return result.low == low && result.high == high && result.offset == CHUNK_SIZE_BITS;
}

bool test_appendCompressedChunk_offset() {
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    const ap_uint<CHUNK_SIZE_BITS> payload = 0b1110000100100001010000010110000110000001101000011100000111100010;

    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    // opcode|      chunk
    ap_uint<CHUNK_SIZE_BITS> high = 0b0000011100001001000010100000101100001100000011010000111000001111;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0001000000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = 5;

    outputChunk result;
    appendCompressedChunk(&result, payload, writeHead);

    return result.low == low && result.high == high && result.offset == CHUNK_SIZE_BITS + 5;
}

bool test_appendCompressedChunk_onlyLow() {
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    const ap_uint<CHUNK_SIZE_BITS> payload = 0b1110000100100001010000010110000110000001101000011100000111100010;

    ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
    ap_uint<CHUNK_SIZE_BITS> low =  0b1110000100100001010000010110000110000001101000011100000111100010;

    struct outputChunk writeHead;
    writeHead.offset = CHUNK_SIZE_BITS;

    outputChunk result;
    appendCompressedChunk(&result, payload, writeHead);

    return result.low == low && result.high == high && result.offset == CHUNK_SIZE_BITS + CHUNK_SIZE_BITS;
}

bool test_appendOpcode_noOffset() {

	ap_uint<OPCODE_SIZE> opcode= 0b11111;
	ap_uint<CHUNK_SIZE_BITS> high = 0b1111100000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = 0;

    struct outputChunk result;
    appendOpcode(&result, opcode, writeHead);

    return result.high == high && result.low == 0 && result.offset == OPCODE_SIZE;
}

bool test_appendOpcode_offset() {

	ap_uint<OPCODE_SIZE> opcode= 0b11111;
	ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000111110000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = 10;

    struct outputChunk result;
    appendOpcode(&result, opcode, writeHead);

    return result.high == high && result.low == 0 && result.offset == 10 + OPCODE_SIZE;
}

bool test_appendOpcode_overlappingOffset() {

	ap_uint<OPCODE_SIZE> opcode= 0b11111;
	ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000111;
	ap_uint<CHUNK_SIZE_BITS> low =  0b1100000000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = 61;

    struct outputChunk result;
    appendOpcode(&result, opcode, writeHead);

    return result.high == high && result.low == low && result.offset == 61 + OPCODE_SIZE;
}

bool test_appendOpcode_onlyLow() {

	ap_uint<OPCODE_SIZE> opcode= 0b11111;
	ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
	ap_uint<CHUNK_SIZE_BITS> low =  0b1111100000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = CHUNK_SIZE_BITS;

    struct outputChunk result;
    appendOpcode(&result, opcode, writeHead);

    return result.high == high && result.low == low && result.offset == CHUNK_SIZE_BITS + OPCODE_SIZE;
}

bool test_appendOpcode_offsetLow() {

	ap_uint<OPCODE_SIZE> opcode= 0b11111;
	ap_uint<CHUNK_SIZE_BITS> high = 0b0000000000000000000000000000000000000000000000000000000000000000;
	ap_uint<CHUNK_SIZE_BITS> low =  0b0000000000111110000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;
    writeHead.offset = 74;

    struct outputChunk result;
    appendOpcode(&result, opcode, writeHead);

    return result.high == high && result.low == low && result.offset == 74 + OPCODE_SIZE;
}

bool test_appendOpcodeAndChunk() {
    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    const ap_uint<CHUNK_SIZE_BITS> payload = 0b1110000100100001010000010110000110000001101000011100000111100010;

    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    // opcode|      chunk
    ap_uint<CHUNK_SIZE_BITS> high = 0b0000011100001001000010100000101100001100000011010000111000001111;
    ap_uint<CHUNK_SIZE_BITS> low =  0b0001000000000000000000000000000000000000000000000000000000000000;

    struct outputChunk writeHead;

    appendOpcode(&writeHead, 0, writeHead);
    appendCompressedChunk(&writeHead, payload, writeHead);

    return writeHead.low == low && writeHead.high == high && writeHead.offset == CHUNK_SIZE_BITS + 5;
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

           && test_readNextCompressedChunk_noOffset()
           && test_readNextCompressedChunk_offset()

           && test_appendCompressedChunk_noOffset()
           && test_appendCompressedChunk_offset()
		   && test_appendCompressedChunk_onlyLow()

		   && test_appendOpcode_noOffset()
		   && test_appendOpcode_offset()
		   && test_appendOpcode_overlappingOffset()
		   && test_appendOpcode_onlyLow()
		   && test_appendOpcode_offsetLow()
		   && test_appendOpcodeAndChunk();

}
