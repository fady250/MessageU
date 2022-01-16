#pragma once

#include "RequestHeader.h"

class pubKeyPullPacket : RequestHeader {
private:
	pubKeyPullPayload* p;
public:
	pubKeyPullPacket() :RequestHeader() {
		p = new pubKeyPullPayload;
	}
	~pubKeyPullPacket() {
		delete p;
	}
};
