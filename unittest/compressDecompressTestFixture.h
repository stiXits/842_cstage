#include <stdlib.h>

#include "ap_int.h"

class Fixture {
public:
	Fixture() : uncompressed((ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>))),
				compressed((ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>))),
				compressedWithIndex((ap_uint<8>*) malloc(BLOCK_SIZE * sizeof(ap_uint<8>))){

		initArray(uncompressed, BLOCK_SIZE, 0);
		initArray(compressed, BLOCK_SIZE, 0);
		initArray(compressedWithIndex, BLOCK_SIZE, 0);


		//        11100001  00100001  01000001  01100001  10000001  10100001  11000001  11100010
		// no 8-byte repetition sequence here!
		uncompressed[0] = 225;
		uncompressed[1] = 33;
		uncompressed[2] = 65;
		uncompressed[3] = 97;
		uncompressed[4] = 129;
		uncompressed[5] = 161;
		uncompressed[6] = 193;
		uncompressed[7] = 226;
		uncompressed[8] = 225;
		uncompressed[9] = 33;
		uncompressed[10] = 65;
		uncompressed[11] = 97;
		uncompressed[12] = 129;
		uncompressed[13] = 161;
		uncompressed[14] = 193;
		uncompressed[15] = 226;

	    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010|000
	    // opcode|      chunk
		compressed[0] = 7;
		compressed[1] = 9;
		compressed[2] = 10;
		compressed[3] = 11;
		compressed[4] = 12;
		compressed[5] = 13;
		compressed[6] = 14;
		compressed[7] = 15;
		compressed[8] = 16;
		compressed[9] = 56;
		compressed[10] = 72;
		compressed[11] = 80;
		compressed[12] = 152;
		compressed[13] = 96;
		compressed[14] = 104;
		compressed[15] = 112;
		compressed[16] = 120;
		compressed[17] = 128;
	    compressed[18] = 0;

	    //      7          9        10        11        12       13        14          15
	    //  00000|111 00001|001 00001|010 00001|011 00001|100 00001|101 00001|110 00001|111 00010
	    // opcode|      chunk

	    // chunk with index opcode referencing the first 64 bit in the ringbuffer
	    //  110 01|000000 00|0000000 0|0000000 0|0000000 0|0000000 0|0000000 0|0000000
	    //  opcode
	    compressedWithIndex[0] = 7;
	    compressedWithIndex[1] = 9;
	    compressedWithIndex[2] = 10;
	    compressedWithIndex[3] = 11;
	    compressedWithIndex[4] = 12;
	    compressedWithIndex[5] = 13;
	    compressedWithIndex[6] = 14;
	    compressedWithIndex[7] = 15;
	    compressedWithIndex[8] = 22;
	    compressedWithIndex[9] = 64;
	    compressedWithIndex[10] = 0;
	    compressedWithIndex[11] = 0;
	    compressedWithIndex[12] = 0;
	    compressedWithIndex[13] = 0;
	    compressedWithIndex[14] = 0;
	    compressedWithIndex[15] = 0;
	    compressedWithIndex[16] = 0;
	    compressedWithIndex[17] = 0;
	    compressedWithIndex[18] = 0;
	}
	~Fixture() {
		free(uncompressed);
		free(compressed);
		free(compressedWithIndex);
	}
protected:
	ap_uint<8> *uncompressed;
	ap_uint<8> *compressed;
	ap_uint<8> *compressedWithIndex;
};
