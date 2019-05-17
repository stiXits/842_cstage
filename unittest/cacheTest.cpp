#include "cacheTest.h"

#include "../addresscache.h"

#include "ap_int.h"

bool test_getElement_hit() {
	AddressCache cache;
	ap_uint<CHUNK_SIZE_BITS> fragment = 42;
	uint32_t address = 56;
	bool valid = false;

	uint32_t retrievedAddress;

	cache.set(&fragment, &address);
	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == true;
	bool hitTest = address == retrievedAddress;

	return hitTest && validTest;
}

bool test_getElement_hit_fullCache() {
	AddressCache cache;
	uint32_t retrievedAddress = NULL;
	ap_uint<CHUNK_SIZE_BITS> insertFragment = 0;
	ap_uint<CHUNK_SIZE_BITS> fragment = CACHE_SIZE - 2;
	bool valid = false;

	for(uint32_t i = 0; i < CACHE_SIZE; i++) {
		cache.set(&insertFragment, &i);
		insertFragment++;
	}

	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == true;
	bool hitTest = fragment == retrievedAddress;

	return hitTest && validTest;
}

bool test_getElement_miss() {
	AddressCache cache;
	uint32_t retrievedAddress = NULL;
	ap_uint<CHUNK_SIZE_BITS> insertFragment = 0;
	ap_uint<CHUNK_SIZE_BITS> fragment = CACHE_SIZE;
	bool valid = false;

	for(uint32_t i = 0; i < CACHE_SIZE; i++) {
		cache.set(&insertFragment, &i);
		insertFragment++;
	}

	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == false;

	return validTest;
}

bool test_getElement_miss_agedOut() {
	AddressCache cache;
	uint32_t retrievedAddress = NULL;
	ap_uint<CHUNK_SIZE_BITS> insertFragment = 0;
	ap_uint<CHUNK_SIZE_BITS> fragment = 0;
	bool valid = false;

	for(uint32_t i = 0; i < CACHE_SIZE + 1; i++) {
		cache.set(&insertFragment, &i);
		insertFragment++;
	}

	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == false;

	return validTest;
}

bool run_cacheTests() {
    return     test_getElement_hit()
    		&& test_getElement_hit_fullCache()
    		&& test_getElement_miss()
			&& test_getElement_miss_agedOut();

}
