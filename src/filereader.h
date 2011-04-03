#include <fstream>
#include <string>
#include <iostream>
#include "common.h"

using namespace std;

class FileReader {

	public:
		// Constructor
		FileReader();
		
		// Functions
		void readFile();
		string readLine();
		string getIP(string);
		void parseKeys(string line);
		void parseLongs(uint64_t* data, int* count, string line);
		
		// Variables
		ifstream file;
		
		// Client stuff
		int clientPort;
		string clientHostname;
		uint64_t clientNonce;
		
		// KDC stuff
		int kdcPort;
		string kdcHostname;
		char** clientKeys;
		int clientKeyCount;
		char* keys;
		
		// Server stuff
		int serverPort;
		string serverHostname;
		uint64_t serverNonce;
		
		// Misc stuff
		double packetLossRate;
		double ackLossRate;
		double corruptedPacketRate;
		uint64_t* droppedPackets;
		int droppedPacketCount;
		uint64_t* droppedAcks;
		int droppedAckCount;		
		uint64_t* corruptedPackets;		
		int corruptedPacketCount;
		uint64_t windowSize;
		uint64_t packetSize;
		string algorithm;
		string fileToSend;

};
