#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>

#include "settings.h"
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

int runTest( int argc, const char* argv[])
{
    ap_uint<8> *in, *out, *decompressed;
    in = out = decompressed = NULL;
    unsigned int ilen, olen, dlen;
    ilen = olen = dlen = 0;
    long long timestart_comp, timeend_comp;
    long long timestart_decomp, timeend_decomp;
    long long timestart_condense, timeend_condense;

    std::cout << "entering test" << std::endl;

    if(argc <= 1) {
        ilen = STRLEN;
        olen = ilen * 2;
#ifdef USEHW
        dlen = ilen * 2;
#else
        dlen = ilen;
#endif

        in = (ap_uint<8>*) malloc(ilen * sizeof(char));
        out = (ap_uint<8>*) malloc(olen * sizeof(char));
        decompressed = (ap_uint<8>*) malloc(dlen * sizeof(char));

        ap_uint<8> tmp[] = {0x30, 0x30, 0x31, 0x31, 0x32, 0x32, 0x33, 0x33, 0x34, 0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37, 0x38, 0x38, 0x39, 0x39, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 0x45};//"0011223344556677889900AABBCCDDEE";
        //tmp[0] = 0xff;
        //tmp[1] = 0x00;

        memset(in, 0, ilen);
        memset(out, 0, olen);
        memset(decompressed, 0, dlen);

        strncpy((char *) in, (const char *) tmp, STRLEN);

    } else if (argc == 2) {
        FILE *fp;
        fp=fopen(argv[1], "r");
        fseek(fp, 0, SEEK_END);
        unsigned int flen = ftell(fp);
        ilen = flen;
        std::cout << "original file length: " << ilen << std::endl;
        ilen = nextMultipleOfBlockSize(ilen);
        std::cout << "original file length (padded): " << ilen << std::endl;
        olen = ilen * 2;
#ifdef USEHW
        dlen = ilen * 2;
#else
        dlen = ilen;
#endif
        fseek(fp, 0, SEEK_SET);

        in = (ap_uint<8>*) malloc(ilen * sizeof(char));
        out = (ap_uint<8>*) malloc(olen* sizeof(char));
        decompressed = (ap_uint<8>*) malloc(dlen* sizeof(char));
        memset(in, 0, ilen);
        memset(out, 0, olen);
        memset(decompressed, 0, dlen);

        if(!fread(in, flen, 1, fp)) {
            std::cerr << "FAIL: Reading file content to memory failed." << std::endl;
        }
        fclose(fp);
    }

    if(ilen > BLOCK_SIZE) {
        std::cerr << "Using chunks of " << BLOCK_SIZE << " bytes" << std::endl;

        int num_chunks = ilen / BLOCK_SIZE;
        uint64_t *compressedChunkPositions = (uint64_t*) malloc(sizeof(uint64_t) * num_chunks);
        uint32_t *compressedChunkSizes = (uint32_t*) malloc(sizeof(uint32_t) * num_chunks);

        timestart_comp = currentTimestamp();
#pragma omp parallel for
        for(int chunk_num = 0; chunk_num < num_chunks; chunk_num++) {

            unsigned int chunk_olen = BLOCK_SIZE * 2;
            ap_uint<8>* chunk_in = in + (BLOCK_SIZE * chunk_num);
            ap_uint<8>* chunk_out = out + ((BLOCK_SIZE * 2) * chunk_num);


#ifdef USEHW
            hw842_compress(chunk_in, chunk_out);
#else
            sw842_compress(chunk_in, chunk_out, BLOCK_SIZE);
#endif
            compressedChunkSizes[chunk_num] = chunk_olen;
        }
        timeend_comp = currentTimestamp();
        timestart_condense = timeend_comp;

        uint64_t currentChunkPos = 0;
        for(int chunk_num = 0; chunk_num < num_chunks; chunk_num++) {
            compressedChunkPositions[chunk_num] = currentChunkPos;
            currentChunkPos += compressedChunkSizes[chunk_num];
        }

        ap_uint<8> *out_condensed = (ap_uint<8> *) malloc(currentChunkPos);

#pragma omp parallel for
        for(int chunk_num = 0; chunk_num < num_chunks; chunk_num++) {
            ap_uint<8> * chunk_out = out + ((BLOCK_SIZE * 2) * chunk_num);
            ap_uint<8> * chunk_condensed = out_condensed + compressedChunkPositions[chunk_num];
            memcpy(chunk_condensed, chunk_out, compressedChunkSizes[chunk_num]);
        }
        timeend_condense = currentTimestamp();


        timestart_decomp = currentTimestamp();
#pragma omp parallel for
        for(int chunk_num = 0; chunk_num < num_chunks; chunk_num++) {
            unsigned int chunk_dlen = BLOCK_SIZE;

            ap_uint<8>* chunk_in = in + (BLOCK_SIZE * chunk_num);
            ap_uint<8>* chunk_condensed = out_condensed + compressedChunkPositions[chunk_num];
            ap_uint<8>* chunk_decomp = in + (BLOCK_SIZE * chunk_num);


#ifdef USEHW
            hw842_decompress(chunk_condensed, chunk_decomp);
#else
            sw842_decompress(chunk_condensed, chunk_decomp, BLOCK_SIZE);
#endif

            if (!(memcmp(chunk_in, chunk_decomp, BLOCK_SIZE) == 0)) {
                std::cerr << "FAIL: Decompressed data differs from the original input data." << std::endl;
                //return -1;
            }
        }
        timeend_decomp = currentTimestamp();

        free(compressedChunkPositions);
        free(compressedChunkSizes);

        std::cout << "Input: " << ilen << "bytes" << std::endl;
        std::cout << "Output: " << currentChunkPos << std::endl;
        std::cout << "Compression factor: " << (float) currentChunkPos / (float) ilen << std::endl;
        std::cout << "Compression performance: " << timeend_comp - timestart_comp << " ms / " << (ilen / 1024 / 1024) / ((float) (timeend_comp - timestart_comp) / 1000) << " MiB/s" << std::endl;
        std::cout << "Condensation performance: " << timeend_condense - timestart_condense << " ms / " << (currentChunkPos / 1024 / 1024) / ((float) (timeend_condense - timestart_condense) / 1000) << " MiB/s" << std::endl;
        std::cout << "Decompression performance: " <<  timeend_decomp - timestart_decomp << " ms / " << (ilen / 1024 / 1024) / ((float) (timeend_decomp - timestart_decomp) / 1000) << " MiB/s" << std::endl;

        std::cout <<"Compression- and decompression was successful!" << std::endl;
    } else {

        std::cout << "input smaller than chunksize" << std::endl;

#ifdef USEHW
        hw842_compress(in, out);
#else
        sw842_compress(in, out, BLOCK_SIZE);
#endif

#ifdef USEHW
        hw842_decompress(out, decompressed);
#else
        sw842_decompress(out, decompressed, BLOCK_SIZE);
#endif

        std::cout << "foobar" << std::endl;

        //std::cout << "Input: " << ilen << " bytes" << std::endl;
        //std::cout << "Output: " << olen << " bytes" << std::endl;
        std::cout << "Compression factor: " << (float) olen / (float) ilen << std::endl;


        //if(argc <= 1) {
        for (int i = 0; i < 64; i++) {
            std::cout << out[i] << "x";
        }

        std::cout << std::endl << std::endl;

        for (int i = 0; i < 32; i++) {
            std::cout << decompressed[i] << "x" << std::endl;
        }
        //}

        std::cout << std::endl << std::endl;


        if (memcmp(in, decompressed, ilen) == 0) {
            std::cout << "Compression- and decompression was successful!" << std::endl;
        } else {
            std::cerr <<  "FAIL: Decompressed data differs from the original input data." << std::endl;
            return -1;
        }

        //free(in);
        //free(out);
        //free(decompressed);

    }

}