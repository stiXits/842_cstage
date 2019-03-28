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

	ap_uint<64> extractHigh() {
		ap_uint<64> toTransfer = high;

		high = low;
		high = 0;
		offset -= 64;

		return toTransfer;
	}
};

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
void appendCompressedChunk(outputChunk *returnValue, const ap_uint<64> chunk, outputChunk writeHead) {
	// how many opcode bits fit into high?
	int numberOfBitsInHigh = CHUNK_SIZE_BITS - writeHead.offset;
	if(numberOfBitsInHigh < 0) {
		numberOfBitsInHigh = 0;
	}
	int numberOfBitsInLow = CHUNK_SIZE_BITS - numberOfBitsInHigh;

	if(numberOfBitsInHigh > 0) {
		// fill high with chunk as far as possible
		if(writeHead.offset == 0) {
			writeHead.high = chunk;
		}
		else {
			ap_uint<64> remainder = writeHead.high(CHUNK_START, CHUNK_START - writeHead.offset + 1);
			ap_uint<64> newBits = chunk(CHUNK_SIZE_BITS - 1, writeHead.offset);
			writeHead.high = (	writeHead.high(CHUNK_START, CHUNK_START - writeHead.offset + 1),
								chunk(CHUNK_START, writeHead.offset));
		}
	}

	if(numberOfBitsInLow > 0) {

		int lowOffset = (writeHead.offset - CHUNK_SIZE_BITS);
		if(lowOffset < 0) {
			lowOffset = 0;
		}

		// fill overflowing bits into low
		writeHead.low =	chunk(CHUNK_START - numberOfBitsInHigh, 0);

		// shift bits to the start of low
		writeHead.low <<= numberOfBitsInHigh;
	}


	writeHead.offset += 64;

	returnValue->high = writeHead.high;
	returnValue->low = writeHead.low;
	returnValue->offset = writeHead.offset;
}

void appendOpcode(outputChunk *returnValue, const ap_uint<OPCODE_SIZE> opcode, outputChunk writeHead) {

	// opcode fits into high
	if(CHUNK_SIZE_BITS - writeHead.offset >= OPCODE_SIZE) {

		// shift offset bits as far to the right as possible while leaving space for the
		// opcode bits
		writeHead.high >>= CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE;

		ap_uint<64> opcode64 = opcode;
		// add opcode at the right end
		writeHead.high |= opcode64;

		// shift payload back
		ap_uint<8> bitsToShift = (CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE);
		writeHead.high <<= bitsToShift;
	}
	// opcode overlaps into low
	else {
		// how many opcode bits fit into high?
		int numberOfBitsInHigh = CHUNK_SIZE_BITS - writeHead.offset;
		if(numberOfBitsInHigh < 0) {
			numberOfBitsInHigh = 0;
		}
		int numberOfBitsInLow = OPCODE_SIZE - numberOfBitsInHigh;

		if(numberOfBitsInHigh > 0) {
								// offset bits
			writeHead.high = (writeHead.high(CHUNK_SIZE_BITS - 1, numberOfBitsInHigh - 1),
								// opcode bits
							  opcode(OPCODE_SIZE - 1, numberOfBitsInHigh - 1));
		}

		// fill remaining opcode bits into low
		writeHead.low = opcode(numberOfBitsInLow - 1, 0);
		// shift bits as far left as possible
		int lowOffset = (writeHead.offset - CHUNK_SIZE_BITS);
		if(lowOffset < 0) {
			lowOffset = 0;
		}
		writeHead.low <<= (CHUNK_SIZE_BITS - numberOfBitsInLow - lowOffset);
	}

	writeHead.offset += OPCODE_SIZE;

	*returnValue = writeHead;
}
void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const ap_uint<8> &offset);

ap_uint<8> readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input);

// compressed chunks are preceded by a 5 bit opcode, so they can't be addressed by a byte pointer, the chunkdata will
// be copied to the chunk data structure
void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk);

ap_uint<8> extractOpcode(ap_uint<8> offset, ap_uint<16> input);
void extractAlignedData(outputChunk *returnValue, outputChunk chunk, ap_uint<8> out[BLOCK_SIZE], uint32_t outputIterator) {
	if(chunk.readyToExtract()) {
		ap_uint<64> extractedSequence = chunk.extractHigh();
		out[outputIterator + 0] = extractedSequence(63, 56);
		out[outputIterator + 1] = extractedSequence(55, 48);
		out[outputIterator + 2] = extractedSequence(47, 40);
		out[outputIterator + 3] = extractedSequence(39, 32);
		out[outputIterator + 4] = extractedSequence(31, 24);
		out[outputIterator + 5] = extractedSequence(23, 16);
		out[outputIterator + 6] = extractedSequence(15, 8);
		out[outputIterator + 7] = extractedSequence(7, 0);
	}

	*returnValue = chunk;
}
