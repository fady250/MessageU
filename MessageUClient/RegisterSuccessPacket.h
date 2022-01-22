#pragma once

#include "ResponsePacketHeader.h"

class RegisterSuccessPacket : public ResponsePacketHeader {
private:
	char* client_id;
public:
	RegisterSuccessPacket(uint8_t version, uint16_t code, uint32_t payload_size, char* cid) :ResponsePacketHeader(version, code, payload_size) {
		client_id = new char[CMN_SIZE];
		memcpy(client_id, cid, CMN_SIZE);
	}
	char* getPay() const {
		return client_id;
	}
	~RegisterSuccessPacket() {
		delete client_id;
	}
};