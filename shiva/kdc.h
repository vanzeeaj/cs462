#ifndef KDC_H
#define KDC_H

#include <iostream>
#include "PracticalSocket.h"

using namespace std;

class KDC {
private:
public:
	// Instance variables
	char* sessionKey;
	int clientCount;
	char** clientKeys;
	char* request;
	//char* nonce;
	uint64_t nonce;
	char* size;
	char* ida;
	char* idb;
	
	// Cosntructors/destructors
	KDC(char*, int, char**);

	// Socket Stuff 
	void getFromTCPClient(TCPSocket*);
	void sendToTCPClient(TCPSocket*);
	TCPServerSocket* kdcSocket;
	TCPSocket* clientSocket;

	// Thread functions
	//void create_thread();
	void* thread_function(void*);
	friend void* proxy_function(void*);	

	// Misc. functions
	void execute();
	uint32_t reorderBytes(uint32_t*);
	// These next 2 functions are to keep things straight in my head.
	// They simply call the function above, as it works both ways.
	uint32_t reorderBytesToLittleEndian(uint32_t*);
	uint32_t reorderBytesToBigEndian(uint32_t*);
	uint64_t reorderLong(uint64_t* i);

	
};

#endif
