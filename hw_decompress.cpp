#include "hw842.h"

#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,in:PHYSICAL_CONTIGUOUS)
int hw842_decompress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE])
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        out[i] = in[i];
    }
}