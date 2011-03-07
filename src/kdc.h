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
		int kdcPort;
		int serverPort;
		string serverHostname;
		string ida;				
		
		// Cosntructors/destructors
		KDC(char* newSessionKey, int newClientCount, char** newClientKeys, 
			int kdcPort);//, char* serverHostname, int serverPort);
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
