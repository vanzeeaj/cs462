
// --------------------------------------------
// Begin
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