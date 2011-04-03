#ifndef NETWORKS_COMMON_H
#define NETWORKS_COMMON_H

#include <iostream>

const uint32_t recvBuffSize = 1000;
const uint32_t maxKeyLen = 16;

struct Packet {
	uint64_t id;
	char* payload;
};

typedef uint64_t Ack;

#endif
