#pragma once

#include <stdint.h>
#include "ap_int.h"

#include "settings.h"

int hw842_compress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize);

int hw842_decompress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize);
