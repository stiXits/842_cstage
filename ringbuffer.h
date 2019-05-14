#include "stdint.h"

#include "ap_int.h"

#include "settings.h"

class RingBuffer {

private:
	// TODO: use fixed size integer
	uint32_t index = 0;
	ap_uint<8> *buffer;

public:
	RingBuffer();
	~RingBuffer();
	void add(const ap_uint<CHUNK_SIZE> *i_fragment);
	const void get(const uint16_t i_index, ap_uint<CHUNK_SIZE> *o_fragment);
};
