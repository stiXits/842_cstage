#include "ringbuffer.h"

//#pragma SDS data mem_attribute(i_fragment:PHYSICAL_CONTIGUOUS, ringBuffer:PHYSICAL_CONTIGUOUS)
void addToRingBuffer(ap_uint<CHUNK_SIZE_BITS> *i_fragment, RingBuffer& ringBufferMeta, ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]) {
	buffer[ringBufferMeta.index] = *i_fragment;
	ap_uint<64> newIndex = ringBufferMeta.index + 1;
	newIndex = newIndex % RINGBUFFER_SIZE;
	ringBufferMeta.index = newIndex;
}

void getFromRingBuffer(	const uint64_t i_index,
								ap_uint<CHUNK_SIZE_BITS> *o_fragment,
								ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]) {
	*o_fragment = buffer[i_index];
}
