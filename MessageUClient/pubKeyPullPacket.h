#pragma once

#include "requestPacketHeader.h"

class PubKeyPullPacket : public RequestPacketHeader {
private:
	char* client_id;
public:
	PubKeyPullPacket(const char* my_id, uint16_t code, char* cid) :RequestPacketHeader(my_id, code) {
		client_id = new char[CMN_SIZE];
		memcpy(client_id, cid, CMN_SIZE);
	}

	char* getPay() const {
		return client_id;
	}
	~PubKeyPullPacket() {
		delete client_id;
	}
};
