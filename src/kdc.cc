//placeholder

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <memory.h>
#include <cstring>
#include "kdc.h"
#include "common.h"
#include "EncryptedSockets.h"

using namespace std;

KDC::KDC(char* newSessionKey, int newClientCount, char** newClientKeys, 
			int kdcPort){//, char* serverHostname, int serverPort) {
	sessionKey = newSessionKey;
	clientCount = newClientCount;
	clientKeys = newClientKeys;
	size    = new char[recvBuffSize];
	request = new char[recvBuffSize];
	clientIDs = new char*[clientCount]();
	for (int i=0;i<clientCount;i++) clientIDs[i] = new char[recvBuffSize];
	this->kdcPort = kdcPort;
	//this->serverHostname = serverHostname;
	//this->serverPort = serverPort;
	kdcSocket = NULL;
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
		cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		cerr << "Unable to get foreign address" << endl;
	}
	try {
		cout << sock->getForeignPort();
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

	cout << "Receiving Information from Client!\n";
	flush(cout);
	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	uint16_t f = 0;
	sock->recv(&f, 4);
	flush(cout);
	char* recvBuf1 = new char [f];
	sock->recv(recvBuf1, f);
	memcpy(request,recvBuf1,f);
	//cout << "grabbed request:" << request << endl;
	cout << "Received from A: (Step 1)" << endl;
	cout << "  Req = '" << request << "' (len = " << f << ")" << endl;
	
	// Same as above, except for second transmission
	uint32_t s;
	sock->recv(&s, 4);
	//cout << "received len:" << s << endl;
	sock->recv(&nonce, 8);
	//cout << "grabbed nonce:" << nonce << endl;
	cout << "  N1 = '" << nonce << "' (len = " << s << ")" << endl;

	delete (recvBuf1);
	//cout << "finished recv" << endl;

}

void KDC::sendToTCPClient(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t sessionKeySize = strlen(sessionKey);
	uint32_t requestSize = strlen(request);
	uint32_t nonceSize = 8;
	uint32_t cipher1Size = sessionKeySize+1;
	uint32_t idaSize = strlen(clientIDs[0]);

	char* cipherToPeer1 = new char [cipher1Size];
	cipherToPeer1 = sessionKey + '\0';
	char* cipherToPeer2 = new char [idaSize];
	cipherToPeer2 = clientIDs[0];

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// Setting up variables we'll need for blowfish and endian stuff
		uint32_t sendSize = sessionKeySize;

		// SEND 1
		//sendSize = htonl(sessionKeySize);
		sock->send(&sendSize, 4);
		sock->send(sessionKey, htonl(sessionKeySize));
		
		// SEND 2
		//sendSize = reorderBytesToLittleEndian(&requestSize);
		// TODO ENCRYPT WITH Ka
		sock->send(&sendSize, 4);
		sock->send(request, requestSize);

		// SEND 3
		//sendSize = reorderBytesToLittleEndian(&nonceSize);
		//reorderLong(&sendNonce);
		// TODO ENCRYPT WITH Ka
		uint64_t sendNonce = nonce;
		sock->send(&sendSize, 4);
		sock->send(&sendNonce,nonceSize);
		

		// SEND 4
		//sendSize = reorderBytesToLittleEndian(&cipher1Size);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&sendSize, 4);
		sock->send(cipherToPeer1, cipher1Size);

		// SEND 5
		//sendSize = reorderBytesToLittleEndian(&idaSize);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&sendSize, 4);
		sock->send(cipherToPeer2, idaSize);
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 2)" << endl;
	cout << "  Eka[Ks||Request||N1||Ekb(Ks, IDa)]" << endl;
}



