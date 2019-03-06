#pragma once

#include <stdint.h>
#include "ap_int.h"

#include "settings.h"

int sw842_compress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize);

int sw842_decompress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize);

