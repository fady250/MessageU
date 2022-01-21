#pragma once

#include "PacketStructs.h"

class requestPacketHeader {
protected:
	requestHeaderUnion* rh;
public:
	requestPacketHeader(char* my_id, uint16_t code) {
		rh = new requestHeaderUnion;
		memcpy(rh->h.client_id, my_id, CMN_SIZE);
		rh->h.version = VERSION;
		rh->h.code = code;
		rh->h.payload_size = ZERO;
	}
	requestHeaderUnion* getHeader() const {
		return rh;
	}
	~requestPacketHeader() {
		delete rh;
	}
};
