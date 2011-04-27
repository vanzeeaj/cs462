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
		char* getIP(string);
		void parseKeys(string line);
		void parseLongs(uint64_t* data, int* count, string line);
		
		// Variables
		ifstream file;
		
		
		// Client stuff
		client_info clientInfo;
		
		
		// KDC stuff
		KDC_info KDCInfo;

		
		// Server stuff
		server_info serverInfo;
		
		
		//Shared stuff
		shared_info sharedInfo;


};
