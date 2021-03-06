#include "client.h"
#include "common.h"
#include "PracticalSocket.h"

// --------------------------------------------
// Constructors/destructors
// --------------------------------------------


Client::Client(){}

Client::~Client() {}

// void Client::init(uint64_t n, char* k, bool i, int s) {
	// if (s%16384 != 0) {
		// this->arraySize = (s/16384)*16384 + 16384;	// truncate and add 16384 
	// } else {
		// this->arraySize = s;
	// }
	// this->userInputSize = s;
	// this->foundPrimes = new vector<long>();
	// this->marked = (char*) calloc(arraySize, sizeof (char));
	// this->key = k;
	// this->nonce = n;
	// this->request = "request for ks";
	// this->initiator = i;
	// this->done = false;
	// this->listenPort = 36537;
	// this->sendPort = listenPort+1;
	// this->recvBuffSize = RCVBUFSIZE;
	// this->cipherToSendToServer1= NULL;
	// this->cipherToSendToServer2= NULL;
	// this->cipherToReceiveFromServer1= NULL;
	// this->cipherToReceiveFromServer2= NULL;
	// this->bf = new Blowfish();
		// initiate();	// starts the whole thing if we're the initiator
	// } else {
		// listenForCommunication();	// otherwise, just sits and waits
	// }
// }


// --------------------------------------------
// Begin
// --------------------------------------------

void Client::initiate(){
	// Start convo with the KDC
	getSessionKeyFromKDC();
	// Auth with Server
	getAuthenticationFromServer();

}

// --------------------------------------------
// Communicate with KDC
// --------------------------------------------

void Client::getSessionKeyFromKDC() {
	TCPSocket* kdcSocket = getConnectionWithKDC();
	sendInfoToKDC(kdcSocket);
	getInfoFromKDC(kdcSocket);
	delete (kdcSocket);
}

TCPSocket* Client::getConnectionWithKDC() {
	try {
		// Establish connection with the echo server
		string servAddress = SHIVA_ADDRESS;
		unsigned short echoServPort = KDC_PORT;
		TCPSocket* clientSocket = new TCPSocket(servAddress, echoServPort);
		return clientSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}
	
void Client::sendInfoToKDC(TCPSocket* sock) {
	
	//cout << "Sending request to KDC" << endl;

	// Our 2 buffers we want to send and their lengths
	unsigned int firstTransLen = strlen(request);
	char* firstTrans = new char [firstTransLen];
	memcpy(firstTrans, request, firstTransLen);
	unsigned int secondTransLen = 8;
	// char* secondTrans = new char [secondTransLen];
	// memcpy(secondTrans, nonce, secondTransLen);

	try {
		// Send the length to the socket, and then the buffer, for both items
		cout << "sending " << firstTransLen << " bytes:" << firstTrans << endl;
		sock->send(&firstTransLen, 4);
		sock->send(firstTrans, firstTransLen);
		cout << "sending " << secondTransLen << " bytes:" << nonce << endl;
		sock->send(&secondTransLen, 4);
		sock->send(&nonce, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to KDC: (Step 1)" << endl;
	cout << "  Req = '" << firstTrans << "'" << endl;
	cout << "  N1 = '" << nonce << "'" << endl;
	//cout << "sent stuff" << endl;
	delete (firstTrans);
	//delete (secondTrans);
	
}

void Client::getInfoFromKDC(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int i=0;
	int transCount = 5;
	char** recvBuff = new char*[transCount];
	uint64_t recvNonce;
	
	try {
		// Loops until we receive 5 different buffers
		while (i<transCount){
			unsigned int s;					// size of the incoming buffer
			if (i==2){
				recvBuff[i] = new char [4];
				sock->recv(&s, 4);
				sock->recv(&recvNonce, s);
			} else {
				sock->recv(&s, 4);
				recvBuff[i] = new char [s];		// create the buffer with that size
				sock->recv(recvBuff[i],s);		// receive the buffer
				cout << "received:" << recvBuff[i] << ", size:" << s << ",iteration " << i << endl;
			}
			i++;
		}
		cout << "Received from KDC: (Step 2)" << endl;
		cout << "  recvBuff[0] = '" << recvBuff[0] << "'" << endl;
		cout << "  recvBuff[1] = '" << recvBuff[1] << "'" << endl;
		cout << "  recvBuff[2] = '" << recvNonce << "'" << endl;
		cout << "  recvBuff[3] = '" << recvBuff[3] << "'" << endl;
		cout << "  recvBuff[4] = '" << recvBuff[4] << "'" << endl;
		cout << "  Eka[Ks] = '" << recvBuff[0] << "'" << endl;
		// TODO DECRYPT RECVBUFF[0-4] WITH Ka
		sessionKey = recvBuff[0];
		if (strcmp(recvBuff[1], request)) {
			cerr << "Did not receive our request back.  Terminating" << endl;
			exit(1);
		}
		if (recvNonce != nonce) {
			cerr << "Did not receive our nonce back.  Terminating" << endl;
			exit(1);	
		}
		cipherToSendToServer1 = recvBuff[3];
		cipherToSendToServer2 = recvBuff[4];
		
		// change this after decryption done:
		cout << "  Ks(decrypted) = '" << recvBuff[0] << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

// --------------------------------------------
// Authenticate with Server (B)
// --------------------------------------------

void Client::getAuthenticationFromServer() {
	TCPSocket* ServerSocket = getConnectionWithServer();
	sendCyphers(ServerSocket);
	receiveNonce2(ServerSocket);
	sendMutatedNonce2(ServerSocket);
	receiveOkay(ServerSocket);
	cout << "Finished receiving authentication from Server" << endl;
	activeSocket = ServerSocket;
}

TCPSocket* Client::getConnectionWithServer() {
	try {
		// TODO
		string servAddress = CLARK_ADDRESS; // Server is always clark
		unsigned short echoServPort = SERVER_PORT;
		TCPSocket* ServerSocket = new TCPSocket(servAddress, echoServPort);
		return ServerSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}

void Client::sendCyphers(TCPSocket* sock) {
	
	//cout << "Sending cyphers to Server" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "  cipherToSendToClient1='" << cipherToSendToClient1 << "'" << endl;
	unsigned int firstTransLen = strlen(cipherToSendToClient1);
	char* firstTrans = new char [firstTransLen];
	memcpy(firstTrans, cipherToSendToClient1, firstTransLen);
	cout << "  cipherToSendToClient2='" << cipherToSendToClient2 << "'" << endl;
	unsigned int secondTransLen = strlen(cipherToSendToClient2);
	char* secondTrans = new char [secondTransLen];
	memcpy(secondTrans, cipherToSendToClient2, secondTransLen);


	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(firstTrans, firstTransLen);
		sock->send(&secondTransLen, 4);
		sock->send(secondTrans, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to B: (Step 3)" << endl;
	cout << "  first cypher  = '" << firstTrans << "'" << endl;
	cout << "  second cypher = '" << secondTrans << "'" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	//cout << "sent stuff" << endl;
	delete (firstTrans);
	delete (secondTrans);
	
}

void Client::receiveNonce2(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
//	int i=0;
	int transCount = 1;
	char** recvBuff = new char*[transCount];
	
	try {
		// Loops until we receive 5 different buffers
		// while (i<transCount){
			// unsigned int s;					// size of the incoming buffer
			// sock->recv(&s, 4);
			// recvBuff[i] = new char [s];		// create the buffer with that size
			// sock->recv(recvBuff[i],s);		// receive the buffer
			// //cout << "received:" << recvBuff[i] << ", size:" << s << ",iteration " << i << endl;
			// i++;
		// }
		uint64_t recvNonce;
		uint32_t nonceSize;
		sock->recv(&nonceSize,4);
		sock->recv(&recvNonce,nonceSize);
		cout << "Received from B: (Step 4)" << endl;
		cout << "  Eks[N2] = '" << recvNonce << "'" << endl;
		// TODO DECRYPT RECVBUFF[0] WITH Ks
		// sessionKey = recvNonce;
		// if (strcmp(recvBuff[1], request)) {
		// 	cerr << "Did not receive our request back.  Terminating" << endl;
		// 	exit(1);
		// }
		// if (strcmp(recvBuff[2], nonce)) {
		// 	cerr << "Did not receive our nonce back.  Terminating" << endl;
		// 	exit(1);	
		// }
		nonce2ToReceiveFromClient = recvNonce;
		// change this after decryption done:
		cout << "  N2(decrypted) = '" << recvNonce << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

void Client::sendMutatedNonce2(TCPSocket* sock) {
	
	
	mutatedNonceToSend = hashF(nonce2ToReceiveFromPeer);
	
	//cipherToSendToPeer1 = recvBuff[3];
	
	
	//cout << "Sending cyphers to peer" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "Sent to B: (Step 5)" << endl;
	cout << "  mutatedNonceToSend='" << mutatedNonceToSend << "'" << endl;
	
	// TODO: Encrypt this with Eks
	
	
	cout << "  mutatedNonce(encrypted)='" << mutatedNonceToSend << "'" << endl;
	
	unsigned int firstTransLen = 8;
	//char* firstTrans = new char [firstTransLen];
	//memcpy(firstTrans, mutatedNonceToSend, firstTransLen);

	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(&mutatedNonceToSend, firstTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "  Eks[f(N2)]" << endl;
	//cout << "sent stuff" << endl;
	//delete (firstTrans);
	
}

void Client::receiveOkay(TCPSocket* sock) {
	try {
		unsigned int s;					// size of the incoming buffer
		sock->recv(&s, 4);
		
		if (s!=1) {
			cout << "Denied \"okay\" message by B! (Step 6)" << endl;
			exit(1);
		}
		cout << "Received \"okay\" message from B (Step 6)" << endl;
		
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}


// --------------------------------------------
// Peer (B): Begin
// --------------------------------------------

void Client::listenForCommunication() {
	//cout << "Client initiated " << endl;
	unsigned short myPort = PEER_PORT;
	try {
		peerServerSocket = new TCPServerSocket(myPort);
		
		// Accept incoming connections (blocking)
		initiatorClientSocket = peerServerSocket->accept();
		
		// Connection initiated, time to do things.
		cout << "Client connected..." << endl;
		
		receiveCyphers(initiatorClientSocket);
		sendNonce2(initiatorClientSocket);
		receiveMutatedNonce2(initiatorClientSocket);
		sendOkay(initiatorClientSocket);
		
		cout << "Finished giving authentication to peer" << endl;
		delete(peerServerSocket);
	
		activeSocket = initiatorClientSocket;
	
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}

void Client::receiveCyphers(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	cipherToReceiveFromPeer1 = new char [f];
	sock->recv(cipherToReceiveFromPeer1, f);
	cout << "cipherToReceiveFromPeer1='" << cipherToReceiveFromPeer1 << "'" << endl;
	// memcpy(cipherToReceiveFromPeer1,recvBuf1,f);
	
	//cout << "grabbed cypher 1:" << cipherToReceiveFromPeer1 << endl;
	
	// Same as above, except for second transmission
	unsigned int s;
	sock->recv(&s, 4);
	//cout << "received len:" << s << endl;
	cipherToReceiveFromPeer2 = new char [s];
	sock->recv(cipherToReceiveFromPeer2, s);
	cout << "cipherToReceiveFromPeer2='" << cipherToReceiveFromPeer2 << "'" << endl;
	//memcpy(cipherToReceiveFromPeer2, recvBuf2, s);
	//cout << "grabbed cypher 2:" << cipherToReceiveFromPeer2 << endl;
	
	cout << "Received from A: (Step 3)" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	
	// TODO: Decrypt
	
	// Change this next line after decryption is done:
	cout << "  Ks(decrypted) = '" << cipherToReceiveFromPeer1 << "'" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	
	sessionKey = cipherToReceiveFromPeer1;
	
	// delete (recvBuf1);
	// delete (recvBuf2);
	
	//cout << "finished recv" << endl;

}

void Client::sendNonce2(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t nonceSize = 8;
	uint64_t cipherNonce = nonce;
	uint32_t cipher1Size = 8;

	//char* cipherToPeer1 = new char [cipher1Size];
	//cipherToPeer1 = nonce + '\0';

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// SEND 1
		//shivaSize = reorderBytesToLittleEndian(&cipher1Size);
		// TODO ENCRYPT WITH Ks
		sock->send(&nonceSize, 4);
		sock->send(&cipherNonce, cipher1Size);
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 4)" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	cout << "  Eks[N2] = '" << cipherNonce << endl;
}

void Client::receiveMutatedNonce2(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	//mutatedNonceToReceive = new char [f];
	uint64_t mutatedNonceToReceive;
	sock->recv(&mutatedNonceToReceive, f);
	cout << "mutatedNonceToReceive='" << mutatedNonceToReceive << "'" << endl;
	// memcpy(mutatedNonceToReceive,recvBuf1,f);
	
	//cout << "grabbed cypher 1:" << mutatedNonceToReceive << endl;
	
	cout << "Received from A: (Step 5)" << endl;
	cout << "  Eks[f(N2)]" << endl;
	
	// TODO: Decrypt
	
	// Change this next line after decryption is done:
	cout << "  f(N2) (decrypted) = '" << mutatedNonceToReceive << "'" << endl;
	
	// delete (recvBuf1);
	// delete (recvBuf2);
	
	//cout << "finished recv" << endl;
	uint64_t tempnonce = hashF(nonce);
	
	if (mutatedNonceToReceive != tempnonce) {
		cout << "Received f(N2) != local f(N2).  Exiting..." << endl;
		exit(1);
	}

}

void Client::sendOkay(TCPSocket* sock) {
	cout << "Sending \"okay\" message to A (Step 6)" << endl;
	int k = 1;
	sock->send(&k, 4);
}

uint64_t Client::hashF(uint64_t nonce) {
	const long A = 48271; 
	const long M = 2147483647; 
	const long Q = M/A; 
	const long R = M%A; 

	static long state = 1; 
	long t = A * (state % Q) - R * (state / Q); 

	//cout << t << "=t" << endl;

	if (t > 0) 
		state = t; 
	else 
		state = t + M; 

	uint64_t retl = (uint64_t)(((double) state/M) * nonce);
	return retl;

}


