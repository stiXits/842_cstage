#include "ringbuffer.h"

RingBuffer::RingBuffer() : buffer(new ap_uint<CHUNK_SIZE_BITS>[RINGBUFFER_SIZE]) {

}

RingBuffer::~RingBuffer() {
	delete[] buffer;
}

void RingBuffer::add(const ap_uint<CHUNK_SIZE_BITS> *i_fragment) {
	buffer[index] = *i_fragment;
	index = (index + 1) % RINGBUFFER_SIZE;
}

const void RingBuffer::get(const uint64_t i_index, ap_uint<CHUNK_SIZE_BITS> *o_fragment) {
	*o_fragment = buffer[i_index];
}

void RingBuffer::getCurrentIndex(uint32_t *o_index) {
	*o_index = this->index;
}
