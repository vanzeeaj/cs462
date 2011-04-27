#include "client.h"

using namespace std;

Client::Client(){
	this->udpSock = new EncryptedUDPSocket();
}

//Client::Client(string kdcHostname, int kdcPort, string clientHostname, 
//			int clientPort, string serverHostname, int serverPort, 
//			uint64_t nonce, char* keyA, string fileToSend, int packetSize) {
Client::Client(client_info* info, shared_info* sharedInfo){
	this->kdcHostname = info->kdcIP;
	this->kdcPort = info->kdcPort;
	this->clientPort = info->clientPort;
	this->nonce = info->nonce;
	this->key = info->keyA;
	this->sessionKey = new char[maxKeyLen]();
	this->clientHostname = clientHostname;
	this->serverHostname = info->serverIP;
	this->serverPort = info->serverPort;
	this->idb = info->clientIP;
	idb.append(":");
	char* temp = new char[sizeof(int) * 8 + 1];
	snprintf(temp, sizeof(temp), "%d", clientPort);
	idb.append(temp);
	delete(temp);
	this->fileToSend = sharedInfo->fileToSend;
	this->currPacketId = -1;
	e = new Encrypter(key);
}

void Client::initiate(){
	// Start convo with the KDC
	getAuthenticationInfoFromKDC();
	// Auth with Server
	authenticateWithServer();
	startFTP();
}

uint64_t Client::hashF(uint64_t nonce) {
	const long A = 48271; 
	const long M = 2147483647; 
	const long Q = M/A; 
	const long R = M%A; 

	static long state = 1; 
	long t = A * (state % Q) - R * (state / Q); 

	if (t > 0) 
		state = t; 
	else 
		state = t + M; 

	uint64_t retl = (uint64_t)(((double) state/M) * nonce);
	return retl;

}

	/*
	 *	
	 *	BEGIN AUTHENTICATION WITH KDC
	 *	
	 */

void Client::getAuthenticationInfoFromKDC() {
	TCPSocket* kdcSocket = getConnectionWithKDC();
	sendInfoToKDC(kdcSocket);
	getInfoFromKDC(kdcSocket);
	delete (kdcSocket);
}

TCPSocket* Client::getConnectionWithKDC() {
	try {
		// Establish connection with the echo server
		// cout << kdcHostname.size() << endl;
		// cout << kdcHostname.c_str() << "." << endl;
		// string servAddress = udpSock->hostMap[kdcHostname.c_str()];
		// cout << servAddress << endl;
		TCPSocket* clientSocket = new TCPSocket(kdcHostname, kdcPort);
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

	char* paddedIDB = new char[idb.size()];
	strcpy(paddedIDB, idb.c_str());
	uint64_t firstTransLen = (uint64_t)idb.size();
	cout << "first Trans Len = " << firstTransLen << "idb = " << idb << endl;
	int temp = firstTransLen;
	paddedIDB = e->padBuffer(paddedIDB,  &temp);
	firstTransLen = temp;
	cout << "paddedIDB = " << paddedIDB << endl;

	try {
		//These two sends are UNENCRYPTED
		// Send the length to the socket, and then the buffer, for both items
		
		cout << "sending " << firstTransLen << " bytes:" << paddedIDB << endl;		
		
		e->printBytes(&firstTransLen, 8);
		e->htonll(&firstTransLen);
		e->printBytes(&firstTransLen, 8);
		sock->send(&firstTransLen, 8);	
		cout << " e->getEncryptedLength(idb.size() = " <<  e->getEncryptedLength(idb.size()) << endl;
		sock->send(paddedIDB, temp);
		
		cout << "sending " << 8 << " bytes:" << nonce << endl;
		e->htonll(&nonce);
		sock->send(&nonce, 8);
		e->htonll(&nonce);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to KDC: (Step 1)" << endl;
	cout << "  Req = '" << idb << "'" << endl;
	cout << "  N1 = '" << nonce << "'" << endl;
	//cout << "sent stuff" << endl;
	//delete (firstTrans);
	
}

void Client::getInfoFromKDC(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int i=0;
	int transCount = 3;
	char** recvBuff = new char*[transCount];
	char** decryptedBuff = new char*[transCount];
	uint64_t** s = new uint64_t*[transCount];	
	uint64_t recvNonce;
	
	//FOR DOUBLY ENCRYPTED RECEIVES
	int doublyEncCount = 2;
	uint64_t* encSize = new uint64_t[doublyEncCount];
	uint64_t* unEncSize = new uint64_t[doublyEncCount];
	char** doublyEncMessage = new char*[doublyEncCount];
	
	
	
	try {
		// Loops until we receive 5 different buffers
		while (i<transCount){				// size of the incoming buffer
				cout << "\n\nstarting new receive:" << endl;
				s[i] = new uint64_t;
				sock->recv(s[i], 8);
				cout << "received enc size: ";
				e->printBytes(s[i], 8);
				s[i] = (uint64_t*) e->decrypt(s[i], 8);
				e->htonll(s[i]);
				cout << "decrypted Size = " << *s[i] << endl;
				recvBuff[i] = new char [*s[i]];			// create the buffer with that size (used to be *s + 1)
				//decryptedBuff[i] = new char[*s[i] + 1]; // create the buffer with size + 1 to add \0 to the end.
				sock->recv(recvBuff[i],*s[i]);				// receive the buffer
				cout << "received: ";
				e->printBytes(recvBuff[i], *s[i]);
			i++;
		}
		
		for (i = 0; i < doublyEncCount; i++){
			//Receive size encrypted w/ A
			sock->recv(&(unEncSize[i]), 8);
			//Decrypt size encrypted w/ A so now it's unencrypted
			unEncSize[i] = *(uint64_t*)e->decrypt(&(unEncSize[i]), 8);
			e->htonll(&(unEncSize[i]));
			cout << "SIZE IS........ " << (unEncSize[i]) << endl;
			
			//Receive size encrypted w/ B then A
			sock->recv(&(encSize[i]), 8);
			//Decrypt size encrypted w/ B then A so now it's only encrypted with B
			encSize[i] = *(uint64_t*)e->decrypt(&(encSize[i]), 8);
			
			//Allocate memory for message
			doublyEncMessage[i] = new char[unEncSize[i]];
			//Receive message of size unencrypted first message
			sock->recv(doublyEncMessage[i], unEncSize[i]);
			//Decrypt this message
			doublyEncMessage[i] = e->decrypt(doublyEncMessage[i], unEncSize[i]);
		}
		
		for(int i = 0; i < transCount; i++){
			decryptedBuff[i] = e->decrypt(recvBuff[i], *s[i]);
		}
		
		cout << "After Decryption from KDC: (Step 2)" << endl;
		cout << "  decryptedBuff[0] = '" << decryptedBuff[0] << "'" << endl;
		cout << "  decryptedBuff[1] = '" << decryptedBuff[1] << "'" << endl;
		cout << "  decryptedBuff[2] = '" << (*(uint64_t*)decryptedBuff[2]) << "'" << endl;
		
		
		recvNonce = (*(uint64_t*)decryptedBuff[2]);
		e->htonll(&recvNonce);
		sessionKey = decryptedBuff[0];
		string cs = decryptedBuff[1];	
		
		if (cs.find(idb) == -1) {
			cerr << "Did not receive our request back.  Terminating" << endl;
			exit(1);
		}
		if (recvNonce != nonce) {
			cerr << "Did not receive our nonce back.  Terminating" << endl;
			exit(1);	
		}
		cipher1Length = encSize[0];
		cipher1LengthUnenc = unEncSize[0];
		cipherToSendToServer1 = doublyEncMessage[0];
		cipher2Length = encSize[1];
		cipher2LengthUnenc = unEncSize[1];
		cipherToSendToServer2 = doublyEncMessage[1];
		
		// change this after decryption done:
		cout << "  Ks(decrypted) = '" << recvBuff[0] << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	
	e->changeKey(sessionKey);
	
	delete [] recvBuff;
}

	/*
	 *	
	 *	END AUTHENTICATION WITH KDC
	 *	
	 */
	 
	/*
	 *	
	 *	BEGIN AUTHENTICATION WITH SERVER
	 *	
	 */

void Client::authenticateWithServer() {
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
		//string servAddress = udpSock->hostMap[serverHostname];
		TCPSocket* ServerSocket = new TCPSocket(serverHostname, serverPort);
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
	cout << "  cipherToSendToServer1='" << cipherToSendToServer1 << "'" << endl;
	//unsigned int firstTransLen = strlen(cipherToSendToServer1);
	//char* firstTrans = new char [firstTransLen];
	//memcpy(firstTrans, cipherToSendToServer1, firstTransLen);
	cout << "  cipherToSendToServer2='" << cipherToSendToServer2 << "'" << endl;
	//unsigned int secondTransLen = strlen(cipherToSendToServer2);
	//char* secondTrans = new char [secondTransLen];
	//memcpy(secondTrans, cipherToSendToServer2, secondTransLen);


	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&cipher1Length, 8);
		sock->send(cipherToSendToServer1, cipher1LengthUnenc);
		sock->send(&cipher2Length, 8);
		sock->send(cipherToSendToServer2, cipher2LengthUnenc);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to B: (Step 3)" << endl;
	cout << "  first cypher  = '" << cipherToSendToServer1 << "'" << endl;
	cout << "  second cypher = '" << cipherToSendToServer2 << "'" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	//cout << "sent stuff" << endl;
	
}


void Client::receiveNonce2(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int transCount = 1;
	char** recvBuff = new char*[transCount];
	
	try {
		uint64_t recvNonce;
		uint64_t nonceSize;
		sock->recv(&nonceSize,8);
		nonceSize = *((uint64_t*)e->decrypt(&nonceSize, 8));
		e->htonll(&nonceSize);
		
		sock->recv(&recvNonce,nonceSize);
		cout << "Received from B: (Step 4)" << endl;
		cout << "  Eks[N2] = '" << recvNonce << "'" << endl;
		// TODO Decrypt Ks Nonce2
		cout << "Decrypted nonceSize should be 8: " << nonceSize << endl;
		recvNonce = *((uint64_t*)e->decrypt(&recvNonce, nonceSize));
		e->htonll(&recvNonce);
		nonce2ReceivedFromServer = recvNonce;
		cout << "  N2(decrypted) = '" << recvNonce << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

void Client::sendMutatedNonce2(TCPSocket* sock) {
	uint64_t firstTransLen = 8;
	uint64_t encrFirstTransLen;
	encryptedAndMutatedNonceToSend = hashF(nonce2ReceivedFromServer);
		
	//cout << "Sending cyphers to peer" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "Sent to B: (Step 5)" << endl;	
	

	try {
		// Send the length to the socket, and then the buffer, for both items
		e->htonll(&firstTransLen);
		encrFirstTransLen = *(uint64_t*)e->encrypt(&firstTransLen, 8);
		sock->send(&encrFirstTransLen, 8);
		e->htonll(&firstTransLen);
		
		
		cout << "  mutatedNonce(unencrypted)='" << encryptedAndMutatedNonceToSend << "'" << endl;
		e->htonll(&encryptedAndMutatedNonceToSend);
		encryptedAndMutatedNonceToSend = *(uint64_t*)e->encrypt(&encryptedAndMutatedNonceToSend, firstTransLen);
		cout << "  mutatedNonce(encrypted)='" << encryptedAndMutatedNonceToSend << "'" << endl;
		sock->send(&encryptedAndMutatedNonceToSend, firstTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "  Eks[f(N2)]" << endl;
	//cout << "sent stuff" << endl;
	
}

void Client::receiveOkay(TCPSocket* sock) {
	try {
		uint64_t s;					// Confirmation int sent by server (Should be 0)
		sock->recv(&s, 8);
		s = *(uint64_t*)e->decrypt(&s, 8); //ALLOCATES BYTE(S)!
		e->htonll(&s);
		
		cout << "s (should be 0)  = " << s << endl;
		
		if (s) {
			cout << "Denied \"okay\" message by B! (Step 6)" << endl;
			exit(1);
		}
		cout << "Received \"okay\" message from B (Step 6)" << endl;
		
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}
	/*
	 *	
	 *	END AUTHENTICATION WITH SERVER
	 *	
	 */
	 
	/*
	 *	
	 *	BEGIN FTP
	 *	
	 */
	 
void Client::startFTP(){
	cout << "Starting FTP..." << endl;
	initUDPSocket();
	cout << "Initialized UDP socket" << endl;
	beginSend();
}

	 
void Client::initUDPSocket(){
	udpSock = new EncryptedUDPSocket(clientHostname, clientPort);
	udpSock->bf->Set_Passwd(sessionKey);
	theIfstream.open(fileToSend.c_str());
	delete activeSocket;
}

void Client::beginSend(){
	if (algorithm.compare(0,2,"sw")) {
		cout << "Stop and Wait" << endl;
		windowSize = 1;
		runSR();
	} else if (algorithm.compare(0,8,"goback-n")) {
		cout << "Go Back-N" << endl;
		runGoBackN();
	} else if (algorithm.compare(0,2,"sr")){ 
		cout << "Selective Repeat" << endl;
		runSR();
	} else {
		cerr << "Unrecognized transfer algorithm specified in config.cfg.  Please use 'sw', 'goback-n', 'sr'." << endl;
		cerr << "exiting..." << endl;
		exit(1);
	}
}

void Client::runSR(){
	Packet p;
	p.payload = new char[packetSize];
	cout << "Initialized packet" << endl;
	readNextPacketFromFile(&p);
	if (p.id != -1) {
		udpSock->sendPayloadTo(&p, sizeof(Packet), serverHostname, serverPort);
		cout << "Sent packet " << currPacketId << "." << endl;
	}
	
	cout << "Listening for ack." << endl;
	Ack a;
	udpSock->recvPayload((void*)&a, (int) sizeof(Ack));
	cout << "Ack received" << endl;
	cout << "Ack number was " << a << endl;
}

void Client::runGoBackN(){

}

void Client::readNextPacketFromFile(Packet* p){
	if (!theIfstream.eof()){
		currPacketId++;
		theIfstream >> p->payload;
		p->id = currPacketId;	
		cout << "Packet " << currPacketId << " created." << endl;
	} else {
		p->id = -1;
		theIfstream.close();
	}
}


