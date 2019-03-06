#pragma once

#include "settings.h"

#include <ap_int.h>
#include <stdint.h>

struct outputChunkPointer {
    uint32_t byteIndex;

    // the offset points to within the first byte, thus 3 bits will be sufficient
    uint8_t offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(uint8_t increment)
    {
        uint8_t accu = offset + increment;
        byteIndex += accu / 8;
        offset = accu % 8;
    }

    uint32_t lsB() {
    	return byteIndex;
    }

    uint32_t msB() {
    	return byteIndex + 1;
    }
};

struct inputChunkPointer {

    // const
    uint32_t byteIndex;

    // the offset points to within the first byte, thus 3 bits will be sufficient
    uint8_t offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(uint8_t increment)
    {
        uint8_t accu = offset + increment;
        byteIndex += accu / 8;
        offset = accu % 8;
    }

    uint32_t lsB() {
    	return byteIndex;
    }

    uint32_t msB() {
    	return byteIndex + 1;
    }

};

struct chunk {
	ap_uint<8> data[CHUNK_SIZE];
    uint8_t opCode;
};

struct outputChunk {
	ap_uint<64> high, low;

	uint8_t offset;

	outputChunk(ap_uint<64> high, ap_uint<64> low, uint8_t offset) :offset(offset),
																	high(high),
																	low(low) {

	}

	outputChunk()  :high(0),
					low(0),
					offset(0){

	}
};

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
outputChunk appendCompressedChunk(const ap_uint<64> chunk, outputChunk writeHead);
void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const uint8_t &offset);

uint8_t readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input);

// compressed chunks are preceded by a 5 bit opcode, so they can't be addressed by a byte pointer, the chunkdata will
// be copied to the chunk data structure
void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk);

uint8_t extractOpcode(uint8_t offset, ap_uint<16> input);
