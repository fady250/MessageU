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
	~RequestPacketHeader() {
		delete rh;
	}
};
