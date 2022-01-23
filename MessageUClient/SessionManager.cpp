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
		char* cid = get_recepient_id_by_name(input);
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
	char* recepient_id = get_recepient_id_by_name(input);
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
	vector<ClientEntry>* client_vec;
	vector<MsgEntry>* msg_vec;
	string cid, name;
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
		client_vec = ((ClientListPacket&)response).getPay();
		cout << "List of Clients : " << endl;
		for (int i = 0; i < client_vec->size(); i++) {
			clients.push_back(client_vec->at(i));		// TODO CC should be used here 
			cout << client_vec->at(i).getName() << endl;
		}
		break;

	case((uint16_t)responseCode::pubKey):
		// add the info to the client entry
		cid = misc::convertToString(((PubKeyResponsePacket&)response).get_id(),CMN_SIZE);
		for (ClientEntry e : clients) {
			if (misc::convertToString(e.getId(),CMN_SIZE) == cid) {
				// update the entry in the list with the public key of that client that we just received from server
				e.set_pub_key(((PubKeyResponsePacket&)response).get_pub_key());		
			}
		}
		break;

	case((uint16_t)responseCode::msgSent):
		cout << "message was sent successfully ( may not be read yet )" << endl;
		break;

	case((uint16_t)responseCode::msgPull):
		msg_vec = ((MessagePacket&)response).getPay();
		for (int i = 0; i < msg_vec->size(); i++) {
			name = get_name_by_id(msg_vec->at(i).getPayHeader()->p.client_id);
			cout << "From: " << name << "\nContent: " << endl;
			// decrypt 
			cout << msg_vec->at(i).getMsg() << endl;
			cout << "-----<EOM>------" << endl;
		}
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

char* SessionManager::get_recepient_id_by_name(string name) const
{
	for (ClientEntry e : clients) {

		if (misc::convertNullTerminatedToString(e.getName()) == name) {
			return e.getId();
		}
	}
	return nullptr;	// TODO throw ?
}

string SessionManager::get_name_by_id(char* id) const
{
	for (ClientEntry e : clients) {
		string s(id);
		if (misc::convertNullTerminatedToString(e.getId()) == s) {
			return string(e.getName());
		}
	}
	return string("");	// TODO throw 
}

SessionManager::~SessionManager() {
	delete aes;
	delete rsapriv;
	if (pt != nullptr) delete pt;
	if (pr != nullptr) delete pr;
}

