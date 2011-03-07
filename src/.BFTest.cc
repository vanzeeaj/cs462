

/********* BFtest.cc **********/
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "blowfish.h"

#define BUFF_SIZE 1048576  // 1MB
#define NUM_TRIALS 100

using namespace std;

int Test(Blowfish *);
// double Speed(Blowfish *);


int main()
{
	EncryptedTCPSocket* sock = new EncryptedTCPSocket("clark",45454);
	//Blowfish* bf = new Blowfish();
	char *Passwd = (char*) malloc(sizeof(char)*16);
	for (int i=0;i<16;i++) Passwd[i] = '1';
	char* payload = new char[16];
	strncpy(payload,"012345678901234",16);
	// uint32_t* payload = (uint32_t*) malloc(sizeof(char)*16);
	// payload[0] = 45;
	// payload[1] = 1235543;
	// payload[2] = 213;
	// payload[3] = 12342;
	cout << payload[0] << "." << payload[1] << "." << payload[2] << "." << payload[3] << endl;

	
	bf->Set_Passwd(Passwd);
	bf->Encrypt(payload,16);
	cout << payload[0] << "." << payload[1] << "." << payload[2] << "." << payload[3] << endl;

	bf->Decrypt(payload, 16);
	cout << payload[0] << "." << payload[1] << "." << payload[2] << "." << payload[3] << endl;
		
	return 0;
	
}


int Test(Blowfish *BF)
{
	unsigned int i;
	DWord Test_Vect;
	char *Passwd[2] = {"abcdefghijklmnopqrstuvwxyz","Who is John Galt?"};
	unsigned int Clr0[2] = {0x424c4f57,0xfedcba98};
	unsigned int Clr1[2] = {0x46495348,0x76543210};
	unsigned int Crypt0[2] = {0x324ed0fe,0xcc91732b};
	unsigned int Crypt1[2] = {0xf413a203,0x8022f684};
	
	for (i=0;i<2;i++)
	{
		Test_Vect.word0.word = Clr0[i];
		Test_Vect.word1.word = Clr1[i];
		BF->Set_Passwd(Passwd[i]);
		BF->Encrypt((void *)&Test_Vect,8);
		if (Test_Vect.word0.word != Crypt0[i] || Test_Vect.word1.word != Crypt1[i])
			return (i+1);
		BF->Decrypt((void *)&Test_Vect,8);
		if (Test_Vect.word0.word != Clr0[i] || Test_Vect.word1.word != Clr1[i])
			return -(i+1);
	}
	return 0;
}


// double Speed(Blowfish *BF)
// {
	// char *buff;
	// unsigned int i;
	// time_t begin,end;

	// buff = new char[BUFF_SIZE];
	// if (buff == NULL)
	// {
		// cerr << "\aRan out of memory for the test buffer\n";
		// return 0;
	// }
	
	// srand(0);
	// for (i=0;i<BUFF_SIZE;i++)
		// buff[i] = rand()%256;
	// BF->Set_Passwd("ianchan");

	// begin = time(NULL);
	// for (i=0;i<NUM_TRIALS;i++)
		// BF->Encrypt((void *)buff,BUFF_SIZE);
	// end = time(NULL);

	// delete []buff;
	// if (end-begin < .01)
		// return 0;
	// else
		// return double(NUM_TRIALS)/(end-begin);
// }


