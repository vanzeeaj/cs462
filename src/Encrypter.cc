#include "Encrypter.h"

Encrypter::Encrypter(){
	this->key = "Empty";
	this->bf = new Blowfish();
	bf->Set_Passwd(key);
}

Encrypter::Encrypter(char* newKey){
	this->key = newKey;
	this->bf = new Blowfish();
	bf->Set_Passwd(key);
}


char* Encrypter::encrypt(void* buffer, int bufferLen){
	if (bufferLen % 8 != 0) {		// aka we need to pad
		buffer = (void*) padBuffer(buffer, &bufferLen); //bufferLen gets changed here.
	}else {
		//copy to new buffer so we don't overwrite original data
		char* temp = new char[bufferLen];
		for(int i = 0; i < bufferLen; i++){
			temp[i] = ((char*)buffer)[i];
		}
		buffer = (void*)temp;
	}
	
	this->reorderBytes(buffer, bufferLen);
	bf->Encrypt(buffer, bufferLen);	
	this->reorderBytes(buffer, bufferLen);
	return (char*)buffer;
}

char* Encrypter::decrypt(void* buffer, int bufferLen){
	if (bufferLen % 8 != 0) {		// aka we need to pad
		cout << "Trying to decrypt a buffer with length not divisible by 8. Aborting."<<endl;
		return (char*)buffer;
	}else {
		//copy to new buffer so we don't overwrite original data
		char* temp = new char[bufferLen];
		for(int i = 0; i < bufferLen; i++){
			temp[i] = ((char*)buffer)[i];
		}
		buffer = (void*)temp;
	}

	this->reorderBytes(buffer, bufferLen);
	bf->Decrypt(buffer, bufferLen);
	this->reorderBytes(buffer,bufferLen);
	return (char*)buffer;
}

void Encrypter::changeKey(char* newKey){
	key = newKey;
	bf->Set_Passwd(key);
}


int Encrypter::getEncryptedLength(int length){
	if(length %8 != 0) return (length + (8 - (length % 8)));
	return length;
}


void Encrypter::printBytes(void* buffer, int bufferLen){
	cout << "[";
	for(int i = 0; i < bufferLen; i ++){
		if (i != bufferLen - 1) cout << (int)((uint8_t*)buffer)[i] << ", ";
		else cout << (int)((uint8_t*)buffer)[i] << "]" << endl;
	}
}


char* Encrypter::padBuffer(void* buffer, int* bufferLen){
	int newSize = *bufferLen + (8 - (*bufferLen % 8));
	char* result = new char[newSize];
	
	cout << "newSize = " << newSize << " bufferLen = " << *bufferLen << endl;
	for(int i = 0; i < *bufferLen; i++){
		result[i] = ((char*)buffer)[i];
		
	}
	
	for(int i = *bufferLen; i < newSize; i++){
		result[i] = '\0';
		(*bufferLen)++;
	}
	return result;
}


void Encrypter::reorderBytes(void* buffer, int bufferLen){
	#ifdef LITTLE_END
	char* chars = (char*) buffer;
	for (int i=0;i<bufferLen;i+=4){
		hton((uint32_t*)&(chars[i]));
	}
	#endif
}


void Encrypter::htonll(uint64_t* buffer){
	#ifdef LITTLE_END
	char* chars = (char*) buffer;
	swap(chars[0],chars[7]);
	swap(chars[1],chars[6]);
	swap(chars[2],chars[5]);
	swap(chars[3],chars[4]);
	#endif
	
}

void Encrypter::hton(uint32_t* buffer){
	char* chars = (char*) buffer;
	swap(chars[0],chars[3]);
	swap(chars[1],chars[2]);
}

