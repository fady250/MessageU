#include "SessionManager.h"
#include "misc.h"

SessionManager::SessionManager() {
	aes = new AESWrapper();					// automatically generate random key ( symmetric key )
	rsapriv = new RSAPrivateWrapper();		// automatically generates private key 
	pt = new packetTransmitter();
	pr = new packetReciever();
}

void SessionManager::handle_user_request(tcp::socket& sock, requestCode rc, string input) {

	RequestPacketHeader* request = nullptr;

	if (rc == requestCode::userRegister) 
		request = new RegisterPacket((uint16_t)rc, input, rsapriv->getPublicKey().c_str());
	if (rc == requestCode::clientsList || rc == requestCode::pullMsgs)
		request = new RequestPacketHeader(get_my_id(), (uint16_t)rc);		// header only 
	if (rc == requestCode::pullClientPubKey) {
		char* cid = get_recepient_id(input);
		if (!cid) {
			throw exception("The user you try to send to doesnt exist !");
		}
		request = new PubKeyPullPacket(get_my_id(), (uint16_t)rc,cid);		// get the client id from the list of available clients 
	}

	pt->send(sock, request);
	pr->recieve(sock);										// receive the response 
	handle_server_response(pr, request);
	delete request;
}

//reach here only when rc == requestCode::sendMsg
void SessionManager::handle_user_request(tcp::socket& sock, msgType mt, string input) {

	// get the client id from the list of available clients 
	char* recepient_id = get_recepient_id(input);
	if (!recepient_id) {
		throw exception("The user you try to send to doesnt exist !");
	}
	RequestPacketHeader* request = nullptr;
	
	if (mt == msgType::textMsgSend) {
		// get the message here as input from user and send it downstream	//TODO
		std::cout << "Enter the text message : ";

		// encrypt message with my symmetric key 
		request = new MsgSendPacket(get_my_id(), recepient_id, (uint8_t)mt, ZERO,"");
	}
	else if (mt == msgType::symKeySend) {
		// encrypt my symKey with recepient public key 
		request = new MsgSendPacket(get_my_id(), recepient_id, (uint8_t)mt, ZERO, "");		//TODO
	}
	else {
		request = new MsgSendPacket(get_my_id(), recepient_id, (uint8_t)mt, ZERO, "");	// for symkey request and file send - no content
	}
	
	pt->send(sock, request);
	pr->recieve(sock);										// receive the response 
	handle_server_response(pr, request);
	delete request;
}

void SessionManager::handle_server_response(packetReciever* pr, RequestPacketHeader* request)
{
	std::ofstream outfilex;
	vector<ClientEntry>* vec;
	ResponsePacketHeader* response = pr->getPacket();

	switch (response->getHeader()->h.code)
	{
	case((uint16_t)responseCode::registerSucc):
		outfilex.open("my.info", std::ios_base::binary);
		if (!outfilex) {
			throw exception("Unable to create file my.info !");
		}
		outfilex.write(((RegisterPacket&)request).getPay()->h.name,MAX_NAME_SIZE);	// TODO try 
		outfilex.write(((RegisterSuccessPacket&)response).getPay(), CMN_SIZE);		// my id from server 
		outfilex.write(rsapriv->getPrivateKey().c_str(), rsapriv->getPrivateKey().size());	// TODo try see lenght 
		outfilex.close();
		break;

	case((uint16_t)responseCode::clientList):
		clients.clear();	// in case get the list again
		vec = ((ClientListPacket&)response).getPay();
		cout << "List of Clients : " << endl;
		for (int i = 0; i < vec->size(); i++) {
			clients.push_back(vec->at(i));		// TODO CC should be used here 
			cout << vec->at(i).getName() << endl;
		}
		break;

	case((uint16_t)responseCode::pubKey):
		break;

	case((uint16_t)responseCode::msgSent):
		break;

	case((uint16_t)responseCode::msgPull):
		break;

	case((uint16_t)responseCode::error):
		if(request->getHeader()->h.code == (uint16_t)requestCode::userRegister)
			cout << "This User name already exists in the server !";
		break;

	default:
		break;
	}
}

const char* SessionManager::get_my_id() const
{
	ifstream input_stream("my.info");
	if (!input_stream) {
		throw exception("Can't open input my.info!");
	}
	else {
		string line;
		getline(input_stream, line);	// ignore name 
		getline(input_stream, line);
		return line.c_str();
	}
}

char* SessionManager::get_recepient_id(string name) const
{
	for (ClientEntry e : clients) {

		if (misc::convertNullTerminatedToString(e.getName()) == name) {
			return e.getId();
		}
	}
	return nullptr;
}

SessionManager::~SessionManager() {
	delete aes;
	delete rsapriv;
	if (pt != nullptr) delete pt;
	if (pr != nullptr) delete pr;
}

