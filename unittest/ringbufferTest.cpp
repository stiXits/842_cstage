#include "ringbufferTest.h"

#include "stdint.h"

#include "../settings.h"
#include "../ringbuffer.h"

bool test_addMaxElements() {
	RingBuffer buffer;

	for(ap_uint<8> i = 0; i < RINGBUFFER_SIZE; i++) {
		buffer.add(&i);
	}

	bool bufferTest = true;
	ap_uint<8> container;

	for(ap_uint<8> i = 0; i < RINGBUFFER_SIZE; i++) {
		buffer.get(i, &container);
		if(container != i) {
			bufferTest = false;
		}
	}

	return bufferTest;
}

bool test_overWriteElements() {
	RingBuffer buffer;

	for(ap_uint<8> i = 0; i < RINGBUFFER_SIZE * 2; i++) {
		buffer.add(&i);
	}

	bool bufferTest = true;
	ap_uint<8> container;

	for(ap_uint<8> i = 0 - 1; i < RINGBUFFER_SIZE; i++) {
		buffer.get(i, &container);
		if(container != RINGBUFFER_SIZE + i) {
			bufferTest = false;
		}
	}

	return bufferTest;
}

bool run_ringbufferTests() {
    return     test_addMaxElements()
    		&& test_overWriteElements();

}
