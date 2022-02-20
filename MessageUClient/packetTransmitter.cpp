#include "packetTransmitter.h"

void packetTransmitter::send(tcp::socket& sock, RequestPacketHeader* rp) {
	try {
		//send header 
		boost::asio::write(sock, boost::asio::buffer(rp->getHeader()->buf, sizeof(requestHeaderUnion)));	// for 1101/1104

		if (rp->getHeader()->h.code == (uint16_t)requestCode::userRegister) {
			boost::asio::write(sock, boost::asio::buffer(((RegisterPacket*)rp)->getPay()->buf));			// for 1100
		}
		else if (rp->getHeader()->h.code == (uint16_t)requestCode::pullClientPubKey) {						// for 1102
			boost::asio::write(sock, boost::asio::buffer(((PubKeyPullPacket*)rp)->getPay(), CMN_SIZE));		// pay is the client_id according to packet 
		}
		else if (rp->getHeader()->h.code == (uint16_t)requestCode::sendMsg) {								//  for 1103
			boost::asio::write(sock, boost::asio::buffer(((MsgSendPacket*)rp)->getPay()->buf));				// size is constant allocation of fileds before the content
			boost::asio::write(sock, boost::asio::buffer(((MsgSendPacket*)rp)->getPay()->p.msg_content, ((MsgSendPacket*)rp)->getPay()->p.content_size));	// send the content according to its size
		}
	}
	catch (std::exception& ex) {
		throw std::exception("Error while sending packet");
	}	
}