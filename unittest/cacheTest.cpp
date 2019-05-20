// testing framework
#include "catch.hpp"

// module to test
#include "../addresscache.h"

// fundamentals
#include "ap_int.h"

TEST_CASE( "Get element: cache hit", "[AddressCache]" ) {
	AddressCache cache;
	ap_uint<CHUNK_SIZE_BITS> fragment = 42;
	uint32_t address = 56;
	bool valid = false;

	uint32_t retrievedAddress;

	cache.set(&fragment, &address);
	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == true;
	bool hitTest = address == retrievedAddress;

	REQUIRE(hitTest);
	REQUIRE(validTest);
}

TEST_CASE( "Get element: cache hit with full cache", "[AddressCache]" ) {
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

	REQUIRE(hitTest);
	REQUIRE(validTest);
}

TEST_CASE( "Get element: cache miss", "[AddressCache]" ) {
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

	REQUIRE(validTest);
}

TEST_CASE( "Get element: cache miss, element aged out", "[AddressCache]" ) {
	AddressCache cache;
	uint32_t retrievedAddress;
	ap_uint<CHUNK_SIZE_BITS> insertFragment = 0;
	ap_uint<CHUNK_SIZE_BITS> fragment = 0;
	bool valid = false;

	for(uint32_t i = 0; i < CACHE_SIZE + 1; i++) {
		cache.set(&insertFragment, &i);
		insertFragment++;
	}

	cache.get(&fragment, &retrievedAddress, &valid);

	bool validTest = valid == false;

	REQUIRE(validTest);
}
