#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include "common.h"
#include "EncryptedSockets.h"



class Server {

	public:
		// constructors and deconstructors
		Server();
		~Server();
		
		// Instance variables
		TCPServerSocket* theServerSocket;
		TCPSocket* clientSocket;
		TCPSocket* activeSocket;	
		
		void listenForCommunication();
		char* cipherToReceiveFromPeer1;
		char* cipherToReceiveFromPeer2;
		uint64_t nonce2ToReceiveFromPeer;
		uint64_t mutatedNonceToReceive;
		
		// Auth, Crypt and Socket functions
		void receiveCyphers(TCPSocket*);
		void sendNonce2(TCPSocket*);
		void receiveMutatedNonce2(TCPSocket*);
		void sendOkay(TCPSocket*);
		uint64_t hashF(uint64_t);				// function given to us by dr. tan
			
		int localId;					// our ID (pid + host ip)
		int listenPort;					// the port we listen on
		int sendPort;					// the port we send to
		int connectedClientId;			// client's ID that we send our list to
		unsigned int recvBuffSize;
		char* sessionKey;
		uint64_t nonce;
		
		
	private:
		
};

#endif
