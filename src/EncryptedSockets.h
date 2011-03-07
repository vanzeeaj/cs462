#ifndef ENCRYPTED_SOCKETS_H
#define ENCRYPTED_SOCKETS_H
#include <string>
#include <map>
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
		map<string,string> hostMap;	
		Blowfish* bf;
		
		// Socket Functions
		void sendPayload(void* buffer, int& bufferLen);
		int recvPayload(void* buffer, int bufferLen);
		int getPaddedBufferLength(int currBufferLen);
		void padBufferWithNulls(void* paddedBuffer, void* buffer, int& bufferLen);
		void reorderBytes(void* buffer, int bufferLen);
		void helperReorder(void* buffer);
		
		
	private:
		
		// Private Helper Functions
		/**
		 *  Takes in a buffer and pads it with nulls 
		 *  until a multiple of 8 bytes is reached.  This
		 *  should only be called on the last packet of 
		 *  our	file transfer, or any irregularly sized 
		 *  packets we might want to send. We pad to 8
		 *  bytes so our blowfish can encrypt properly.
		 *  @param buffer 	- incoming buffer to pad with nulls
		 *	@param bufferLen- incoming buffer len; is mutated to match outgoing buffer len.
		 *  @return 		- pointer to new buffer padded with nulls
		 */

		//void cleanNullsFromBuffer(void* buffer, int bufferLen);   // never need

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
		
		// From Parent Classes //
		// Socket:
		// void setLocalAddressAndPort(string &localAddress, short localPort = 0)
		
		TCPServerSocket* sock;

		EncryptedTCPSocket* accept();
};

class EncryptedUDPSocket : public EncryptedSocket {

	public:
		// Constructors/Destructors
		EncryptedUDPSocket();
		EncryptedUDPSocket(string, int);
		//~EncryptedUDPSocket();
		
		UDPSocket* sock;
		
};

#endif
