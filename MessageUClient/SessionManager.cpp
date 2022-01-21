#include "SessionManager.h"

SessionManager::SessionManager() {
	aes = new AESWrapper();					// automatically generate random key ( symmetric key )
	rsapriv = new RSAPrivateWrapper();		// automatically generates private key 
	pt = new packetTransmitter();
	pr = new packetReciever();
	my_id = new char[CMN_SIZE];
}

char* SessionManager::get_recepient_id(string name) const
{
	return nullptr;
}

//TODO move the common between the below 2 functions to private method
void SessionManager::handle_request(tcp::socket& sock, requestCode rc, string input) {

	requestPacketHeader* rp = nullptr;

	if (rc == requestCode::userRegister) 
		rp = new RegisterPacket(my_id, (uint16_t)rc, input, rsapriv->getPublicKey().c_str());
	if (rc == requestCode::clientsList || rc == requestCode::pullMsgs)
		rp = new requestPacketHeader(my_id, (uint16_t)rc);		// header only 
	if (rc == requestCode::pullClientPubKey) {}
	//	rp = new PubKeyPullPacket(my_id,(uint16_t)rc,)		// get the client id from the list of available clients 

	pt->send(sock, rp);
	delete rp;
	pr->recieve(sock);										// receive the response 

	// parse and print the response // TODO

}

void SessionManager::handle_request(tcp::socket& sock, msgType mt, string input) {

	// get the client id from the list of available clients 
	char* rec_id = 0;
	requestPacketHeader* rp = nullptr;
	//reach here only when rc == requestCode::sendMsg
	if (mt == msgType::textMsgSend) {
		// get the message here as input from user and send it downstream	//TODO
		std::cout << "Enter the text message : ";

		// encrypt message with my symmetric key 
		rp = new MsgSendPacket(my_id, rec_id, (uint8_t)mt, ZERO,"");
	}
	else if (mt == msgType::symKeySend) {
		// encrypt my symKey with recepient public key 
		rp = new MsgSendPacket(my_id, rec_id, (uint8_t)mt, ZERO, "");		//TODO
	}
	else {
		rp = new MsgSendPacket(my_id, rec_id, (uint8_t)mt, ZERO, "");	// for symkey request and file send - no content
	}
	
	pt->send(sock, rp);
	delete rp;
	pr->recieve(sock);										// receive the response 

}

SessionManager::~SessionManager() {
	delete aes;
	delete rsapriv;
	delete pt;
	delete pr;
	delete my_id;
}