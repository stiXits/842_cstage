#include "ringbuffer.h"

RingBuffer::RingBuffer() : buffer(new ap_uint<8>[RINGBUFFER_SIZE]) {

}

RingBuffer::~RingBuffer() {
	delete[] buffer;
}

void RingBuffer::add(const ap_uint<CHUNK_SIZE> *i_fragment) {
	buffer[index] = *i_fragment;
	index = (index + 1) % RINGBUFFER_SIZE;
}

const void RingBuffer::get(const uint16_t i_index, ap_uint<CHUNK_SIZE> *o_fragment) {
	*o_fragment = buffer[i_index];
}
