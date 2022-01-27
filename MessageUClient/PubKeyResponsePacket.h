#pragma once

#include "ResponsePacketHeader.h"

class PubKeyResponsePacket : public ResponsePacketHeader {
private:
	char* client_id;
	char* public_key;
public:
	PubKeyResponsePacket(responseHeaderUnion* rh, char* cid, char* pubkey) :ResponsePacketHeader(rh->h.version, rh->h.code, rh->h.payload_size) {
		client_id = new char[CMN_SIZE];
		public_key = new char[PUB_KEY_LEN];
		memcpy(client_id, cid, CMN_SIZE);
		memcpy(public_key, pubkey, PUB_KEY_LEN);
	}
	char* get_id() const {
		return client_id;
	}
	char* get_pub_key() const {
		return public_key;
	}
	~PubKeyResponsePacket() {
		delete client_id;
		delete public_key;
	}
};