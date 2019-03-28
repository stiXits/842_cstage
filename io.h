#pragma once

#include "settings.h"

#include <ap_int.h>
#include <stdint.h>

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

struct chunk {
	ap_uint<8> data[CHUNK_SIZE];
    ap_uint<8> opCode;
};

struct outputChunk {
	ap_uint<64> high, low;

	ap_uint<8> offset;

	outputChunk(ap_uint<64> high, ap_uint<64> low, ap_uint<8> offset) :offset(offset),
																	high(high),
																	low(low) {

	}

	outputChunk()  :high(0),
					low(0),
					offset(0){

	}

	bool readyToExtract() {
		return offset >= 64;
	}

	ap_uint<64> extractLow() {
		ap_uint<64> toTransfer = low;

		low = high;
		high = 0;
		offset -= 64;

		return toTransfer;
	}
};

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
outputChunk appendCompressedChunk(const ap_uint<64> chunk, outputChunk writeHead);
outputChunk appendOpcode(const ap_uint<OPCODE_SIZE> opcode, outputChunk writeHead);
void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const ap_uint<8> &offset);

ap_uint<8> readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input);

// compressed chunks are preceded by a 5 bit opcode, so they can't be addressed by a byte pointer, the chunkdata will
// be copied to the chunk data structure
void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk);

ap_uint<8> extractOpcode(ap_uint<8> offset, ap_uint<16> input);
