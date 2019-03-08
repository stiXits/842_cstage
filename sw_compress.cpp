#include "sw842.h"

#include "ap_int.h"

#include "io.h"

outputChunk extractAlignedData(outputChunk chunk, ap_uint<8> *out, uint32_t outputIterator) {
	if(chunk.readyToExtract()) {
		ap_uint<64> extractedSequence = chunk.extractLow();
		out[outputIterator + 0] = extractedSequence(63, 56);
		out[outputIterator + 1] = extractedSequence(55, 48);
		out[outputIterator + 2] = extractedSequence(47, 40);
		out[outputIterator + 3] = extractedSequence(39, 32);
		out[outputIterator + 4] = extractedSequence(31, 24);
		out[outputIterator + 5] = extractedSequence(23, 16);
		out[outputIterator + 6] = extractedSequence(15, 8);
		out[outputIterator + 7] = extractedSequence(7, 0);
	}

	return chunk;
}

int sw842_compress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize)
{
    // append chunk as all (D8) data action
    const uint8_t opCode = 0;

    struct outputChunk writeHead;
    uint32_t outputIterator = 0;

    for(int i = 0; i <= blockSize; i += CHUNK_SIZE)
    {
    	ap_uint<64> chunk = (in[i + 0], in[i + 1], in[i + 2], in[i + 3], in[i + 4], in[i + 5], in[i + 6], in[i + 7]);
    	ap_uint<5> opcode = 0;
    	writeHead = appendOpcode(opcode, writeHead);
    	uint8_t change = writeHead.offset;
    	writeHead = extractAlignedData(writeHead, out, outputIterator);
    	if(change != writeHead.offset) {
    		outputIterator += 8;
    	}
    	change = outputIterator;
    	writeHead = appendCompressedChunk(chunk, writeHead);
    	writeHead = extractAlignedData(writeHead, out, outputIterator);
    	if(change != writeHead.offset) {
    		outputIterator += 8;
    	}
    }

    return 0;
}
