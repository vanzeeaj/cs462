#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include "EncryptedSockets.h"
#include "common.h"


class Client {
	public:
		// constructors, deconstructors
		Client();
		~Client();
		void initiate();
		
		// Instance variables
		uint64_t nonce;					// our nonce (used in authenticating)
		char* key;						// our private key (used for authenticating)
		char* sessionKey;				// our session key (used for encrypting/decrypting lists)
		char* request;
		char* cipherToSendToServer1;
		char* cipherToSendToServer2;
		uint64_t nonce2ReceivedFromServer;
		uint64_t encryptedAndMutatedNonceToSend;
		string kdcHostname;
		string serverHostname;
		int serverPort;
		int kdcPort;
		int listeningPort;
		TCPSocket* activeSocket;
		
		// Auth, Crypt and Socket functions
		uint64_t hashF(uint64_t);				// function given to us by dr. tan
		TCPSocket* getConnectionWithKDC();
		void getAuthenticationInfoFromKDC();			// generates our session key
		void authenticateWithServer();
		
		TCPSocket* getConnectionWithServer();
		void sendCyphers(TCPSocket*);
		void receiveNonce2(TCPSocket*);
		void sendMutatedNonce2(TCPSocket*);
		void receiveOkay(TCPSocket*);
		
		EncryptedUDPSocket* udpSock;
		
	
	private:
		
		// Helper functions
		void sendInfoToKDC(TCPSocket*);	
		void getInfoFromKDC(TCPSocket*);
};

#endif
