#pragma once

#include "PacketStructs.h"

class ResponsePacketHeader {
protected:
	responseHeaderUnion* rh;
public:
	ResponsePacketHeader(uint8_t version, uint16_t code, uint32_t payload_size) {
		rh = new responseHeaderUnion;
		rh->h.version = version;
		rh->h.code = code;
		rh->h.payload_size = payload_size;
	}
	responseHeaderUnion* getHeader() const {
		return rh;
	}
	~ResponsePacketHeader() {
		delete rh;
	}
};