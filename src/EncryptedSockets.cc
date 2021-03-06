#include "EncryptedSockets.h"


/* ENCRYPTED SOCKET STUFF */
EncryptedSocket::EncryptedSocket() {
	// char* clark = new char [6];
	// clark[0] = 'c';
	// clark[1] = 'l';
	// clark[2] = 'a';
	// clark[3] = 'r';
	// clark[4] = 'k';
	// clark[5] = '\0';
	// char* andy = new char [5];
	// andy[0] = 'a';
	// andy[1] = 'n';
	// andy[2] = 'd';
	// andy[3] = 'y';
	// andy[4] = '\0';
	// char* shiva = new char [6];
	// shiva[0] = 's';
	// shiva[1] = 'h';
	// shiva[2] = 'i';
	// shiva[3] = 'v';
	// shiva[4] = 'a';
	// shiva[5] = '\0';
	
	// hostMap[andy] = "137.28.8.161";
	// hostMap[clark] = "137.28.8.160";
	// hostMap[shiva] = "137.28.8.143";
	// hostMap["andy\0"] = "137.28.8.161";
	// hostMap["clark\0"] = "137.28.8.160";
	// hostMap["shiva\0"] = "137.28.8.143";
	this->bf = new Blowfish();
}
EncryptedSocket::~EncryptedSocket(){
	delete bf;
}
int EncryptedSocket::getPaddedBufferLength(int currBufferLen){
	if (currBufferLen/8 != (currBufferLen-1)/8) 
		return currBufferLen;
	else
		return currBufferLen/8*8 + 8;
}
void EncryptedSocket::padBufferWithNulls(void* paddedBuffer, void* buffer, int& bufferLen){
	int newBuffLen = getPaddedBufferLength(bufferLen);
	char* newBuff = new char [newBuffLen];
	memcpy(&newBuff, &buffer, newBuffLen);
	while (bufferLen<newBuffLen){
		newBuff[bufferLen] = '\0';
		bufferLen++;
	}
	paddedBuffer = (void*) newBuff;
}

void EncryptedSocket::reorderBytes(void* buffer, int bufferLen){
	#ifdef LITTLE_END
	char* chars = (char*) buffer;
	for (int i=0;i<bufferLen;i+=4){
		helperReorder(&(chars[i]));
	}
	#endif
}

void EncryptedSocket::helperReorder(void* buffer){
	char* chars = (char*) buffer;
	swap(chars[0],chars[3]);
	swap(chars[1],chars[2]);
}


/* ENCRYPTED UDP SOCKET STUFF */
EncryptedUDPSocket::EncryptedUDPSocket(string localAddress, int port) : EncryptedSocket() {
	sock = new UDPSocket(localAddress, port);
}

 EncryptedUDPSocket::EncryptedUDPSocket() : EncryptedSocket(){
	sock = new UDPSocket(34522);	 
}

// EncryptedUDPSocket::~EncryptedUDPSocket() {
	// delete sock;
// }

void EncryptedUDPSocket::sendPayloadTo(void* buffer, int bufferLen, string foreignAddress, int foreignPort){
	sock->sendTo(buffer, bufferLen, foreignAddress, foreignPort);
}

int EncryptedUDPSocket::recvPayload(void *buffer, int bufferLen){
	return sock->recv(buffer, bufferLen);
}



/* ENCRYPTED TCP SOCKET STUFF */

EncryptedTCPSocket::EncryptedTCPSocket(string name, int port) : EncryptedSocket() {
	sock = new TCPSocket(name, port);
}
	
EncryptedTCPSocket::EncryptedTCPSocket(TCPSocket* t) : EncryptedSocket() {
	sock = t;
}

// EncryptedTCPSocket::~EncryptedTCPSocket() {
	// delete sock;
// }

void EncryptedTCPSocket::sendPayload(void* buffer, int bufferLen) {
	cout << "inside send payload" << endl;
	if (getPaddedBufferLength(bufferLen) != bufferLen) {		// aka we need to pad
		cout << "inside if" << endl;
		char* paddedBuffer;
		padBufferWithNulls((void*)paddedBuffer, buffer, bufferLen);	// pads with nulls so we can encrypt
		buffer = (void*) paddedBuffer;
		cout << "padded buffer" << endl;
	}	
	this->reorderBytes(buffer, bufferLen);
	bf->Encrypt(buffer, bufferLen);			
	this->reorderBytes(buffer, bufferLen);
	cout << "encrypted" << endl;
	cout << bufferLen << endl;
	sock->send(buffer, bufferLen);					// sends across network
	cout << "send done" << endl;
}

int EncryptedTCPSocket::recvPayload(void* buffer, int bufferLen) {
	int result = sock->recv(buffer, bufferLen);			// receives into buffer, bufferLen
	this->reorderBytes(buffer, bufferLen);
	bf->Decrypt(buffer, bufferLen);	
	this->reorderBytes(buffer, bufferLen);	
	return result;			
}

/* ENCRYPTED TCP SERVER SOCKET STUFF */
// EncryptedTCPServerSocket::EncryptedTCPServerSocket() : 
		// TCPServerSocket(34510), EncryptedSocket() {}

EncryptedTCPServerSocket::EncryptedTCPServerSocket(string name, int port):EncryptedSocket() {
	sock = new TCPServerSocket(name, port, 5);
}

// EncryptedTCPServerSocket::~EncryptedTCPServerSocket(){
	// delete sock;
// }
	
EncryptedTCPSocket* EncryptedTCPServerSocket::accept() {
	TCPSocket* t = sock->accept();
	return new EncryptedTCPSocket(t);
} 
