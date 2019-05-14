#include "hw842.h"
#include "io.h"
#include "settings.h"

//#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,in:PHYSICAL_CONTIGUOUS)
int hw842_decompress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE], uint32_t blockSize)
{
    uint32_t outputIterator = 0;
    uint8_t offset = 0;

    for(uint8_t i = 0; i <= blockSize; i += CHUNK_SIZE)
    {

    	// debug
		uint8_t in0 = in[i + 0];
		uint8_t in1 = in[i + 1];
		uint8_t in2 = in[i + 2];
		uint8_t in3 = in[i + 3];
		uint8_t in4 = in[i + 4];
		uint8_t in5 = in[i + 5];
		uint8_t in6 = in[i + 6];
		uint8_t in7 = in[i + 7];
		uint8_t in8 = in[i + 8];
		uint8_t in9 = in[i + 9];

    	ap_uint<64> compressedData[2];
    	compressedData[0] = (in[i + 0], in[i + 1], in[i + 2], in[i + 3], in[i + 4], in[i + 5], in[i + 6], in[i + 7]);
		compressedData[1] = (in[i + 8], in[i + 9], in[i + 10], in[i + 11], in[i + 12], in[i + 13], in[i + 14], in[i + 15]);

    	ap_uint<64> chunk = 0;
    	ap_uint<5> opcode = 0;

		readCompressedChunk(compressedData, &chunk, &opcode, &offset);

		// do some real decompression here

		appendUncompressedChunk(chunk, out, outputIterator);

		// debug
		uint8_t out0 = out[outputIterator + 0];
		uint8_t out1 = out[outputIterator + 1];
		uint8_t out2 = out[outputIterator + 2];
		uint8_t out3 = out[outputIterator + 3];
		uint8_t out4 = out[outputIterator + 4];
		uint8_t out5 = out[outputIterator + 5];
		uint8_t out6 = out[outputIterator + 6];
		uint8_t out7 = out[outputIterator + 7];

		// TODO: #1
		outputIterator += 8;
		if(offset >= 8) {
			offset -= 8;
			i += 1;
		}
    }

    return 0;
}

