#ifndef ENCRYPTED_SOCKETS_H
#define ENCRYPTED_SOCKETS_H
#include <string>
#include <map>
#include <string>
#include "PracticalSocket.h"
#include "blowfish.h"
#include "common.h"

// Tells us if we're on Linux or SunOS
#if defined(__SVR4) && defined(__sun)
	#define BIG_END
#else
	#define LITTLE_END
#endif



using namespace std;



// Basically handles all the encryption and local variables common to both TCP/UDP.
// All of our EncryptedUDP/TCPSockets inherit this.
class EncryptedSocket {

	public:
		// Constructors/Destructors
		EncryptedSocket();
		virtual ~EncryptedSocket();
		
		// Instance variables
		//map<string,string> hostMap;	
		Blowfish* bf;
		
		// Socket Functions
		void sendPayload(void* buffer, int& bufferLen);
		int recvPayload(void* buffer, int bufferLen);
		int getPaddedBufferLength(int currBufferLen);
		void padBufferWithNulls(void* paddedBuffer, void* buffer, int& bufferLen);
		void reorderBytes(void* buffer, int bufferLen);
		void helperReorder(void* buffer);
		

};


class EncryptedTCPSocket : public EncryptedSocket {

	public:
		// Constructors/Destructors
		//EncryptedTCPSocket();
		EncryptedTCPSocket(string, int);
		EncryptedTCPSocket(TCPSocket*);
		//~EncryptedTCPSocket();
		
		void sendPayload(void* buffer, int bufferLen);
		int recvPayload(void* buffer, int bufferLen);
		
		// Instance Variables
		TCPSocket* sock;
		
	private:
		// Access for EncryptedTCPServerSocket::accept() connection creation
		friend class EncryptedTCPServerSocket;
		
};

class EncryptedTCPServerSocket : public EncryptedSocket {

	public:
		// Constructors/Destructors
		//EncryptedTCPServerSocket();
		EncryptedTCPServerSocket(string, int);
		//~EncryptedTCPServerSocket();
		
		// Variables
		TCPServerSocket* sock;

		// Functions
		EncryptedTCPSocket* accept();
};

class EncryptedUDPSocket : public EncryptedSocket {

	public:
		// Constructors/Destructors
		EncryptedUDPSocket();
		EncryptedUDPSocket(string, int);
		//~EncryptedUDPSocket();
		
		// Instance Variables
		UDPSocket* sock;
		void sendPayloadTo(void* buffer, int bufferLen, string foreignAddress, int foreignPort);
		int recvPayload(void *buffer,int bufferLen);
		
};

#endif
