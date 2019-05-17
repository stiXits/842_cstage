#include "hw842.h"

#include "ap_int.h"

#include "io.h"
#include "ringbuffer.h"
#include "addresscache.h"

//#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,out:PHYSICAL_CONTIGUOUS)
int hw842_compress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE], uint32_t blockSize)
{
	auto buffer = new RingBuffer();
	auto cache = new AddressCache();

    // append chunk as all (D8) data action
    const ap_uint<8> opCode = 0;

    struct outputChunk writeHead;
   	uint8_t offset = 0;
    uint32_t outputIterator = 0;

    for(uint32_t i = 0; i <= blockSize; i += CHUNK_SIZE)
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

    	ap_uint<64> chunk = (in[i + 0], in[i + 1], in[i + 2], in[i + 3], in[i + 4], in[i + 5], in[i + 6], in[i + 7]);

    	// check for cache hit
    	bool valid = false;
    	uint32_t cachedAddress = 0;
    	cache->get(&chunk, &cachedAddress, &valid);

    	if(valid) {
    		std::cout << "cache hit!" << std::endl;
    	} else {
    		// this is not correct yet! just testing
    		uint32_t index;
    		buffer->getCurrentIndex(&index);
    		cache->set(&chunk, &index);
    	}
		buffer->add(&chunk);

    	ap_uint<5> opcode = 0;

		#pragma SDS async(4)
		appendOpcode(&opcode, &writeHead, &offset);
		#pragma SDS wait(4)

    	uint8_t change = offset;
    	writeHead.offset = offset;
    	extractAlignedData(&writeHead, out, outputIterator);
    	offset = writeHead.offset;
    	if(change != writeHead.offset) {
    		outputIterator += 8;
    	}

		#pragma SDS async(5)
		appendWord(&chunk, &writeHead, &offset);
		#pragma SDS wait(5)

		change = offset;
		writeHead.offset = offset;
		extractAlignedData(&writeHead, out, outputIterator);
		offset = writeHead.offset;
		if(change != writeHead.offset) {
			outputIterator += 8;
		}

		// debug
		uint8_t out0 = out[outputIterator + 0];
		uint8_t out1 = out[outputIterator + 1];
		uint8_t out2 = out[outputIterator + 2];
		uint8_t out3 = out[outputIterator + 3];
		uint8_t out4 = out[outputIterator + 4];
		uint8_t out5 = out[outputIterator + 5];
		uint8_t out6 = out[outputIterator + 6];
		uint8_t out7 = out[outputIterator + 7];
    }

    delete buffer;

    return 0;

}
