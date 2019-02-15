#include "io.h"

#include "settings.h"

void appendUncompressedByte(const uint8_t *source, uint8_t *destination0, uint8_t *destination1, const uint8_t &offset){

    // copy source chunk
    uint8_t rightshifted = *source;
    uint8_t leftshifted = *source;

    // shift content behind existing content in destination0
    rightshifted = rightshifted >> offset;

    // copy overflowing bits to destination1 (the follow up byte)
    leftshifted = leftshifted << (8 - offset);

    // add zeropadded, shifted content to destination 0
    *destination0 = *destination0 | rightshifted;
    *destination1 = leftshifted;
}

void appendUncompressedChunk(const uint8_t opCode, const uint8_t *chunk, outputChunkPointer &destination) {

    // shift opcode to the left
    uint8_t shiftedOpcode = opCode << (8 - OPCODE_LENGHT);

    // write opcode to destination
    uint8_t *lsB = destination.byteIndex;
    uint8_t *msB = destination.byteIndex + 1;
    appendUncompressedByte(&shiftedOpcode, lsB, msB, destination.offset);

    // increment writehead
    destination.increment(OPCODE_LENGHT);

    // Todo: use smaller int type for byteIndex
    for (uint8_t byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {

        //acquire addresses of the next 2 bytes
        uint8_t *lsB = destination.byteIndex;
        uint8_t *msB = destination.byteIndex + 1;

        // set source to current byte of chunk
        const uint8_t *source = chunk + byteIndex;

        appendUncompressedByte(source, lsB, msB, destination.offset);

        // increment writehead
        destination.increment(8);
    }
}

void readNextCompressedByte(inputChunkPointer &readHead, const uint8_t *input, uint8_t *outPutChunk) {

    uint8_t lsB, msB;

    lsB = *(input + readHead.byteIndex);
    msB = *(input + readHead.byteIndex + 1);

    uint8_t offset = readHead.offset;

    // shift bytes, to align them
    lsB <<= readHead.offset;
    msB >>= 8 - readHead.offset;

    *outPutChunk = lsB | msB;

    readHead.increment(8);
}

uint8_t extractOpcode(uint8_t lsB, uint8_t msB, inputChunkPointer &readHead) {
    uint8_t  opcode;

    // opcode is in one byte
    if(readHead.offset <= 8 - OPCODE_LENGHT) {
        opcode = lsB >> (8 - OPCODE_LENGHT - readHead.offset);
    }
    // opcode is in 2 bytes
    else {
        // 000000|11 11100000

        // clear out leading 1s
        lsB <<= readHead.offset;
        lsB >>= readHead.offset;

        // shift opcode bits to correct position in lsB
        lsB <<= readHead.offset + OPCODE_LENGHT - 8;

        // shift opcode bits to correct position in msB
        msB >>= 8 - (readHead.offset + OPCODE_LENGHT - 8);

        opcode = lsB | msB;
    }

    readHead.increment(5);

    return opcode;
}

void readNextCompressedChunk(inputChunkPointer &readHead, const uint8_t* input, struct chunk &outputChunk) {

	uint8_t lsB = *(input + readHead.byteIndex);
	uint8_t msB = *(input + readHead.byteIndex + 1);
    outputChunk.opCode = extractOpcode(lsB, msB, readHead);

    for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
        readNextCompressedByte(readHead, input, outputChunk.data + byteIndex);
    }
}
