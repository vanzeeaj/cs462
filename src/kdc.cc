#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <memory.h>
#include <cstring>
#include <stdint.h>
#include "kdc.h"
#include "common.h"
#include "EncryptedSockets.h"

using namespace std;

//KDC::KDC(char* newSessionKey, int newClientCount, char** newClientKeys, 
//			int kdcPort){//, char* serverHostname, int serverPort) {
KDC::KDC(KDC_info* info){
	sessionKey = info->sessionKey;
	clientCount = info->clientCount;
	clientKeys = info->clientServerKeys;
	size    = new char[recvBuffSize];
	clientIDs = new char*[clientCount]();
	for (int i=0;i<clientCount;i++) clientIDs[i] = new char[recvBuffSize];
	this->kdcPort = info->kdcPort;
	//this->serverHostname = serverHostname;
	//this->serverPort = serverPort;
	kdcSocket = NULL;
	e = new Encrypter();
}

void KDC::execute() {
	cout << "KDC initiated" << endl;
	try {
		kdcSocket = new TCPServerSocket(kdcPort);
		
		while (true) {
			// Accept incoming connections (blocking)
			clientSocket = kdcSocket->accept();
	
			// Connection initiated, time to do things.
			cout << "Client connected..." << endl;
			// Fork a new thread to handle the connection
			pthread_t threadID;
			if (pthread_create(&threadID, NULL, proxy_function, this) != 0) {
				cerr << "Unable to create thread" << endl;
				exit(1);
			}
		}
		// NEVER REACH THIS
		// would normally delete kdcSocket, but since we don't ever exit cleanly we don't.
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	
}

void* proxy_function(void* kdc_ptr) {
	KDC* k = static_cast<KDC*>(kdc_ptr);
	k->thread_function(k->clientSocket);
	return NULL;
}

void* KDC::thread_function(void* clntSock) {
	// Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	TCPSocket* sock = (TCPSocket*) clntSock;

	// Error catching and screen output
	cout << "Handling client ";
	try {
		ida = sock->getForeignAddress();
		ida.append(":");
		cout << ida;
	} catch (SocketException e) {
		cerr << "Unable to get foreign address" << endl;
	}
	try {
		//CONVERTING AN INTO TO CHAR ARRAY >> UGLY
		char* temp = new char[sizeof(int) * 4 + 1];
		snprintf(temp, sizeof(temp), "%d", sock->getForeignPort());
		ida.append(temp);
		cout << temp;
		delete(temp);
	} catch (SocketException e) {
		cerr << "Unable to get foreign port" << endl;
	}
	cout << " with thread " << pthread_self() << endl;

	// Perform the authentication
	getFromTCPClient(sock);
	sendToTCPClient(sock);

	cout << "Authentication with client complete. Terminating thread " << pthread_self() << endl;
	// Close socket
	delete sock;
	clntSock = NULL;
	return NULL;
}


void KDC::getFromTCPClient(TCPSocket* sock) {

	cout << "Receiving Information from Client!" << endl;
	e->changeKey(clientKeys[0]);
	
	// Grabs the lengths of the transmissions to come
	// Performsthe receive and populates the proper char array
	
	uint64_t f;
	sock->recv(&f, 8);
	//REORDER BYTES
	e->htonll(&f);
	
	request = new char [f];
	sock->recv(request, f);
	
	cout << "Received from A: (Step 1)" << endl;
	cout << "  Req = '" << request << "' (len = " << f << ")" << endl;
	
	// Same as above, except for second transmission
	sock->recv(&nonce, 8);
	//REORDER BYTES
	e->htonll(&nonce);
	
	//cout << "grabbed nonce:" << nonce << endl;
	cout << "  N1 = '" << nonce << "' (len = " << 8 << ")" << endl;
	//cout << "finished recv" << endl;

}

void KDC::sendToTCPClient(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t sessionKeySize = strlen(sessionKey);
	uint32_t requestSize = strlen(request);
	uint32_t nonceSize = 8;
	uint32_t cipher1Size = sessionKeySize;
	uint32_t idaSize = ida.size();

	char* cipherToPeer1 = new char [cipher1Size];
	cipherToPeer1 = sessionKey;
	//char* cipherToPeer2 = new char [idaSize];
	//cipherToPeer2 = clientIDs[0];

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		/*cout << "SENDING THESE VALUES" << endl;
		cout << "SessionKey size: " << sessionKeySize << " sessionKey: " << sessionKey << endl;
		cout << "Request size: " << requestSize << " request: " << request << endl;		
		cout << "nonce size: " << nonceSize << " nonce: " << nonce << endl;
		cout << "cipher1 size: " << cipher1Size << " cipherToPeer1: " << cipherToPeer1 << endl;
		cout << "ida size: " << idaSize << " ida: " << ida << endl;*/
	
	
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// Setting up variables we'll need for blowfish and endian stuff

		//Set up Encrypter
		uint64_t* encryptedSize = new uint64_t;
		
		
		// SEND 1	
		//encrypt with a	
		*encryptedSize = (uint64_t) e->getEncryptedLength(sessionKeySize);
		e->htonll(encryptedSize);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		sock->send(e->encrypt(sessionKey, sessionKeySize), e->getEncryptedLength(sessionKeySize));
		
		
		// SEND 2		
		//encrypt with a
		*encryptedSize = (uint64_t) e->getEncryptedLength(requestSize);
		e->htonll(encryptedSize);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		sock->send(e->encrypt(request, requestSize), e->getEncryptedLength(requestSize));
		
		
		// SEND 3
		//encrypt with a (NONCE)
		*encryptedSize = (uint64_t) e->getEncryptedLength(nonceSize);
		e->htonll(encryptedSize);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		uint64_t sendNonce = nonce;
		sock->send(encryptedSize, 8);
		sendNonce = nonce;
		e->htonll(&sendNonce);
		sendNonce = *(uint64_t*)e->encrypt(&sendNonce, nonceSize);
		sock->send(&sendNonce,  e->getEncryptedLength(nonceSize));
		

		// SEND 4
		//Send size encrypted with a (when receiving assume this is 8 bytes)
		//Send size encrypted with b then a (when receiving assume this is 8 bytes)
		//Send actual message encoded with b then a		
		*encryptedSize = (uint64_t) e->getEncryptedLength(cipher1Size);
		e->htonll(encryptedSize);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		
		//Now encrypt size with b then a so it can be sent to b still encoded.
		*encryptedSize = (uint64_t) e->getEncryptedLength(cipher1Size);
		e->htonll(encryptedSize);
		e->changeKey(clientKeys[1]);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		e->changeKey(clientKeys[0]);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		
		//Now encrypt actual message with b then a
		e->changeKey(clientKeys[1]);		//Change to Server's Key
		char* sessionKeyEncoded = e->encrypt(cipherToPeer1, cipher1Size);
		e->changeKey(clientKeys[0]);		//Change to Client's Key
		sessionKeyEncoded = e->encrypt(sessionKeyEncoded, e->getEncryptedLength(cipher1Size));
		sock->send(sessionKeyEncoded,  e->getEncryptedLength(cipher1Size));
		
		

		// SEND 5
		//Send size encrypted with a
		//Send size encrypted with b then a (when receiving assume this is 8 bytes)
		//Send actual message encoded with b then a		
		*encryptedSize = (uint64_t) e->getEncryptedLength(idaSize);
		e->htonll(encryptedSize);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		
		*encryptedSize = (uint64_t) e->getEncryptedLength(idaSize);
		e->htonll(encryptedSize);
		e->changeKey(clientKeys[1]);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		e->changeKey(clientKeys[0]);
		encryptedSize = (uint64_t*) e->encrypt(encryptedSize, 8);
		sock->send(encryptedSize, 8);
		
		e->changeKey(clientKeys[1]);		//Change to Server's Key
		char* idaEncoded = new char[idaSize];
		strcpy(idaEncoded, ida.c_str());
		idaEncoded = e->encrypt(idaEncoded, idaSize);
		e->changeKey(clientKeys[0]);		//Change to Client's Key
		idaEncoded = e->encrypt(idaEncoded, e->getEncryptedLength(idaSize));
		sock->send(idaEncoded,  e->getEncryptedLength(idaSize));		
		
		
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 2)" << endl;
	cout << "  Eka[Ks||Request||N1||Ekb(Ks, IDa)]" << endl;
}



