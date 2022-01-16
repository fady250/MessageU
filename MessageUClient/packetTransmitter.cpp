#include "packetTransmitter.h"

/*
packetTransmitter::packetTransmitter() {
	constHeader = new responsePacketHeaderConst;	// the rest of the packet will be dynamically allocated according to sizes
}

void packetTransmitter::buildConstHeader(enum class status st) {
	constHeader->hc.version = VERSION;
	constHeader->hc.status = (uint16_t)st;
}

void packetTransmitter::buildFlexHeader(std::string st,uint16_t size) {
	flexHeader = (PacketHeaderFlex*)new char[sizeof(PacketHeaderFlex) + sizeof(char)*size];
	flexHeader->hf.namelen = size;
	memcpy(flexHeader->hf.file_name, st.c_str(),size);
}

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

packetTransmitter::~packetTransmitter() {
	if (constHeader != nullptr) delete constHeader;
	if (flexHeader != nullptr) delete flexHeader;
	if (payChunk != nullptr) delete payChunk;
}
*/