#pragma once
#include<stdint.h>

// defines 
#define CID_SIZE		16		// 16 bytes
//#define REQ_CONST_H_LEN 6		// request header lenght in bytes w/o the file_name len and buffer
//#define REQ_FLEX_H_LEN	2
//#define RES_CONST_H_LEN 3		// response header lenght in bytes w/o the file_name len and buffer
//#define RES_FLEX_H_LEN  2
//#define PAY_LEN	        4		// payload size field
#define CHUNK_SIZE		1024	// payload chunk size in bytes 

#pragma pack(push,1)

struct requestHeader {	
	uint8_t client_id[CID_SIZE];
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct responseHeader {	
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct payloadChunk {
	char data[CHUNK_SIZE];
};

// payload structs here, according to the request 


#pragma pack(pop)

// unions :

union requestPacketHeader
{
	requestHeader h;
	char buf[sizeof(requestHeader)];
};

union responsePacketHeaderConst
{
	responseHeader h;						
	char buf[sizeof(responseHeader)];
};

// payload unions :



