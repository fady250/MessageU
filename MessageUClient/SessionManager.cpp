#include "SessionManager.h"

SessionManager::SessionManager() {
	aes = new AESWrapper();					// automatically generate random key ( symmetric key )
	rsapriv = new RSAPrivateWrapper();		// automatically generates private key 
	pt = new packetTransmitter();
	pr = new packetReciever();
}

//TODO move the common between the below 2 functions to private method
void SessionManager::handle_request(tcp::socket& sock, requestCode rc, string input) {
	requestPacketHeader* header = new requestPacketHeader;
	registerPacketPayload* registerPay = new registerPacketPayload;

	// TODO call sender and receiver ?
	memcpy(header->h.client_id, my_id, CMN_SIZE);
	header->h.version = VERSION;
	header->h.code = (uint16_t)rc;
	
	if (rc == requestCode::userRegister) {
		header->h.payload_size = REGISTER_PAY_SIZE;
		size_t s = input.size() < MAX_NAME_SIZE ? input.size() : MAX_NAME_SIZE;
		memcpy(registerPay->h.name, input.c_str(),s);
		registerPay->buf[s] = '\0';
		memcpy(registerPay->h.public_key, rsapriv->getPublicKey().c_str(), PUB_KEY_SIZE);
		pt->send(sock, header, registerPay);
	}
	// TODO fill the payload in the below 
	if(rc == requestCode::clientsList)
		header->h.payload_size = ZERO;
	if(rc == requestCode::pullClientPubKey)
		header->h.payload_size = CMN_SIZE;			// TODO get the client id 

	
}

void SessionManager::handle_request(tcp::socket& sock, requestCode rc, msgType mt, string input) {

}

SessionManager::~SessionManager() {

}