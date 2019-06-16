#include "stdint.h"

#include "ap_int.h"

#include "settings.h"

typedef struct RingBuffer_struct {
	// TODO: use fixed size integer
	uint32_t index = 0;
}  __attribute__ ((packed, aligned(4))) RingBuffer;

//#pragma SDS data mem_attribute(i_fragment:PHYSICAL_CONTIGUOUS, ringBuffer:PHYSICAL_CONTIGUOUS, buffer::PHYSICAL_CONTIGUOUS)
void addToRingBuffer(	ap_uint<CHUNK_SIZE_BITS> *i_fragment,
						RingBuffer& ringBufferMeta,
						ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]);

void getFromRingBuffer(	const uint64_t i_index,
						ap_uint<CHUNK_SIZE_BITS> *o_fragment,
						ap_uint<CHUNK_SIZE_BITS> buffer[RINGBUFFER_SIZE]);
