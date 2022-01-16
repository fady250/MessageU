#pragma once

#include "PacketStructs.h"

class ResponseHeader {
protected:
	responsePacketHeader* rh;
public:
	ResponseHeader() {
		rh = new responsePacketHeader;
	}
	responsePacketHeader* getHeader() const {
		return rh;
	}
	~ResponseHeader() {
		delete rh;
	}
};