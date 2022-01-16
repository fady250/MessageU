#pragma once

#include "RequestHeader.h"

class RegisterPacket : RequestHeader {
private:
	registerPacketPayload* p;
public:
	RegisterPacket() :RequestHeader() {
		p = new registerPacketPayload;
	}
	~RegisterPacket() {
		delete p;
	}
};