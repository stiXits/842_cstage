#include "addresscache.h"

#include "stdint.h"

AddressCache::AddressCache() {

}

AddressCache::~AddressCache() {

}

void AddressCache::get(ap_uint<CHUNK_SIZE_BITS> *i_fragment, uint32_t *o_address, bool *valid) {

	// maybe use ap int with log(CACHE_SIZE) bits
	for(uint16_t i; i < CACHE_SIZE; i++) {
		if(this->fragments[i] == *i_fragment) {
			*o_address = this->adresses[i];
			*valid = true;
			return;
		}
		else {
			*valid = false;
		}
	}
}

void AddressCache::set(ap_uint<CHUNK_SIZE_BITS> *i_fragment, uint32_t *i_address) {

			this->fragments[agingIndex] = *i_fragment;
			this->adresses[agingIndex] = *i_address;

			if(agingIndex == CACHE_SIZE - 1) {
				agingIndex = 0;
			}
			else {
				agingIndex++;
			}
}
