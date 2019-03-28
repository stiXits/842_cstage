#include "hw842.h"
#include "io.h"

#pragma SDS data mem_attribute(in:PHYSICAL_CONTIGUOUS,in:PHYSICAL_CONTIGUOUS)
int hw842_decompress(const ap_uint<8> in[BLOCK_SIZE], ap_uint<8> out[BLOCK_SIZE], uint32_t blockSize)
{
    struct inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    for(int chunkIndex = 0; chunkIndex <= BLOCK_SIZE; chunkIndex += CHUNK_SIZE)
    {
        struct chunk chunk;

        readNextCompressedChunk(readHead, in, chunk);

        for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
            out[chunkIndex + byteIndex] = chunk.data[byteIndex];
        }
    }

    return 0;
}
