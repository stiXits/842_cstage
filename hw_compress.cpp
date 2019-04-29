#include "hw842.h"

#include "ap_int.h"

#include "io.h"

//#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,out:PHYSICAL_CONTIGUOUS)
int hw842_compress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE], uint32_t blockSize)
{
    // append chunk as all (D8) data action
    const ap_uint<8> opCode = 0;

    struct outputChunk writeHead;
    uint32_t outputIterator = 0;

    for(int i = 0; i <= blockSize; i += CHUNK_SIZE)
    {
    	ap_uint<64> chunk = (in[i + 0], in[i + 1], in[i + 2], in[i + 3], in[i + 4], in[i + 5], in[i + 6], in[i + 7]);
    	ap_uint<5> opcode = 0;
    	uint8_t offset = 0;


		#pragma SDS async(4)
		appendOpcode(&opcode, &writeHead, &offset);
		#pragma SDS wait(4)

    	ap_uint<8> change = offset;
    	writeHead.offset = offset;
    	extractAlignedData(&writeHead, writeHead, out, outputIterator);
    	if(change != offset) {
    		outputIterator += 8;
    	}

		//    auto writeHead2 = (outputChunk*) sds_alloc(sizeof(outputChunk));
		//    writeHead2 = writeHead;
		std::cout<<"foobar";

		#pragma SDS async(5)
		appendWord(&chunk, &writeHead, &offset);
		#pragma SDS wait(5)

		change = offset;
		writeHead.offset = offset;
		extractAlignedData(&writeHead, writeHead, out, outputIterator);
		if(change != offset) {
			outputIterator += 8;
		}
    }

    return 0;

}
