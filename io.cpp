#include "io.h"
#include "ap_int.h"
#include "hls_math.h"

#include "settings.h"

#define CHUNK_START (CHUNK_SIZE_BITS - 1)

void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const ap_uint<8> &offset){

    // copy source chunk
    ap_uint<8> rightshifted = *source;
    ap_uint<8> leftshifted = *source;

    // shift content behind existing content in destination0
    rightshifted = rightshifted >> offset;

    // copy overflowing bits to destination1 (the follow up byte)
    leftshifted = leftshifted << (8 - offset);

    // add zeropadded, shifted content to destination 0
    *destination0 = *destination0 | rightshifted;
    *destination1 = leftshifted;
}

//void appendCompressedChunk(outputChunk *returnValue, const ap_uint<64> chunk, outputChunk writeHead) {
//	// how many opcode bits fit into high?
//	int numberOfBitsInHigh = CHUNK_SIZE_BITS - writeHead.offset;
//	if(numberOfBitsInHigh < 0) {
//		numberOfBitsInHigh = 0;
//	}
//	int numberOfBitsInLow = CHUNK_SIZE_BITS - numberOfBitsInHigh;
//
//	if(numberOfBitsInHigh > 0) {
//		// fill high with chunk as far as possible
//		if(writeHead.offset == 0) {
//			writeHead.high = chunk;
//		}
//		else {
//			ap_uint<64> remainder = writeHead.high(CHUNK_START, CHUNK_START - writeHead.offset + 1);
//			ap_uint<64> newBits = chunk(CHUNK_SIZE_BITS - 1, writeHead.offset);
//			writeHead.high = (	writeHead.high(CHUNK_START, CHUNK_START - writeHead.offset + 1),
//								chunk(CHUNK_START, writeHead.offset));
//		}
//	}
//
//	if(numberOfBitsInLow > 0) {
//
//		int lowOffset = (writeHead.offset - CHUNK_SIZE_BITS);
//		if(lowOffset < 0) {
//			lowOffset = 0;
//		}
//
//		// fill overflowing bits into low
//		writeHead.low =	chunk(CHUNK_START - numberOfBitsInHigh, 0);
//
//		// shift bits to the start of low
//		writeHead.low <<= numberOfBitsInHigh;
//	}
//
//
//	writeHead.offset += 64;
//
//	returnValue->high = writeHead.high;
//	returnValue->low = writeHead.low;
//	returnValue->offset = writeHead.offset;
//}

//void appendOpcode(outputChunk *returnValue, const ap_uint<OPCODE_SIZE> opcode, outputChunk writeHead) {
//
//	// opcode fits into high
//	if(CHUNK_SIZE_BITS - writeHead.offset >= OPCODE_SIZE) {
//
//		// shift offset bits as far to the right as possible while leaving space for the
//		// opcode bits
//		writeHead.high >>= CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE;
//
//		ap_uint<64> opcode64 = opcode;
//		// add opcode at the right end
//		writeHead.high |= opcode64;
//
//		// shift payload back
//		ap_uint<8> bitsToShift = (CHUNK_SIZE_BITS - writeHead.offset - OPCODE_SIZE);
//		writeHead.high <<= bitsToShift;
//	}
//	// opcode overlaps into low
//	else {
//		// how many opcode bits fit into high?
//		int numberOfBitsInHigh = CHUNK_SIZE_BITS - writeHead.offset;
//		if(numberOfBitsInHigh < 0) {
//			numberOfBitsInHigh = 0;
//		}
//		int numberOfBitsInLow = OPCODE_SIZE - numberOfBitsInHigh;
//
//		if(numberOfBitsInHigh > 0) {
//								// offset bits
//			writeHead.high = (writeHead.high(CHUNK_SIZE_BITS - 1, numberOfBitsInHigh - 1),
//								// opcode bits
//							  opcode(OPCODE_SIZE - 1, numberOfBitsInHigh - 1));
//		}
//
//		// fill remaining opcode bits into low
//		writeHead.low = opcode(numberOfBitsInLow - 1, 0);
//		// shift bits as far left as possible
//		int lowOffset = (writeHead.offset - CHUNK_SIZE_BITS);
//		if(lowOffset < 0) {
//			lowOffset = 0;
//		}
//		writeHead.low <<= (CHUNK_SIZE_BITS - numberOfBitsInLow - lowOffset);
//	}
//
//	writeHead.offset += OPCODE_SIZE;
//
//	*returnValue = writeHead;
//}

ap_uint<8> readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input) {

    uint16_t input_readable = input;

	ap_uint<8> start = 15 - readHead.offset;
	ap_uint<8> end = 15 - readHead.offset - 7;

    // shift bytes, to align them
    ap_uint<8> output = input(start, end);

    return output;
}

ap_uint<8> extractOpcode(ap_uint<8> offset, ap_uint<16> input) {

	ap_uint<8> start = 15 - offset;
	ap_uint<8> end = 15 - offset - 4;

    return input(start, end);
}

void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk) {

	ap_uint<8> lsB = *(input + readHead.byteIndex);
	ap_uint<8> msB = *(input + readHead.byteIndex + 1);
    outputChunk.opCode = extractOpcode(readHead.offset, (lsB,msB));
    readHead.increment(5);

    for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
        ap_uint<8> byte = readNextCompressedByte(readHead, (input[readHead.lsB()], input[readHead.msB()]));
        *(outputChunk.data + byteIndex) = byte;
        readHead.increment(8);
    }
}

//void extractAlignedData(outputChunk *returnValue, outputChunk chunk, ap_uint<8> out[BLOCK_SIZE], uint32_t outputIterator) {
//	if(chunk.readyToExtract()) {
//		ap_uint<64> extractedSequence = chunk.extractLow();
//		out[outputIterator + 0] = extractedSequence(63, 56);
//		out[outputIterator + 1] = extractedSequence(55, 48);
//		out[outputIterator + 2] = extractedSequence(47, 40);
//		out[outputIterator + 3] = extractedSequence(39, 32);
//		out[outputIterator + 4] = extractedSequence(31, 24);
//		out[outputIterator + 5] = extractedSequence(23, 16);
//		out[outputIterator + 6] = extractedSequence(15, 8);
//		out[outputIterator + 7] = extractedSequence(7, 0);
//	}
//
//	*returnValue = chunk;
//}
