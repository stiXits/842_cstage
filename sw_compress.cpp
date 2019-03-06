#include "sw842.h"

#include "ap_int.h"

#include "io.h"

int sw842_compress(const ap_uint<8> *in, ap_uint<8> *out, uint32_t blockSize)
{
    struct outputChunkPointer writeHead{};
    writeHead.byteIndex = 0;
    writeHead.offset = 0;

    // append chunk as all (D8) data action
    const uint8_t opCode = 0;

    for(int i = 0; i <= blockSize; i += CHUNK_SIZE)
    {
        appendCompressedChunk(opCode, in + i);
    }

    return 0;
}
