#include "io.h"
#include "ap_int.h"
#include "hls_math.h"

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
	// how many opcode bits fit into high?
	int numberOfBitsInHigh = CHUNK_SIZE_BITS - writeHead.offset;
	if(numberOfBitsInHigh < 0) {
		numberOfBitsInHigh = 0;
	}
	int numberOfBitsInLow = CHUNK_SIZE_BITS - numberOfBitsInHigh;

	if(numberOfBitsInHigh > 0) {
		// fill high with chunk as far as possible
		writeHead.high = (	writeHead.high(CHUNK_SIZE_BITS - 1, CHUNK_SIZE_BITS - 1 - writeHead.offset),
							chunk(CHUNK_SIZE_BITS - 1, writeHead.offset));
	}

	int lowOffset = (writeHead.offset - CHUNK_SIZE_BITS);
	if(lowOffset < 0) {
		lowOffset = 0;
	}

	// fill overflowing bits into low
	writeHead.low =	chunk(CHUNK_SIZE_BITS - lowOffset - 1, 0);

	// shift bits to the start of low
	writeHead.low <<= lowOffset;

	writeHead.offset += 64;

	return writeHead;
}

outputChunk appendOpcode(const ap_uint<OPCODE_SIZE> opcode, outputChunk writeHead) {

	// opcode fits into high
	if(CHUNK_SIZE_BITS - writeHead.offset >= OPCODE_SIZE) {

		// shift offset bits as far to the right as possible while leaving space for the
		// opcode bits
		writeHead.high >>= CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE;

		ap_uint<64> opcode64 = opcode;
		// add opcode at the right end
		writeHead.high |= opcode64;

		// shift payload back
		uint8_t bitsToShift = (CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE);
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
