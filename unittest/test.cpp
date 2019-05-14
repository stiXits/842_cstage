#include "ioTest.h"
//#include "sw_compressTest.h"
#include "hw_compressTest.h"
#include "ringbufferTest.h"

bool runTests() {

    // exit code 1 means an error occurred
    return run_IoTests()
            //&& run_sw_compressTests()
			&& run_hw_compressTests()
			&& run_ringbufferTests();
}
