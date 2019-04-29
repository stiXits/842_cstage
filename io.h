#pragma once

#include "settings.h"

#include <stdint.h>

#include "ap_int.h"
#include "hls_math.h"

#define CHUNK_START (CHUNK_SIZE_BITS - 1)

struct outputChunkPointer {
    uint32_t byteIndex;

    // the offset points to within the first byte, thus 3 bits will be sufficient
    ap_uint<8> offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(ap_uint<8> increment)
    {
        ap_uint<8> accu = offset + increment;
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
    ap_uint<8> offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(ap_uint<8> increment)
    {
        ap_uint<8> accu = offset + increment;
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

struct outputChunk {
	ap_uint<64> high, low;
	uint8_t offset;

	outputChunk(ap_uint<64> high, ap_uint<64> low, ap_uint<8> offset): high(high),
																		low(low),
																		offset(offset){

	}

	outputChunk()  :high(0),
					low(0),
					offset(0){

	}

	bool readyToExtract() {
		return offset >= 64;
	}

	ap_uint<64> extractHigh() {
		ap_uint<64> toTransfer = high;

		high = low;
		high = 0;
		offset -= 64;

		return toTransfer;
	}
};

//uint8_t appendChunk(ap_uint<CHUNK_SIZE_BITS> *opcode, ap_uint<CHUNK_SIZE_BITS> *chunkPointer, outputChunk *writeHead);

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
void appendWord(ap_uint<64> *chunkPointer, outputChunk *writeHead, uint8_t *offset);

void appendOpcode(ap_uint<OPCODE_SIZE> *opcodePointer, outputChunk *writeHead, uint8_t *offset);
void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const ap_uint<8> &offset);

ap_uint<8> readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input);

// compressed chunks are preceded by a 5 bit opcode, so they can't be addressed by a byte pointer, the chunkdata will
// be copied to the chunk data structure
//void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk);

ap_uint<8> extractOpcode(ap_uint<8> offset, ap_uint<16> input);
void extractAlignedData(outputChunk *returnValue, outputChunk chunk, ap_uint<8> out[BLOCK_SIZE], uint32_t outputIterator);
