#include "ioTest.h"
#include "sw_compressTest.h"

bool runTests() {

    // exit code 1 means an error occurred
    return run_IoTests()
            && run_sw_compressTests();
}
