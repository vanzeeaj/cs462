#ifndef KDC_H
#define KDC_H

#include <iostream>
#include "EncryptedSockets.h"

void* proxy_function(void*);

class KDC {
	
	public:
		// Instance variables
		int clientCount;
		char* sessionKey;
		char* request;
		uint64_t nonce;
		char* size;
		char** clientKeys;
		char** clientIDs;
		int localPort;
		
		// Cosntructors/destructors
		KDC(char*, int, char**);
		void execute();
		
		// Socket Stuff 
		void getFromTCPClient(TCPSocket*);
		void sendToTCPClient(TCPSocket*);
		TCPServerSocket* kdcSocket;
		TCPSocket* clientSocket;

		// Thread functions
		void* thread_function(void*);
	
};

	
#endif
