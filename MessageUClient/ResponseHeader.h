#pragma once

#include "PacketStructs.h"

class ResponseHeader {
protected:
	responseHeaderUnion* rh;
public:
	ResponseHeader() {
		rh = new responseHeaderUnion;
	}
	responseHeaderUnion* getHeader() const {
		return rh;
	}
	~ResponseHeader() {
		delete rh;
	}
};