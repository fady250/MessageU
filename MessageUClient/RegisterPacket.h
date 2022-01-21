#pragma once

#include "requestPacketHeader.h"

class RegisterPacket : public requestPacketHeader {
private:
	registerPayloadUnion* p;
public:
	RegisterPacket(char* my_id, uint16_t code, std::string name,const char* pubkey) :requestPacketHeader(my_id,code) {
		// update payload size 
		rh->h.payload_size = REGISTER_PAY_SIZE;
		p = new registerPayloadUnion;
		size_t s = name.size() < MAX_NAME_SIZE ? name.size() : MAX_NAME_SIZE;
		memcpy(p->h.name, name.c_str(), s);
		p->buf[s] = '\0';
		memcpy(p->h.public_key, pubkey, PUB_KEY_SIZE);
	}
	registerPayloadUnion* getPay() const {
		return p;
	}
	~RegisterPacket() {
		delete p;
	}
};