#pragma once

#include "PacketStructs.h"

class RequestHeader {
protected:
	requestPacketHeader* rh;
public:
	RequestHeader() {							// TODO fill the struct through the constructor 
		rh = new requestPacketHeader;
	}
	requestPacketHeader* getHeader() const {
		return rh;
	}
	~RequestHeader() {
		delete rh;
	}
};
