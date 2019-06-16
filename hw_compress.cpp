#include "hw842.h"

//#include "sds_lib.h"
#include "ap_int.h"

#include "io.h"
#include "ringbuffer.h"
#include "addresscache.h"

//#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,out:PHYSICAL_CONTIGUOUS)
int hw842_compress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE], uint32_t blockSize)
{

	auto ringBufferMeta = new RingBuffer();
	auto buffer = (ap_uint<CHUNK_SIZE_BITS>*) malloc(RINGBUFFER_SIZE*sizeof(ap_uint<CHUNK_SIZE_BITS>));
	AddressCache cache;

	for(uint32_t i = 0; i < CACHE_SIZE; i++) {
		cache.valid[i] = false;
	}

    // append chunk as all (D8) data action
    const ap_uint<8> opCode = 0;

    outputChunk writeHead;
   	uint8_t offset = 0;
    uint32_t outputIterator = 0;

    for(uint32_t i = 0; i <= blockSize - CHUNK_SIZE; i += CHUNK_SIZE)
    {
//    	std::cout<<"0"<<std::endl;
    	// debug
		uint8_t in0 = in[i + 0];
		uint8_t in1 = in[i + 1];
		uint8_t in2 = in[i + 2];
		uint8_t in3 = in[i + 3];
		uint8_t in4 = in[i + 4];
		uint8_t in5 = in[i + 5];
		uint8_t in6 = in[i + 6];
		uint8_t in7 = in[i + 7];

		// D8 action for now
    	ap_uint<OPCODE_SIZE> opcode = 0;
    	ap_uint<64> chunk = (	in[i + 0],
    							in[i + 1],
								in[i + 2],
								in[i + 3],
								in[i + 4],
								in[i + 5],
								in[i + 6],
								in[i + 7]);

//    	std::cout<<"1"<<std::endl;
    	// check for cache hit
    	bool valid = false;
    	uint32_t cachedAddress = 0;
    	cache.get(&chunk, &cachedAddress, &valid);

//    	std::cout<<"2"<<std::endl;
    	if(valid) {
//    		std::cout << "cache hit!" << std::endl;

    		// use index action 0x19:  { I8, N0, N0, N0 }, 8 bits
    		opcode = 0x19;
    		chunk = cachedAddress;
    	} else {
//    		std::cout<<"cache not hit!"<<std::endl;
    		// this is not correct yet! just testing
    		uint32_t index = ringBufferMeta->index;
    		cache.set(&chunk, &index);
    	}


//    	std::cout<<"addToRingbuffer: "<<chunk<<std::endl;
//		#pragma SDS async(4)
		addToRingBuffer(&chunk, *ringBufferMeta, buffer);
//		#pragma SDS wait(4)

//		std::cout<<"appendOpcode: "<<opcode<<std::endl;
		appendOpcode(&opcode, &writeHead, &offset);

    	uint8_t change = offset;
    	writeHead.offset = offset;
//    	std::cout<<"extractAlignedData: "<<std::endl;
    	extractAlignedData(&writeHead, out, outputIterator);
    	offset = writeHead.offset;
    	if(change != writeHead.offset) {
    		outputIterator += 8;
    	}

//    	std::cout<<"appendWord: "<<chunk<<" writehead: "<<writeHead.high<<" "<<writeHead.low<<" opcode: "<<opcode<<" offset: " << offset<< std::endl;
//		#pragma SDS async(6)
		appendWord((uint64_t) chunk, &writeHead, &offset);
//		#pragma SDS wait(6)

		change = offset;
		writeHead.offset = offset;

//		std::cout<<"extractAlignedData: "<<std::endl;
		extractAlignedData(&writeHead, out, outputIterator);
		offset = writeHead.offset;
		if(change != writeHead.offset) {
			outputIterator += 8;
		}

//		std::cout<<"set output: "<<std::endl;
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

    return 0;

}
