#include <iostream>
//#include "external_test/test.h"
#include "unittest/test.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <vector>

#include "hw842.h"
#include "sw842.h"
//#include <sds_lib.h>
//#define BLOCK_SIZE 32768
// #define BLOCK_SIZE 4096
#define STRLEN 4096

//#define USEHW


long long currentTimestamp() {
    return 1000000000;
}

int nextMultipleOfBlockSize(unsigned int input) {
    return (input + (BLOCK_SIZE-1)) & ~(BLOCK_SIZE-1);
}

bool checkResult(ap_uint<8> *beforeCompression, ap_uint<8> *afterCompression, ap_uint<8> *in, unsigned int ilen) {
    //if(argc <= 1) {
    for (int i = 0; i < 64; i++) {
        std::cout << beforeCompression[i] << "x";
    }

    std::cout << std::endl << std::endl;

    for (int i = 0; i < 32; i++) {
        std::cout << afterCompression[i] << "x";
    }
    //}

    std::cout << std::endl << std::endl;


    if (memcmp(in, afterCompression, ilen) == 0) {
        std::cout << "Compression- and decompression was successful!" << std::endl;
        return true;
    } else {
        std::cerr <<  "FAIL: Decompressed data differs from the original input data." << std::endl;
        return false;
    }
}

bool runExternalTest()
{
	int argc = 0;
	const char* argv[] = {};


    ap_uint<8> *in, *out_sw, *out_hw, *decompressed_sw, *decompressed_hw;
    in = out_sw = out_hw = decompressed_sw = decompressed_hw = NULL;
    unsigned int ilen, olen, dlen_sw, dlen_hw;
    ilen = olen = dlen_sw = 0;
    long long timestart_comp, timeend_comp;
    long long timestart_decomp, timeend_decomp;
    long long timestart_condense, timeend_condense;

    std::cout << "entering test" << std::endl;

	ilen = STRLEN;
	olen = ilen * 2;

	dlen_hw = ilen * 2;
	dlen_sw = ilen;

	in = (ap_uint<8>*) malloc(ilen * sizeof(char));
	out_sw = (ap_uint<8>*) malloc(olen * sizeof(char));
	out_hw = (ap_uint<8>*) malloc(olen * sizeof(char));
	decompressed_sw = (ap_uint<8>*) malloc(dlen_sw * sizeof(char));
	decompressed_hw = (ap_uint<8>*) malloc(dlen_hw * sizeof(char));

	std::vector<ap_uint<8>> tmp (32);
	tmp = {48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63};//"0011223344556677889900AABBCCDDEE";
	//tmp[0] = 0xff;
	//tmp[1] = 0x00;
	std::cout << "input smaller than chunksize" << std::endl;

	memset(in, 0, ilen);
	memset(out_sw, 0, olen);
	memset(decompressed_sw, 0, dlen_sw);
	strncpy((char *) in, (const char *) &tmp, STRLEN);
	hw842_compress(in, out_hw, BLOCK_SIZE);
	hw842_decompress(out_hw, decompressed_hw, BLOCK_SIZE);

	memset(in, 0, ilen);
	memset(out_hw, 0, olen);
	memset(decompressed_hw, 0, dlen_hw);
	strncpy((char *) in, (const char *) &tmp, STRLEN);
	sw842_compress(in, out_sw, BLOCK_SIZE);
	sw842_decompress(out_sw, decompressed_sw, BLOCK_SIZE);

	std::cout << "foobar" << std::endl;

	bool testSw = checkResult(out_sw, decompressed_sw, in, ilen);
	bool testHw = checkResult(out_hw, decompressed_hw, in, ilen);

	//std::cout << "Input: " << ilen << " bytes" << std::endl;
	//std::cout << "Output: " << olen << " bytes" << std::endl;
	std::cout << "Compression factor: " << (float) olen / (float) ilen << std::endl;

	return testSw == testHw == true;

}

int main(int argc, const char* argv[]){

    if(! runTests())
    {
        std::cout << "tests failed!";
        return 1;
    }
    else {
    	std::cout << "unittests successfull!";
    }

    if(! runExternalTest())
    {
        std::cout << "tests failed!";
        return 1;
    }

    std::cout << "All tests passes, YAAAYYY!";
    return 0;
}
