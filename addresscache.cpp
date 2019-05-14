#include "addresscache.h"

#include "stdint.h"

void AddressCache::get(ap_uint<CHUNK_SIZE> *i_fragment, ap_uint<8> *o_address) {

	// maybe use ap int with log(CACHE_SIZE) bits
	for(uint16_t i; i < CACHE_SIZE; i++) {
		if(this->fragments[i] == *i_fragment) {
			*o_address = this->adresses[i];
		}
		else {
			o_address = NULL;
		}
	}
}

void AddressCache::set(ap_uint<CHUNK_SIZE> *i_fragment, ap_uint<8> *i_address) {
	for(uint16_t i; i < CACHE_SIZE; i++) {
		if(this->fragments[i] == NULL) {
			this->fragments[i] = *i_fragment;
			this->adresses[i] = *i_fragment;

			// clear out next array element for reeeaaally simple aging

			// do it like a ring buffer
			if(i == CACHE_SIZE - 1) {
				this->fragments[0] = NULL;
				this->adresses[0] = NULL;
			} else {
				this->fragments[i + 1] = NULL;
				this->adresses[i+ 1] = NULL;
			}
		}
	}
}
