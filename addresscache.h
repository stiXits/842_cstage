
#include "settings.h"
#include "ap_int.h"

class AddressCache {

private:
	ap_uint<8> fragments[CACHE_SIZE];
	ap_uint<8> adresses[CACHE_SIZE];

public:
	void get(ap_uint<8> *i_fragment, ap_uint<8> *o_address);
	void set(ap_uint<8> *i_fragment, ap_uint<8> *i_address);
};
