#include "EncryptedSockets.h"

uint64_t htonll(uint64_t v) {
    union { uint32_t lv[2]; uint64_t llv; } u;
    u.lv[0] = htonl(v >> 32);
    u.lv[1] = htonl(v & 0xFFFFFFFFULL);
    return u.llv;
}

uint64_t ntohll(uint64_t v) {
    union { uint32_t lv[2]; uint64_t llv; } u;
    u.llv = v;
    return ((uint64_t)ntohl(u.lv[0]) << 32) | (uint64_t)ntohl(u.lv[1]);
}


EncryptedUDPSocket::EncryptedUDPSocket(int localPort) {
	init(localPort);
}

EncryptedUDPSocket::EncryptedUDPSocket() {
	init(6545);
}

EncryptedUDPSocket::~EncryptedUDPSocket() {
	delete bf;
	delete sock;
}

void EncryptedUDPSocket::init(int localPort){
	this->bf = new Blowfish();
	this->sock = new UDPSocket((short)localPort);
}

EncryptedSocket::EncryptedSocket() {
	hostMap["andy"] = "137.28.8.161";
	hostMap["clark"] = "137.28.8.160";
	hostMap["shiva"] = "137.28.8.143";
}
