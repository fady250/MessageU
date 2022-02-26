#include "SessionManager.h"
#include "misc.h"

SessionManager::SessionManager() {
	if (std::filesystem::exists(std::filesystem::current_path().string() + "\\my.info")) {
		// Create an RSA decryptor based on the existing private key
		string privkey = get_my_private_key();
		rsapriv = new RSAPrivateWrapper(Base64Wrapper::decode(privkey));
	}
	else {
		rsapriv = new RSAPrivateWrapper();		// automatically generates pair of keys 
	}
	
	pt = new packetTransmitter();
	pr = new packetReciever();
}

void SessionManager::handle_user_request(tcp::socket& sock, requestCode rc, string input) {

	RequestPacketHeader* request = nullptr;	// only pointer to hold the object
	char my_cid[CMN_SIZE];	//only 16 bytes , keep on the stack ...

	if (rc == requestCode::userRegister) {
		request = new RegisterPacket((uint16_t)rc, input, rsapriv->getPublicKey().c_str());
	}
	else{
		get_my_id(my_cid);
		if (rc == requestCode::clientsList || rc == requestCode::pullMsgs) 
			request = new RequestPacketHeader(my_cid, (uint16_t)rc);		// header only 
		else if (rc == requestCode::pullClientPubKey) {
			string rec_cid = get_recepient_id_by_name(input);	// get the client id from the list of available clients 
			if (rec_cid.empty()) {
				throw exception("The user you try to send to doesnt exist !");
			}
			request = new PubKeyPullPacket(my_cid, (uint16_t)rc, rec_cid.c_str());		
		}
		else {
			return;// never reach here
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
	string local_input;
	
	// get the client id from the list of available clients 
	string recepient_id = get_recepient_id_by_name(input);
	if (recepient_id.empty()) {
		throw exception("The user you try to send to doesnt exist !");
	}
	RequestPacketHeader* request = nullptr;
	
	if (mt == msgType::textMsgSend) {
		// get the message here as input from user and send it
		std::cout << "Enter the text message : ";
		std::getline(std::cin, local_input);

		//encrypt message with symmetric key 
		for (auto& e : clients) {
			if (e.getId() == recepient_id) {
				unsigned char* sym_key = e.get_sym_key();
				if (sym_key) {
					AESWrapper aes(sym_key, AESWrapper::DEFAULT_KEYLENGTH);
					std::string encrypted_msg = aes.encrypt(local_input.c_str(), local_input.length());
					request = new MsgSendPacket(my_cid, recepient_id.c_str(), (uint8_t)mt, encrypted_msg);
				}
			}
			else {
				throw exception("Encryption key not found !");
			}
		}
	}
	else if (mt == msgType::symKeySend) {
		// encrypt my symKey with recepient public key  
		for (auto& e : clients) {
			if (e.getId() == recepient_id) {
				// get the public key from the clients list for this specific client that i want to send this message to
				unsigned char* pub_key = e.get_pub_key();
				if (!pub_key) {
					throw exception("Cant process request - no public key available");
				}
				else {
					// create new symmetric key , each symKeySend will cretae new key and overwrite existing one 
					AESWrapper aes;					// generates new random key 
					e.set_sym_key(aes.getKey());	// set it in the recepient client entry 
					//create an RSA encryptor
					string pubkey = misc::convertToString((char *)pub_key, PUB_KEY_LEN);
					RSAPublicWrapper rsapub(pubkey);
					std::string encrypted = rsapub.encrypt((const char*)aes.getKey(), AESWrapper::DEFAULT_KEYLENGTH);
					request = new MsgSendPacket(my_cid, recepient_id.c_str(), (uint8_t)mt, encrypted);	
				}
			}
		}
	}
	else {
		request = new MsgSendPacket(my_cid, recepient_id.c_str(), (uint8_t)mt, "");	// for symkey request - no content and no encryption 
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
		outfilex.open("my.info");
		if (!outfilex) {
			throw exception("Unable to create file my.info !");
		}
		name = string(((RegisterPacket*)request)->getPay()->h.name) +"\n";	// The name is null terminated , its safe to send it to string ctor 
		outfilex.write(name.c_str(), name.size());	
		cid = misc::convertToString(((RegisterSuccessPacket*)response)->getPay(), CMN_SIZE);// my id from server 
		for (int i = 0; i < CMN_SIZE; i++)
			outfilex << std::hex << std::setfill('0') << std::setw(2) << ((int)cid[i] & 0xff);
		outfilex << endl;
		base64key = Base64Wrapper::encode(rsapriv->getPrivateKey());
		outfilex.write(base64key.c_str(),base64key.size());	
		outfilex.close();
		break;

	case((uint16_t)responseCode::clientList):
		clients.clear();	// in case get the list again
		client_vec = ((ClientListPacket*)response)->getPay();
		cout << "List of Clients : " << endl;
		if (client_vec->size() == 0) {
			cout << "There are no Clients" << endl;
		}
		else {
			for (unsigned int i = 0; i < client_vec->size(); i++) {
				clients.push_back(client_vec->at(i));	
				cout << client_vec->at(i).getName() << endl;
			}
		}
		break;

	case((uint16_t)responseCode::pubKey):
		// add the info to the client entry
		cid = misc::convertToString(((PubKeyResponsePacket*)response)->get_id(),CMN_SIZE);
		for (auto& e : clients) {
			if (e.getId() == cid) {
				// update the entry in the list with the public key of that client that we just received from server
				e.set_pub_key(((PubKeyResponsePacket*)response)->get_pub_key());	
				cout << e.getName() << "'s Public key was received " << endl;
			}
		}
		break;

	case((uint16_t)responseCode::msgSent):
		cout << "message was sent successfully ( may not be read yet )" << endl;
		break;

	case((uint16_t)responseCode::msgPull):
		msg_vec = ((MessagePacket*)response)->getPay();
		for (unsigned int i = 0; i < msg_vec->size(); i++) {
			name = get_name_by_id(msg_vec->at(i).getPayHeader()->p.client_id);
			cout << "From: " << name << "\nContent: " << endl;

			msgType type = (msgType)(msg_vec->at(i).getPayHeader()->p.msg_type);

			if ( type == msgType::symKeyReq) {

				cout << "Request for symmetric key" << endl;

			}else if(type == msgType::symKeySend){

				cout << "Symmetric key received" << endl;
				// decrypt using my private key 
				string decrypted = rsapriv->decrypt(msg_vec->at(i).getMsg());
				// update sym_key in the entry of the client that sent it 
				for (auto& e : clients) {
					if (e.getId() == misc::convertToString(msg_vec->at(i).getPayHeader()->p.client_id, CMN_SIZE)) {
						e.set_sym_key((unsigned char*)decrypted.c_str());
					}
				}
			}
			else if (type == msgType::textMsgSend) {

				// decrypt the message 
				for (auto& e : clients) {
					if (e.getId() == misc::convertToString(msg_vec->at(i).getPayHeader()->p.client_id, CMN_SIZE)) {
						unsigned char* sym_key = e.get_sym_key();
						if (!sym_key) {
							throw exception("Decryption key not found !");
						}
						else {
							try {
								AESWrapper aes_decription(sym_key, AESWrapper::DEFAULT_KEYLENGTH);	// create object for decrypting the message with the sender relevant symmetric key
								std::string decrypted = aes_decription.decrypt(msg_vec->at(i).getMsg().c_str(), msg_vec->at(i).getMsg().length());
								std::cout << decrypted << endl;
							}
							catch (std::exception& ) {
								std::cout << "Failed to decrypt message" << endl; // dont throw, allow moving to next message
							}
						}
					}
				}
				cout << "-----<EOM>------" << endl;
			}
		}
		break;

	case((uint16_t)responseCode::error):
		cout << "Server responded with an error !" << endl;
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
		for (unsigned int i = 0; i < line.size() && i < CMN_SIZE*2; i+=2) {
			// convert back each two hex characters into raw data (extended ASCII value)
			std::istringstream iss(line.substr(i, 2));
			iss >> std::hex >> temp;
			outBuf[i / 2] = static_cast<char>(temp);
		}
	}
}

string SessionManager::get_my_private_key() const
{
	ifstream input_stream("my.info");
	if (!input_stream) {
		throw exception("Can't open input my.info!");
	}
	else {
		string line, private_key;
		getline(input_stream, line);	// ignore name 
		getline(input_stream, line);	// ignore uuid
		while (std::getline(input_stream, line))
		{
			private_key += line;
		}
		return private_key;
	}
}

string SessionManager::get_recepient_id_by_name(string name) const
{
	for (const auto& e : clients) {		// use the original client entry object to get the correct buffer pointer

		if (misc::convertNullTerminatedToString(e.getName()) == name) {
			return e.getId();
		}
	}
	return "";
}

string SessionManager::get_name_by_id(char* id) const
{
	for (ClientEntry e : clients) {
		if (e.getId() == misc::convertToString(id, CMN_SIZE)) {
			return string(e.getName());
		}
	}
	return "";
}

SessionManager::~SessionManager() {
	delete rsapriv;
	delete pt;
	delete pr;
}

