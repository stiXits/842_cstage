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

typedef struct outputChunk_struct {
	uint64_t high, low = 0;
	uint8_t offset = 0;
	uint64_t padding0;
	uint32_t padding1;
	uint16_t padding2;
	uint8_t padding3;

	bool readyToExtract() {
		return offset >= 64;
	}

	ap_uint<64> extractHigh() {
		ap_uint<64> toTransfer = high;

		high = low;
		low = 0;
		offset -= 64;

		return toTransfer;
	}
} __attribute__ ((packed, aligned(32))) outputChunk;

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
void appendWord(uint64_t chunkT,
				outputChunk *writeHead,
				uint8_t *offset);

void appendOpcode(	ap_uint<OPCODE_SIZE> *opcodePointer,
					outputChunk *writeHead,
					uint8_t *offset);

void appendUncompressedByte(const ap_uint<8> *source,
							ap_uint<8> *destination0,
							ap_uint<8> *destination1,
							const ap_uint<8> &offset);

ap_uint<8> readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input);

void extractAlignedData(outputChunk *chunk,
						ap_uint<8> out[BLOCK_SIZE],
						uint32_t outputIterator);

void readCompressedChunk(	const ap_uint<64> i_data[2],
							ap_uint<64> *o_chunk,
							ap_uint<OPCODE_SIZE> *o_opcode,
							uint8_t *io_offset);

void appendUncompressedChunk(	ap_uint<64> chunk,
								ap_uint<8> out[BLOCK_SIZE],
								uint32_t outputIterator);
