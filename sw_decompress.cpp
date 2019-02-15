#include "sw842.h"

#include "io.h"

int sw842_decompress(const uint8_t *in, uint8_t *out, uint32_t blockSize)
{
    struct inputChunkPointer readHead;
    readHead.byteIndex = 0;
    readHead.offset = 0;

    for(int chunkIndex = 0; chunkIndex <= blockSize; chunkIndex += CHUNK_SIZE)
    {
        struct chunk chunk;

        readNextCompressedChunk(readHead, in, chunk);

        for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
            out[chunkIndex + byteIndex] = chunk.data[byteIndex];
        }
    }

    return 0;
}
