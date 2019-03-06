#include "io.h"
#include "ap_int.h"

#include "settings.h"

void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const uint8_t &offset){

    // copy source chunk
    uint8_t rightshifted = *source;
    uint8_t leftshifted = *source;

    // shift content behind existing content in destination0
    rightshifted = rightshifted >> offset;

    // copy overflowing bits to destination1 (the follow up byte)
    leftshifted = leftshifted << (8 - offset);

    // add zeropadded, shifted content to destination 0
    *destination0 = *destination0 | rightshifted;
    *destination1 = leftshifted;
}

outputChunk appendCompressedChunk(const ap_uint<64> chunk, outputChunk writeHead) {

	// fill high with chunk as far as possible
	writeHead.high = (	writeHead.high(CHUNK_SIZE_BITS - 1, CHUNK_SIZE_BITS - 1 - writeHead.offset),
						chunk(CHUNK_SIZE_BITS - 1, writeHead.offset));

	// fill overflowing bits into low
	writeHead.low =	(chunk(writeHead.offset, 0));

	// shift bits to the start of low
	writeHead.low <<= 64 - writeHead.offset;

	writeHead.offset += 64;

	return writeHead;
}

uint8_t readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input) {

    uint16_t input_readable = input;

	uint8_t start = 15 - readHead.offset;
	uint8_t end = 15 - readHead.offset - 7;

    // shift bytes, to align them
    uint8_t output = input(start, end);

    return output;
}

uint8_t extractOpcode(uint8_t offset, ap_uint<16> input) {

	uint8_t start = 15 - offset;
	uint8_t end = 15 - offset - 4;

    return input(start, end);
}

void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk) {

	ap_uint<8> lsB = *(input + readHead.byteIndex);
	ap_uint<8> msB = *(input + readHead.byteIndex + 1);
    outputChunk.opCode = extractOpcode(readHead.offset, (lsB,msB));
    readHead.increment(5);

    for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
        uint8_t byte = readNextCompressedByte(readHead, (input[readHead.lsB()], input[readHead.msB()]));
        *(outputChunk.data + byteIndex) = byte;
        readHead.increment(8);
    }
}
