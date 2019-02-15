#include "settings.h"

#include <stdint.h>

struct outputChunkPointer {

    uint8_t* byteIndex;

    // the offset points to within the first byte, thus 3 bits will be sufficient
    uint8_t offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(uint8_t increment)
    {
        uint8_t accu = offset + increment;
        byteIndex += accu / 8;
        offset = accu % 8;
    }

};

struct inputChunkPointer {

    // const
    uint32_t byteIndex;

    // the offset points to within the first byte, thus 3 bits will be sufficient
    uint8_t offset;

    outputChunkPointer& increment()
    {
        byteIndex++;
    }

    outputChunkPointer& increment(uint8_t increment)
    {
        uint8_t accu = offset + increment;
        byteIndex += accu / 8;
        offset = accu % 8;
    }

};

struct chunk {
    uint8_t data[CHUNK_SIZE];
    uint8_t opCode;
};

// the chunk lies within an aligned data block (no opcodes before data bytes), so a pointer suffices to address it
void appendUncompressedChunk(const uint8_t opCode, const uint8_t *chunk, outputChunkPointer &destination);
void appendUncompressedByte(const uint8_t *source, uint8_t *destination0, uint8_t *destination1, const uint8_t &offset);

void readNextCompressedByte(inputChunkPointer &readHead, const uint8_t *input, uint8_t *outPutChunk);

// compressed chunks are preceded by a 5 bit opcode, so they can't be addressed by a byte pointer, the chunkdata will
// be copied to the chunk data structure
void readNextCompressedChunk(inputChunkPointer &readHead, const uint8_t* input, struct chunk &outputChunk);

uint8_t extractOpcode(uint8_t lsB, uint8_t msB, inputChunkPointer &readHead);
