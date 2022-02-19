#pragma once

#include "PacketStructs.h"

class RequestPacketHeader {
protected:
	requestHeaderUnion* rh;
public:
	RequestPacketHeader(uint16_t code) {
		rh = new requestHeaderUnion;
		rh->h.version = VERSION;
		rh->h.code = code;
		rh->h.payload_size = ZERO;
	}
	RequestPacketHeader(const char* my_id, uint16_t code) : RequestPacketHeader(code) {
		memcpy(rh->h.client_id, my_id, CMN_SIZE);
	}
	requestHeaderUnion* getHeader() const {
		return rh;
	}
	~RequestPacketHeader() {			// TODO if we delete object of type derived class through this base class pointer, this destructor needs to be virtual in order for it to be called
		delete rh;
	}
};
