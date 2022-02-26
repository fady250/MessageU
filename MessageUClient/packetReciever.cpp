#include "packetReciever.h"

packetReciever::packetReciever() { rp = nullptr; }

packetReciever::~packetReciever() {
	if (rp != nullptr) delete rp;
}

void packetReciever::recieve(tcp::socket& sock)
{
	responseHeaderUnion rhu;
	try {
		if (rp != nullptr) delete rp;		// start clean
		size_t length;
		// receive the header 
		length = boost::asio::read(sock, boost::asio::buffer(rhu.buf, sizeof(responseHeaderUnion)));

		if (rhu.h.code == (uint16_t)responseCode::registerSucc) {
			char cid[CMN_SIZE];
			length = boost::asio::read(sock, boost::asio::buffer(cid, CMN_SIZE));
			rp = new RegisterSuccessPacket(&rhu, cid);
		}
		else if (rhu.h.code == (uint16_t)responseCode::clientList) {
			uint32_t numOfClients = rhu.h.payload_size / (CMN_SIZE + MAX_NAME_SIZE);
			rp = new ClientListPacket(&rhu);
			for (unsigned int i = 0; i < numOfClients; i++) {
				char id[CMN_SIZE];
				char name[MAX_NAME_SIZE];
				length = boost::asio::read(sock, boost::asio::buffer(id, CMN_SIZE));
				length = boost::asio::read(sock, boost::asio::buffer(name, MAX_NAME_SIZE));
				ClientEntry ce(id, name);
				((ClientListPacket*)rp)->addEntry(ce);
			}
		}
		else if (rhu.h.code == (uint16_t)responseCode::pubKey) {
			// receive the payload 
			char cid[CMN_SIZE];
			char pub_key[PUB_KEY_LEN];
			length = boost::asio::read(sock, boost::asio::buffer(cid, CMN_SIZE));
			length = boost::asio::read(sock, boost::asio::buffer(pub_key, PUB_KEY_LEN));
			rp = new PubKeyResponsePacket(&rhu, cid, pub_key);
		}
		else if (rhu.h.code == (uint16_t)responseCode::msgSent) {
			// receive the payload 
			char cid[CMN_SIZE];
			char mid[MSG_ID_LEN];
			length = boost::asio::read(sock, boost::asio::buffer(cid, CMN_SIZE));
			length = boost::asio::read(sock, boost::asio::buffer(mid, MSG_ID_LEN));
			rp = new MessageSentPacket(&rhu, cid, mid);
		}
		else if (rhu.h.code == (uint16_t)responseCode::msgPull) {
			uint32_t pay_size = rhu.h.payload_size;
			msgPullPayloadUnion mppu;
			// create packet
			rp = new MessagePacket(&rhu);
			while (pay_size > 0) {
				// receive the structure that is before the variable lenght message content
				length = boost::asio::read(sock, boost::asio::buffer(mppu.buf, sizeof(msgPullPayloadUnion)));
				MsgEntry e(&mppu);
				// receive the message 
				char* msg = new char[mppu.p.msg_size];
				length = boost::asio::read(sock, boost::asio::buffer(msg, mppu.p.msg_size));
				pay_size -= (mppu.p.msg_size + sizeof(msgPullPayloadUnion));
				e.set_msg(msg);
				((MessagePacket*)rp)->addEntry(e);
			}
		}
		else if (rhu.h.code == (uint16_t)responseCode::error) {
			// nothing to receive - WE HAVE THE HEADER READY
			rp = new ResponsePacketHeader(rhu.h.version, rhu.h.code, rhu.h.payload_size);
		}
		else {
			// empty rhu , throw exception
			throw std::exception();
		}
	}
	catch (std::exception&) {
		throw std::exception("Error while receiving packet");
	}

	if (rhu.h.code == (uint16_t)responseCode::error)
		throw std::exception("General error received from server");
}

ResponsePacketHeader* packetReciever::getPacket() const
{
	return rp;
}