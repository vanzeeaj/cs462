#ifndef COMMON_H
#define COMMON_H

#define RCVBUFSIZE 1000000;

#define KDC_PORT 5131;
#define INITIATOR_PORT 5012;
#define PEER_PORT 5013;

#define ANDY_ADDRESS "137.28.8.161"
#define CLARK_ADDRESS "137.28.8.160"
#define SHIVA_ADDRESS "137.28.8.143"

int kdc_main(int argc, char *argv[]);
void dieAndShowUsage();

#endif
