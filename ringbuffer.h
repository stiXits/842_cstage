#include "stdint.h"

#include "ap_int.h"

#include "settings.h"

class RingBuffer {

private:
	// TODO: use fixed size integer
	uint32_t index = 0;
	ap_uint<CHUNK_SIZE_BITS> *buffer;

public:
	RingBuffer();
	~RingBuffer();
	void add(const ap_uint<CHUNK_SIZE_BITS> *i_fragment);
	const void get(const uint64_t i_index, ap_uint<CHUNK_SIZE_BITS> *o_fragment);
	void getCurrentIndex(uint32_t *o_index);
};
