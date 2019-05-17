
#include "settings.h"
#include "ap_int.h"

class AddressCache {

private:
	ap_uint<8> fragments[CACHE_SIZE];
	ap_uint<8> adresses[CACHE_SIZE];

	uint16_t agingIndex = 0;

public:
	AddressCache();
	~AddressCache();
	void get(ap_uint<CHUNK_SIZE> *i_fragment, ap_uint<8> *o_address, bool *valid);
	void set(ap_uint<8> *i_fragment, ap_uint<8> *i_address);
};
