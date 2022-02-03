#include "packetTransmitter.h"

packetTransmitter::packetTransmitter() {}

packetTransmitter::~packetTransmitter() {}

void packetTransmitter::send(tcp::socket& sock, RequestPacketHeader* rp) {
	boost::system::error_code ec;
	//send header 
	boost::asio::write(sock, boost::asio::buffer(rp->getHeader()->buf,sizeof(requestHeaderUnion)), ec);		// for 1101/1104

	if (rp->getHeader()->h.code == (uint16_t)requestCode::userRegister) {
		boost::asio::write(sock, boost::asio::buffer(((RegisterPacket*)rp)->getPay()->buf), ec);			// for 1100
	}
	else if (rp->getHeader()->h.code == (uint16_t)requestCode::pullClientPubKey) {							// for 1102
		boost::asio::write(sock, boost::asio::buffer(((PubKeyPullPacket*)rp)->getPay(),CMN_SIZE),ec);		// pay is the client_id according to packet 
		//char buf[CMN_SIZE];
		//memcpy(buf, ((PubKeyPullPacket*)rp)->getPay(), CMN_SIZE);
		//boost::asio::write(sock, boost::asio::buffer(buf));
	}
	else if(rp->getHeader()->h.code == (uint16_t)requestCode::sendMsg) {									//  for 1103
		// if type is file - handle accordingly 

		boost::asio::write(sock, boost::asio::buffer(((MsgSendPacket*)rp)->getPay()->buf), ec);				// size is constant allocation of fileds before the content
		boost::asio::write(sock, boost::asio::buffer(((MsgSendPacket*)rp)->getPay()->p.msg_content, ((MsgSendPacket*)rp)->getPay()->p.content_size), ec);	// send the content according to its size
	}
		
}

/*
void packetTransmitter::send(tcp::socket& soc, packetReciever* pr) {
	boost::system::error_code ec;
	enum class status st = (status)constHeader->hc.status;
	boost::asio::write(soc, boost::asio::buffer(constHeader->buf),ec);

	if (st == status::err_no_files || st == status::error) return;      // doesnt have name  
	
	// continue sending 
	boost::asio::write(soc, boost::asio::buffer(flexHeader->buf), ec);		// since the buf size is known(2) , need to send the buffer after it 
	boost::asio::write(soc, boost::asio::buffer(flexHeader->hf.file_name, flexHeader->hf.namelen), ec);

	if (st != status::succ_restored && st != status::succ_list) return;		// doesnt have payload 

	// handle payload part 

	std::string fname = misc::convertToString(flexHeader->hf.file_name, flexHeader->hf.namelen);
	std::ifstream input_stream;
	if (pr->getConstHeader()->hc.op == (uint16_t)operation::list_files)
		input_stream.open(fname);	// the file that we are opening is located in the same folder where the server is running from
	else {
		fname = "C:\\backupsvr\\" + std::to_string(pr->getConstHeader()->hc.user_id) + "\\" + fname;
		input_stream.open(fname,std::ios::binary);
	}
	if (!input_stream) {
		std::cerr << "Can't open file";
	}
	else {
		if (payChunk == nullptr) payChunk = new payloadChunk;
		// send the payload size 
		uint32_t size = std::filesystem::file_size(fname);
		char sizebuf[PAY_LEN];
		*(uint32_t*)&sizebuf = size;
		boost::asio::write(soc, boost::asio::buffer(sizebuf, PAY_LEN), ec);		
		uint32_t bytes_left = size, bytes_sent = 0;
		// send by chunks - this way we dont allocate memory for the whole file !!!
		while (bytes_left > 0) {
			size = bytes_left > CHUNK_SIZE ? CHUNK_SIZE : bytes_left;	// as long as the bytes left are bigger that the chunk size --> send chunk size else send the remaining bytes left
			input_stream.read(payChunk->data, size);
			boost::asio::write(soc, boost::asio::buffer(payChunk->data, size), ec);
			bytes_sent += size;
			bytes_left -= size;
		}
		input_stream.close();
	}
	if (pr->getConstHeader()->hc.op == (uint16_t)operation::list_files)
		std::filesystem::remove(misc::convertToString(flexHeader->hf.file_name,flexHeader->hf.namelen));	// delete this text file - server doesnt need it
}
*/