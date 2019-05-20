// testing framework
#include "catch.hpp"

// module to test
#include "../ringbuffer.h"

// fundamentals
#include "stdint.h"

// global magic numbers
#include "../settings.h"

TEST_CASE( "Append elements till buffer is full", "[RingBuffer]" ) {
	RingBuffer buffer;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0; i < RINGBUFFER_SIZE; i++) {
		buffer.add(&i);
	}

	bool bufferTest = true;
	ap_uint<CHUNK_SIZE_BITS> container;

	for(ap_uint<8> i = 0; i < RINGBUFFER_SIZE; i++) {
		buffer.get(i, &container);
		if(container != i) {
			bufferTest = false;
		}
	}

	REQUIRE(bufferTest);
}

TEST_CASE( "Append elements and overwrite existing", "[RingBuffer]" ) {
	RingBuffer buffer;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0; i < RINGBUFFER_SIZE * 2; i++) {
		buffer.add(&i);
	}

	bool bufferTest = true;
	ap_uint<CHUNK_SIZE_BITS> container;

	for(ap_uint<CHUNK_SIZE_BITS> i = 0 - 1; i < RINGBUFFER_SIZE; i++) {
		buffer.get(i, &container);
		if(container != RINGBUFFER_SIZE + i) {
			bufferTest = false;
		}
	}

	REQUIRE(bufferTest);
}
