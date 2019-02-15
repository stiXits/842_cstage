#pragma once

#include <stdint.h>

#include "settings.h"

int sw842_compress(const uint8_t *in, uint8_t *out, uint32_t blockSize);

int sw842_decompress(const uint8_t *in, uint8_t *out, uint32_t blockSize);

