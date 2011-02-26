#ifndef ENCRYPTED_SOCKETS_H
#define ENCRYPTED_SOCKETS_H
#include <netinet/in.h>
#include "PracticalSocket.h"
#include "blowfish.h"


using namespace std;

// Functions we're going to need for byte reordering of 64bit numbers (long longs)
// (32bit definitions already included with netinet/in.h)
uint64_t htonll(uint64_t v);
uint64_t ntohll(uint64_t v);

class EncryptedUDPSocket : public UDPSocket {
	
	protected:
		// Constructors/Destructors
		EncryptedUDPSocket();
		EncryptedUDPSocket(int);
		~EncryptedUDPSocket();
		
		// Encryption Functions
		void setPassword(char*);
		void resetPassword();
		
		// Socket Functions
		void setLocalAddressAndPort(const string& localAddress, int localPort);
		int getLocalPort();
		string getLocalAddress();
		
		void setDestAddressAndPort(const string& destAddress, int destPort);
		int getDestPort();
		string getDestAddress();
		
		void send(void* buffer, int bufferLen);		// Sends buffer
		int recv(void* buffer, int bufferLen);		// Receives into buffer, returns errno
		
	private:
		// Instance variables
		Blowfish* bf;
		UDPSocket* sock;
		string localAddress;
		string destAddress;
		int localPort;
		int destPort;
		map<string, string> hostMap;		// <HOSTNAME, IP>, instantiated in init()
		
		void init(int);		// Used in constructor chaining
		
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
		void* padBufferWithNulls(void* buffer, int& bufferLen);
		
		/**
		 *  Private encryption and decryption functions called
		 *  when we send or receive from a socket. Makes calls 
		 *  to Blowfish->Encrypt and Blowfish->Decrypt to encrypt 
		 *  or decrypt our payloads.
		 */
		void encryptPayload(void* buffer, int bufferLen);
		void decryptPayload(void* buffer, int bufferLen);
		
		
		
};


#endif
