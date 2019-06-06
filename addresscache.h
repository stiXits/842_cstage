
#include "settings.h"
#include "ap_int.h"
#include "stdint.h"

class AddressCache {

private:
	ap_uint<CHUNK_SIZE_BITS> fragments[CACHE_SIZE];
	uint32_t adresses[CACHE_SIZE];
	bool valid[CACHE_SIZE];

	uint16_t agingIndex = 0;

public:
	AddressCache();
	~AddressCache();
	void get(ap_uint<CHUNK_SIZE_BITS> *i_fragment, uint32_t *o_address, bool *valid);
	void set(ap_uint<CHUNK_SIZE_BITS> *i_fragment, uint32_t *i_address);
};
