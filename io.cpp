#include "io.h"
#include "ap_int.h"

#include "settings.h"

void appendUncompressedByte(const ap_uint<8> *source, ap_uint<8> *destination0, ap_uint<8> *destination1, const uint8_t &offset){

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

ap_uint<69> appendCompressedChunk(const uint8_t opCode, const ap_uint<8> &chunk) {

	return (opCode, chunk.data[0], chunk.data[1], chunk.data[2], chunk.data[3], chunk.data[4], chunk.data[5], chunk.data[6], chunk.data[7]);
//    // shift opcode to the left
//	ap_uint<8> shiftedOpcode = opCode << (8 - OPCODE_LENGHT);
//
//    // write opcode to destination
//	ap_uint<8> *lsB = destination.byteIndex;
//	ap_uint<8> *msB = destination.byteIndex + 1;
//    appendUncompressedByte(&shiftedOpcode, lsB, msB, destination.offset);
//
//    // increment writehead
//    destination.increment(OPCODE_LENGHT);
//
//    // Todo: use smaller int type for byteIndex
//    for (uint8_t byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
//
//        //acquire addresses of the next 2 bytes
//        uint8_t *ap_uint<8> = destination.byteIndex;
//        uint8_t *ap_uint<8> = destination.byteIndex + 1;
//
//        // set source to current byte of chunk
//        const ap_uint<8> *source = chunk + byteIndex;
//
//        appendUncompressedByte(source, lsB, msB, destination.offset);
//
//        // increment writehead
//        destination.increment(8);
//    }
}

uint8_t readNextCompressedByte(inputChunkPointer &readHead, const ap_uint<16> input) {

    uint16_t input_readable = input;

	uint8_t start = 15 - readHead.offset;
	uint8_t end = 15 - readHead.offset - 7;

    // shift bytes, to align them
    uint8_t output = input(start, end);

    return output;
}

uint8_t extractOpcode(uint8_t offset, ap_uint<16> input) {

	uint8_t start = 15 - offset;
	uint8_t end = 15 - offset - 4;

    return input(start, end);
}

void readNextCompressedChunk(inputChunkPointer &readHead, const ap_uint<8>* input, struct chunk &outputChunk) {

	ap_uint<8> lsB = *(input + readHead.byteIndex);
	ap_uint<8> msB = *(input + readHead.byteIndex + 1);
    outputChunk.opCode = extractOpcode(readHead.offset, (lsB,msB));
    readHead.increment(5);

    for(int byteIndex = 0; byteIndex < CHUNK_SIZE; byteIndex++) {
        uint8_t byte = readNextCompressedByte(readHead, (input[readHead.lsB()], input[readHead.msB()]));
        *(outputChunk.data + byteIndex) = byte;
        readHead.increment(8);
    }
}
