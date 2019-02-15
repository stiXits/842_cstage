#include "sw842.h"

#include "io.h"

int sw842_compress(const uint8_t *in, uint8_t *out, uint32_t blockSize)
{
    struct outputChunkPointer writeHead{};
    writeHead.byteIndex = out;
    writeHead.offset = 0;

    // append chunk as all (D8) data action
    const uint8_t opCode = 0;

    for(int i = 0; i <= blockSize; i += CHUNK_SIZE)
    {
        appendUncompressedChunk(opCode, in + i, writeHead);
    }

    return 0;
}
