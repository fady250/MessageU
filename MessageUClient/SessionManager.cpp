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
	char my_cid[CMN_SIZE];	//only 16 bytes , keep on the stack ...

	if (rc == requestCode::userRegister) {
		request = new RegisterPacket((uint16_t)rc, input, rsapriv->getPublicKey().c_str());
	}
	else{
		get_my_id(my_cid);
		if (rc == requestCode::clientsList || rc == requestCode::pullMsgs) 
			request = new RequestPacketHeader(my_cid, (uint16_t)rc);		// header only 
		if (rc == requestCode::pullClientPubKey) {
			char* rec_cid = get_recepient_id_by_name(input);
			if (!rec_cid) {
				throw exception("The user you try to send to doesnt exist !");
			}
			request = new PubKeyPullPacket(my_cid, (uint16_t)rc, rec_cid);		// get the client id from the list of available clients 
		}
	}

	pt->send(sock, request);
	pr->recieve(sock);										// receive the response 
	handle_server_response(pr, request);
	delete request;
}

//reach here only when rc == requestCode::sendMsg
void SessionManager::handle_user_request(tcp::socket& sock, msgType mt, string input) {
	
	char my_cid[CMN_SIZE];	//only 16 bytes , keep on the stack ...
	get_my_id(my_cid);		// if its one of the below requests i should already have cid ( these requests are not reachable if user didnt register )
	
	// get the client id from the list of available clients 
	char* recepient_id = get_recepient_id_by_name(input);
	if (!recepient_id) {
		throw exception("The user you try to send to doesnt exist !");
	}
	RequestPacketHeader* request = nullptr;
	
	if (mt == msgType::textMsgSend) {
		// get the message here as input from user and send it downstream	//TODO
		std::cout << "Enter the text message : ";
		std::getline(std::cin, input);		// TODO never trust input ???

		// TODO encrypt message with my symmetric key 
		//std::string encrypted_msg = aes->encrypt(input.c_str(), input.length());
	
		request = new MsgSendPacket(my_cid, recepient_id, (uint8_t)mt,input);
	}
	else if (mt == msgType::symKeySend) {
		// encrypt my symKey with recepient public key 
		// TODO where does the rec pub key comes from ? check 
		request = new MsgSendPacket(my_cid, recepient_id, (uint8_t)mt, "");		//TODO
	}
	else {
		request = new MsgSendPacket(my_cid, recepient_id, (uint8_t)mt, "");	// for symkey request and file send - no content
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
	string cid, name, base64key;// key;
	ResponsePacketHeader* response = pr->getPacket();

	switch (response->getHeader()->h.code)
	{
	case((uint16_t)responseCode::registerSucc):
		outfilex.open("my.info");// , std::ios_base::binary);
		if (!outfilex) {
			throw exception("Unable to create file my.info !");
		}
		name = string(((RegisterPacket*)request)->getPay()->h.name) +"\n";
		outfilex.write(name.c_str(), name.size());
		cid = string(((RegisterSuccessPacket*)response)->getPay());	// my id from server 
		for (int i = 0; i < CMN_SIZE; i++)
			outfilex << std::hex << std::setfill('0') << std::setw(2) << ((int)cid[i] & 0xff);
		outfilex << endl;
		//key = string(rsapriv->getPrivateKey());
		//cout << key.size();
		//outfilex.write(key.c_str(), key.size());	
		base64key = Base64Wrapper::encode(rsapriv->getPrivateKey());
		outfilex.write(base64key.c_str(),base64key.size());	
		//cout << base64key.size(); // TODO clean
		outfilex.close();
		break;

	case((uint16_t)responseCode::clientList):
		clients.clear();	// in case get the list again
		client_vec = ((ClientListPacket*)response)->getPay();
		cout << "List of Clients : " << endl;
		for (int i = 0; i < client_vec->size(); i++) {
			clients.push_back(client_vec->at(i));		// TODO CC should be used here 
			cout << client_vec->at(i).getName() << endl;
		}
		break;

	case((uint16_t)responseCode::pubKey):
		// add the info to the client entry
		cid = misc::convertToString(((PubKeyResponsePacket*)response)->get_id(),CMN_SIZE);
		for (auto& e : clients) {
			if (misc::convertToString(e.getId(),CMN_SIZE) == cid) {
				// update the entry in the list with the public key of that client that we just received from server
				e.set_pub_key(((PubKeyResponsePacket*)response)->get_pub_key());	
				// TODO clean 
				std::cout << "public key that was received : " << std::endl;
				std::string pubk(((PubKeyResponsePacket*)response)->get_pub_key(), PUB_KEY_LEN);
				for (int i = 0; i < pubk.size(); i++)
					std::cout << std::hex << std::setfill('0') << std::setw(2) << ((int)pubk[i] & 0xff);
			}
		}
		break;

	case((uint16_t)responseCode::msgSent):
		cout << "message was sent successfully ( may not be read yet )" << endl;
		break;

	case((uint16_t)responseCode::msgPull):
		msg_vec = ((MessagePacket*)response)->getPay();
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

void SessionManager::get_my_id(char* outBuf) const
{
	ifstream input_stream("my.info");
	if (!input_stream) {
		throw exception("Can't open input my.info!");
	}
	else {
		string line;
		int temp;
		getline(input_stream, line);	// ignore name 
		getline(input_stream, line);
		std::stringstream ss;
		for (int i = 0; i < line.size() && i < CMN_SIZE*2; i+=2) {
			// convert back each two hex characters into raw data (extended ASCII value)
			std::istringstream iss(line.substr(i, 2));
			iss >> std::hex >> temp;
			outBuf[i / 2] = static_cast<char>(temp);
		}
	}
}

char* SessionManager::get_recepient_id_by_name(string name) const
{

	for (const auto& e : clients) {		// use the original client entry object to get the correct buffer pointer

		if (misc::convertNullTerminatedToString(e.getName()) == name) {
			return e.getId();
		}
	}
	//for (ClientEntry e : clients) {
	//
	//	if (misc::convertNullTerminatedToString(e.getName()) == name) {
	//		return e.getId();
	//	}
	//}
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

