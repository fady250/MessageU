#pragma once

#include "ResponsePacketHeader.h"
#include "MsgEntry.h"
#include <vector>

class MessagePacket : public ResponsePacketHeader {
private:
	std::vector<MsgEntry>* lis;
public:
	MessagePacket(responseHeaderUnion* rh) :ResponsePacketHeader(rh->h.version, rh->h.code, rh->h.payload_size) {
		lis = new std::vector<MsgEntry>;
	}
	std::vector<MsgEntry>* getPay() {
		return lis;
	}
	void addEntry(MsgEntry& me) {
		lis->push_back(me);
	}
	~MessagePacket() {
		delete lis;
	}
};