#include "hw842.h"

#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,in:PHYSICAL_CONTIGUOUS)
int hw842_decompress(const uint8_t in[BLOCK_SIZE], uint8_t out[BLOCK_SIZE])
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        out[i] = in[i];
    }
}