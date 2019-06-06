#ifndef HW842_H
#define HW842_H

#include <stdint.h>
#include "ap_int.h"

#include "settings.h"

int hw842_compress(	const ap_uint<8> in[BLOCK_SIZE],
					ap_uint<8> out[BLOCK_SIZE],
					uint32_t blockSize);

int hw842_decompress(	const ap_uint<8> in[BLOCK_SIZE],
						ap_uint<8> out[BLOCK_SIZE],
						uint32_t blockSize);

#endif
