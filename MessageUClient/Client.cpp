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
			port = line.substr(pos);
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


	for (std::string input; std::getline(std::cin, input);) {
		std::cout << input << std::endl;
		handle_user_input(input);
		std::cout << "?";
	}
}

void Client::handle_user_input(string input)
{
	// validate input 
	// move from string to other representation ???
	sm->handle_request();
}

Client::~Client()
{
	if(sm != nullptr) delete sm;
}
