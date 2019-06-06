// testing framework
#include "../Catch2/single_include/catch2/catch.hpp"

// module to test
#include "../ringbuffer.h"

// fundamentals
#include "stdint.h"

// global magic numbers
#include "../settings.h"

void  addToRingBufferWrapper(	ap_uint<CHUNK_SIZE_BITS> *i_fragment,
								RingBuffer& ringBufferMeta,
								ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]) {
	#pragma SDS async(10)
	addToRingBuffer(i_fragment, ringBufferMeta, buffer);
	#pragma SDS wait(10)
}

const void getFromRingBufferWrapper(	const uint64_t i_index,
								ap_uint<CHUNK_SIZE_BITS> *o_fragment,
								ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]) {
	#pragma SDS async(11)
	getFromRingBuffer(i_index, o_fragment, buffer);
	#pragma SDS wait(11)
}

TEST_CASE( "Append elements till buffer is full", "[RingBuffer]" ) {

	auto buffer = (ap_uint<CHUNK_SIZE_BITS>*) malloc(RINGBUFFER_SIZE*sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto *bufferMeta = (RingBuffer*) malloc(sizeof(RingBuffer));
	auto bufferrMeta = new (buffer) RingBuffer();
	bufferrMeta->index = 0;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0; i < RINGBUFFER_SIZE; i++) {
		addToRingBufferWrapper(&i, *bufferrMeta, buffer);
	}

	bool bufferTest = true;
	ap_uint<CHUNK_SIZE_BITS> container;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0; i < RINGBUFFER_SIZE; i++) {
		getFromRingBufferWrapper(i, &container, buffer);
		if(container != i) {
			bufferTest = false;
		}
	}

	REQUIRE(bufferTest);
}

TEST_CASE( "Append elements and overwrite existing", "[RingBuffer]" ) {
	auto buffer = (ap_uint<CHUNK_SIZE_BITS>*) malloc(RINGBUFFER_SIZE*sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto payload = (ap_uint<CHUNK_SIZE_BITS>*) malloc(sizeof(ap_uint<CHUNK_SIZE_BITS>));
	auto *bufferMeta = (RingBuffer*) malloc(sizeof(RingBuffer));
	auto bufferrMeta = new (buffer) RingBuffer();
	bufferMeta->index = 0;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0; i < RINGBUFFER_SIZE; i++) {
		addToRingBufferWrapper(&i, *bufferMeta, buffer);
	}

	bool bufferTest = true;
	ap_uint<CHUNK_SIZE_BITS> container;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0 - 1; i < RINGBUFFER_SIZE; i++) {
		getFromRingBufferWrapper(i, &container, buffer);
		if(container != RINGBUFFER_SIZE + i) {
			bufferTest = false;
		}
	}

	REQUIRE(bufferTest);
}
