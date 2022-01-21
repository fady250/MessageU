#include "Client.h"

Client::Client()
{
	ifstream input_stream("server.info");
	if (!input_stream) {
		throw exception("Can't open input server.info!");
	}
	else {
		string line;
		getline(input_stream, line);
		size_t pos = line.find(":");
		if (pos != string::npos) {
			address = line.substr(0, pos);
			port = line.substr(pos+1);
			sm = new SessionManager();
		}
		else {
			throw exception("Bad address:port");
		}
	}
}

void Client::start()
{
	boost::asio::io_context io_context;
	tcp::socket s(io_context);
	tcp::resolver resolver(io_context);
	boost::asio::connect(s, resolver.resolve(address,port));

	std::cout << "MessageU client at your service !\n";
	std::cout << "110) Register\n";
	std::cout << "120) Request for clients list\n";
	std::cout << "130) Request for public key\n";
	std::cout << "140) Request for waiting messages\n";
	std::cout << "150) Send a text message\n";
	std::cout << "151) Send a request for symmetric key\n";
	std::cout << "152) Send your symmetric key\n";
	std::cout << "0)   Exit client\n?";

	handle_user_input(s);
}

/*
* get user input from console , check its a valid input  
* calls the handle request with correct request code according to the protocol
*/
void Client::handle_user_input(tcp::socket& sock)
{
	for (std::string input; std::getline(std::cin, input);) {
		// validate input 
		uint8_t code = atoi(input.c_str());
		if (!code && input != "0") {
			cout << "Bad request code, please retry" << endl;
		}
		else {
			if (code == (uint8_t)userInput::exit) break;
			else if (code == (uint8_t)userInput::Register) {
				if (std::filesystem::exists(std::filesystem::current_path().string() + "\\my.info")) {
					cout << "Error ! my.info already exists !" << endl;
				}
				else {
					std::cout << "Enter your name : ";
					std::getline(std::cin, input);
					sm->handle_request(sock, requestCode::userRegister, input);
				}
			}
			else if (code == (uint8_t)userInput::RequestList) { sm->handle_request(sock, requestCode::clientsList, ""); }		
			else if (code == (uint8_t)userInput::RequestpubKey) {									// request client B public key
				std::cout << "Enter username for whom you want to request its public key : ";
				std::getline(std::cin, input);
				sm->handle_request(sock, requestCode::pullClientPubKey, input);
			}		
			else if (code == (uint8_t)userInput::Requestmsgs) {										// request to pull waiting messages for me
				sm->handle_request(sock, requestCode::pullMsgs, ""); 
			}	
			else if (code >= (uint8_t)userInput::SendTxtMsg && code <= (uint8_t)userInput::SendSymKey) {
				std::cout << "Enter target username : ";
				std::getline(std::cin, input);
				if(code == (uint8_t)userInput::SendTxtMsg)											// send text message to client B
					sm->handle_request(sock, msgType::textMsgSend, input);	
				if(code == (uint8_t)userInput::RequestSymKey)										// request client B symmetric key
					sm->handle_request(sock, msgType::symKeyReq, input);
				if (code == (uint8_t)userInput::SendSymKey)											// send my symmetric key to client B
					sm->handle_request(sock, msgType::symKeySend, input);
			}
			else {
				cout << "Bad request code, please retry" << endl;
			}
		}
		std::cout << "?";
	}
}
	

Client::~Client()
{
	if(sm != nullptr) delete sm;
}
