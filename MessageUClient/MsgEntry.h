#pragma once

class MsgEntry {
private:
	msgPullPayloadUnion* mppu = nullptr;
	char* message_content = nullptr;
public:
	MsgEntry() {
		mppu = new msgPullPayloadUnion;
	}
	MsgEntry(msgPullPayloadUnion* m) : MsgEntry() {
		memcpy(mppu->buf, m->buf, sizeof(msgPullPayloadUnion));
	}
	MsgEntry(const MsgEntry& me) : MsgEntry(me.getPayHeader()) {			// the first ctor will build the union and copy it 
		//build and copy the message content
		set_msg(me.getMsg().c_str());
	}
	msgPullPayloadUnion* getPayHeader()const {
		return mppu;
	}
	std::string getMsg()const {
		return misc::convertToString(message_content, mppu->p.msg_size);
	}

	void set_msg(const char* msg) {
		message_content = new char[mppu->p.msg_size];
		memcpy(message_content, msg, mppu->p.msg_size);
	}

	~MsgEntry() {
		delete mppu;
		delete message_content;
	}
};