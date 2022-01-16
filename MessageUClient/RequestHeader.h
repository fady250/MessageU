#pragma once

#include "PacketStructs.h"

class RequestHeader {
protected:
	requestPacketHeader* rh;
public:
	RequestHeader() {
		rh = new requestPacketHeader;
	}
	requestPacketHeader* getHeader() const {
		return rh;
	}
	~RequestHeader() {
		delete rh;
	}
};
