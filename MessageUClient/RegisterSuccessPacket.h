#pragma once

#include "ResponsePacketHeader.h"

class RegisterSuccessPacket : public ResponsePacketHeader {
private:
	char* client_id;
public:
	RegisterSuccessPacket(responseHeaderUnion* rh, const char* cid) :ResponsePacketHeader(rh->h.version, rh->h.code, rh->h.payload_size) {
		client_id = new char[CMN_SIZE];
		memcpy(client_id, cid, CMN_SIZE);
	}
	char* getPay() const {
		return client_id;
	}
	~RegisterSuccessPacket() {
		delete[] client_id;
	}
};