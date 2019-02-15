#include "../src/io.h"

#include <cstdlib>

#include "tools.h"

bool test_extractOpcode_noOffset() {
    // binary: 111110000
    uint8_t lsB = 248;
    uint8_t msB = 0;

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    uint8_t opcode = extractOpcode(lsB, msB, readHead);

    // binary: 00011111
    return opcode == 31 && readHead.offset == 5;
}

bool test_xtractOpcode_offset() {
    // binary: 00111110
    uint8_t lsB = 62;
    uint8_t msB = 0;

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 2;

    uint8_t opcode = extractOpcode(lsB, msB, readHead);

    // binary: 00011111
    return opcode == 31 && readHead.offset == 7;
}

bool test_extractOpcode_overlapping() {
    // binary: 00000011 11100000 11111111
    uint8_t payload[] = {3, 224, 255};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 6;

    uint8_t opcode0 = extractOpcode(payload[0], payload[1], readHead);

    readHead.offset++;
    uint8_t opcode1 = extractOpcode(payload[1], payload[2], readHead);

    // binary: 00011111
    return opcode0 == 31 && opcode1 == 1 && readHead.offset == 1;
}

bool test_appendUncompressedByte_noOffset_lsB() {
    // 10101010
    const uint8_t source = 170;

    uint8_t lsB = 0;
    uint8_t msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 0);

    return lsB == 170 && msB == 0;
}

bool test_appendUncompressedByte_offset() {
    // 10101010
    const uint8_t source = 170;

    uint8_t lsB = 0;
    uint8_t msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 3);

    // destinations shoul be
    // 00010101 01000000

    return lsB == 21 && msB == 64;
}

bool test_appendUncompressedByte_offset_msB() {
    // 10101010
    const uint8_t source = 170;

    uint8_t lsB = 0;
    uint8_t msB = 0;


    appendUncompressedByte(&source, &lsB, &msB, 8);

    // destinations should be
    // 00000000 10101010

    return lsB == 0 && msB == 170;
}

bool test_readNextCompressedByte_noOffset() {

    // binary: 00000011 11100000 11111111
    uint8_t payload[] = {3, 224, 255};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    uint8_t output[3] = {};

    readNextCompressedByte(readHead, payload, &output[0]);
    readNextCompressedByte(readHead, payload, &output[1]);
    readNextCompressedByte(readHead, payload, &output[2]);

    return output[0] == 3 && output[1] == 224 && output[2] == 255;
}

bool test_readNextCompressedByte_offset() {

    // binary: 0000|0011 1110|0000 11111111
    uint8_t payload[] = {3, 224, 255};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 4;

    uint8_t output[2] = {};

    readNextCompressedByte(readHead, payload, &output[0]);
    readNextCompressedByte(readHead, payload, &output[1]);

    return output[0] == 62 && output[1] == 15;
}

bool test_readNextCompressedChunk_noOffset() {
    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
    // opcode|      chunk
    uint8_t payload[9] = {7, 9, 10, 11 , 12, 13, 14, 15, 16};

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    uint8_t expected_result[8] = {225, 33, 65, 97, 129, 161, 193, 226};

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
    uint8_t payload[10] = {0, 9, 10, 11 , 12, 13, 14, 15, 16, 16};

    //               00010010  00010100  00010110  00011000  00011010  00011100  00011110  00100000
    uint8_t expected_result[8] = {18, 20, 22, 24, 26, 28, 30, 32};

    inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 4;

    struct chunk chunk;

    readNextCompressedChunk(readHead, payload, chunk);

    return assertArraysAreEqual(expected_result, chunk.data, 8);
}

bool test_appendUncompressedChunk_noOffset() {

    //        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
    uint8_t payload[8] = {225, 33, 65, 97, 129, 161, 193, 226};

    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
    // opcode|      chunk
    uint8_t expectedResult[9] = {7, 9, 10, 11 , 12, 13, 14, 15, 16};

    uint8_t output[9] = {};

    outputChunkPointer writehead;
    writehead.byteIndex = output;
    writehead.offset = 0;

    uint8_t *outputPointer = output;
    appendUncompressedChunk(0, payload, writehead);

    return assertArraysAreEqual(output, expectedResult, 9);
}

bool test_appendUncompressedChunk_offset() {

    //               00010010  00010100  00010110  00011000  00011010  00011100  00011110  00100000
    uint8_t payload[8] = {18, 20, 22, 24, 26, 28, 30, 32};

    //   0000;0000 0|0001001 0|0001010 0|0001011 0|0001100 0|0001101 0|0001110 0|0001111 0|0010000  0|0000000
    // offset|opcode|     chunk                                                                      | next chunk
    uint8_t expectedResult[10] = {0, 9, 10, 11 , 12, 13, 14, 15, 16, 0};

    uint8_t output[4096] = {};

    outputChunkPointer writehead;
    writehead.byteIndex = output;
    writehead.offset = 4;

    appendUncompressedChunk(0, payload, writehead);

    return assertArraysAreEqual(output, expectedResult, 9)
            && writehead.byteIndex == output + 9
            && writehead.offset == 1;
}

bool run_IoTests() {
    return test_extractOpcode_noOffset()
           && test_xtractOpcode_offset()
           && test_extractOpcode_overlapping()

           && test_appendUncompressedByte_noOffset_lsB()
           && test_appendUncompressedByte_offset()
           && test_appendUncompressedByte_offset_msB()

           && test_readNextCompressedByte_noOffset()
           && test_readNextCompressedByte_offset()

           && test_readNextCompressedChunk_noOffset()
           && test_readNextCompressedChunk_offset()

           && test_appendUncompressedChunk_noOffset()
           && test_appendUncompressedChunk_offset();
}
