blowfish.cc                                                                                         0000700 �    Q��   ����00000045003 11472563770 015310  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              /********** blowfish.cc **********/

#include <iostream>
#include <string>
#include "blowfish.h"
#define F(x)    (((SB[0][x.byte.zero] + SB[1][x.byte.one]) ^ SB[2][x.byte.two]) + SB[3][x.byte.three])

#if defined(__SVR4) && defined(__sun)
	#define BIG_END
#else
	#define LITTLE_END
#endif

using namespace std;

int TestForBigEndian(void) { 
	short int word = 0x0001; 
	char *byte = (char *) &word; 
	return(byte[0] ? 0 : 1); 
}

uint32_t ReorderHelper(uint32_t* i){
	// moving bytes like so:
	// [0] [1] [2] [3]
	// to
	// [3] [2] [1] [0]
	unsigned char* chars = (unsigned char*) i;
	uint32_t j = 0;

	swap(chars[0],chars[3]);
	swap(chars[1],chars[2]);
	
	j = *((uint32_t*) chars);
	i = &j;

/*
	char a = i & 0xFF000000;
	char b = i & 0xFF0000;
	char c = i & 0xFF00;
	char d = i & 0xFF;
	
	uint32_t j;
	j += a;
	j += (b<<8);
	j += (c<<16);
	j += (d<<24);
*/
	chars = NULL;
	return j;
}
void ReorderBytesToLittleEndian(Word* x1, Word* x2) {
	// moving bytes like so:
	// [a] [b] [c] [d]
	// to
	// [d] [c] [b] [a]
	Word w1 = *x1;
	Word w2 = *x2;

	ReorderHelper(&(w1.word));
	ReorderHelper(&(w2.word));

	*x1 = w1;
	*x2 = w2;
}


void Blowfish::Gen_Subkeys(char *Passwd)
{
  uint32_t i,j,len=strlen(Passwd);
  Word Work,null0,null1;

  if (len > 0)
  {
	j = 0;
    for (i=0;i<NUM_SUBKEYS;i++)
    {
	Work.byte.zero = Passwd[(j++)%len];
	Work.byte.one = Passwd[(j++)%len];
	Work.byte.two = Passwd[(j++)%len];
			Work.byte.three = Passwd[(j++)%len];
			PA[i] ^= Work.word;
		}

	null0.word = null1.word = 0;
	
    for (i=0;i<NUM_SUBKEYS;i+=2)
	  {
			BF_En(&null0,&null1);
		PA[i] = null0.word;
			PA[i+1] = null1.word;
		}

    for (j=0;j<NUM_S_BOXES;j++)
			for (i=0;i<NUM_ENTRIES;i+=2)
		{
		BF_En(&null0,&null1);
		SB[j][i] = null0.word;
		SB[j][i+1] = null1.word;
		}
   }

   Work.word = null0.word = null1.word = 0;
   Passwd = NULL;
   len = 0;
}

void Blowfish::BF_En(Word *x1,Word *x2)
{
  Word w1=*x1,w2=*x2;

  w1.word ^= PA[0];
  w2.word ^= F(w1)^PA[1];       w1.word ^= F(w2)^PA[2];
  w2.word ^= F(w1)^PA[3];       w1.word ^= F(w2)^PA[4];
  w2.word ^= F(w1)^PA[5];       w1.word ^= F(w2)^PA[6];
  w2.word ^= F(w1)^PA[7];       w1.word ^= F(w2)^PA[8];
  w2.word ^= F(w1)^PA[9];       w1.word ^= F(w2)^PA[10];
  w2.word ^= F(w1)^PA[11];      w1.word ^= F(w2)^PA[12];
  w2.word ^= F(w1)^PA[13];      w1.word ^= F(w2)^PA[14];
  w2.word ^= F(w1)^PA[15];      w1.word ^= F(w2)^PA[16];
  w2.word ^= PA[17];

  *x1 = w2;
  *x2 = w1;
}

void Blowfish::BF_De(Word *x1,Word *x2)
{
  Word w1=*x1,w2=*x2;

  w1.word ^= PA[17];
  w2.word ^= F(w1)^PA[16];      w1.word ^= F(w2)^PA[15];
  w2.word ^= F(w1)^PA[14];      w1.word ^= F(w2)^PA[13];
  w2.word ^= F(w1)^PA[12];      w1.word ^= F(w2)^PA[11];
  w2.word ^= F(w1)^PA[10];      w1.word ^= F(w2)^PA[9];
  w2.word ^= F(w1)^PA[8];       w1.word ^= F(w2)^PA[7];
  w2.word ^= F(w1)^PA[6];       w1.word ^= F(w2)^PA[5];
  w2.word ^= F(w1)^PA[4];       w1.word ^= F(w2)^PA[3];
  w2.word ^= F(w1)^PA[2];       w1.word ^= F(w2)^PA[1];
  w2.word ^= PA[0];

  *x1 = w2;
  *x2 = w1;
}


Blowfish::Blowfish()
{
  Reset();
}

Blowfish::~Blowfish()
{
  Reset();
}


void Blowfish::Reset()
{
  uint32_t i,j;

  uint32_t PA_Init[NUM_SUBKEYS] =
  {
    0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344,
    0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89,
    0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c,
    0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917,
    0x9216d5d9, 0x8979fb1b
  };

  uint32_t SB_Init[NUM_S_BOXES][NUM_ENTRIES] =
  {
    0xd1310ba6, 0x98dfb5ac, 0x2ffd72db, 0xd01adfb7,
    0xb8e1afed, 0x6a267e96, 0xba7c9045, 0xf12c7f99,
    0x24a19947, 0xb3916cf7, 0x0801f2e2, 0x858efc16,
    0x636920d8, 0x71574e69, 0xa458fea3, 0xf4933d7e,
    0x0d95748f, 0x728eb658, 0x718bcd58, 0x82154aee,
    0x7b54a41d, 0xc25a59b5, 0x9c30d539, 0x2af26013,
    0xc5d1b023, 0x286085f0, 0xca417918, 0xb8db38ef,
    0x8e79dcb0, 0x603a180e, 0x6c9e0e8b, 0xb01e8a3e,
    0xd71577c1, 0xbd314b27, 0x78af2fda, 0x55605c60,
    0xe65525f3, 0xaa55ab94, 0x57489862, 0x63e81440,
    0x55ca396a, 0x2aab10b6, 0xb4cc5c34, 0x1141e8ce,
    0xa15486af, 0x7c72e993, 0xb3ee1411, 0x636fbc2a,
    0x2ba9c55d, 0x741831f6, 0xce5c3e16, 0x9b87931e,
    0xafd6ba33, 0x6c24cf5c, 0x7a325381, 0x28958677,
    0x3b8f4898, 0x6b4bb9af, 0xc4bfe81b, 0x66282193,
    0x61d809cc, 0xfb21a991, 0x487cac60, 0x5dec8032,
    0xef845d5d, 0xe98575b1, 0xdc262302, 0xeb651b88,
    0x23893e81, 0xd396acc5, 0x0f6d6ff3, 0x83f44239,
    0x2e0b4482, 0xa4842004, 0x69c8f04a, 0x9e1f9b5e,
    0x21c66842, 0xf6e96c9a, 0x670c9c61, 0xabd388f0,
    0x6a51a0d2, 0xd8542f68, 0x960fa728, 0xab5133a3,
    0x6eef0b6c, 0x137a3be4, 0xba3bf050, 0x7efb2a98,
    0xa1f1651d, 0x39af0176, 0x66ca593e, 0x82430e88,
    0x8cee8619, 0x456f9fb4, 0x7d84a5c3, 0x3b8b5ebe,
    0xe06f75d8, 0x85c12073, 0x401a449f, 0x56c16aa6,
    0x4ed3aa62, 0x363f7706, 0x1bfedf72, 0x429b023d,
    0x37d0d724, 0xd00a1248, 0xdb0fead3, 0x49f1c09b,
    0x075372c9, 0x80991b7b, 0x25d479d8, 0xf6e8def7,
    0xe3fe501a, 0xb6794c3b, 0x976ce0bd, 0x04c006ba,
    0xc1a94fb6, 0x409f60c4, 0x5e5c9ec2, 0x196a2463,
    0x68fb6faf, 0x3e6c53b5, 0x1339b2eb, 0x3b52ec6f,
    0x6dfc511f, 0x9b30952c, 0xcc814544, 0xaf5ebd09,
    0xbee3d004, 0xde334afd, 0x660f2807, 0x192e4bb3,
    0xc0cba857, 0x45c8740f, 0xd20b5f39, 0xb9d3fbdb,
    0x5579c0bd, 0x1a60320a, 0xd6a100c6, 0x402c7279,
    0x679f25fe, 0xfb1fa3cc, 0x8ea5e9f8, 0xdb3222f8,
    0x3c7516df, 0xfd616b15, 0x2f501ec8, 0xad0552ab,
    0x323db5fa, 0xfd238760, 0x53317b48, 0x3e00df82,
    0x9e5c57bb, 0xca6f8ca0, 0x1a87562e, 0xdf1769db,
    0xd542a8f6, 0x287effc3, 0xac6732c6, 0x8c4f5573,
    0x695b27b0, 0xbbca58c8, 0xe1ffa35d, 0xb8f011a0,
    0x10fa3d98, 0xfd2183b8, 0x4afcb56c, 0x2dd1d35b,
    0x9a53e479, 0xb6f84565, 0xd28e49bc, 0x4bfb9790,
    0xe1ddf2da, 0xa4cb7e33, 0x62fb1341, 0xcee4c6e8,
    0xef20cada, 0x36774c01, 0xd07e9efe, 0x2bf11fb4,
    0x95dbda4d, 0xae909198, 0xeaad8e71, 0x6b93d5a0,
    0xd08ed1d0, 0xafc725e0, 0x8e3c5b2f, 0x8e7594b7,
    0x8ff6e2fb, 0xf2122b64, 0x8888b812, 0x900df01c,
    0x4fad5ea0, 0x688fc31c, 0xd1cff191, 0xb3a8c1ad,
    0x2f2f2218, 0xbe0e1777, 0xea752dfe, 0x8b021fa1,
    0xe5a0cc0f, 0xb56f74e8, 0x18acf3d6, 0xce89e299,
    0xb4a84fe0, 0xfd13e0b7, 0x7cc43b81, 0xd2ada8d9,
    0x165fa266, 0x80957705, 0x93cc7314, 0x211a1477,
    0xe6ad2065, 0x77b5fa86, 0xc75442f5, 0xfb9d35cf,
    0xebcdaf0c, 0x7b3e89a0, 0xd6411bd3, 0xae1e7e49,
    0x00250e2d, 0x2071b35e, 0x226800bb, 0x57b8e0af,
    0x2464369b, 0xf009b91e, 0x5563911d, 0x59dfa6aa,
    0x78c14389, 0xd95a537f, 0x207d5ba2, 0x02e5b9c5,
    0x83260376, 0x6295cfa9, 0x11c81968, 0x4e734a41,
    0xb3472dca, 0x7b14a94a, 0x1b510052, 0x9a532915,
    0xd60f573f, 0xbc9bc6e4, 0x2b60a476, 0x81e67400,
    0x08ba6fb5, 0x571be91f, 0xf296ec6b, 0x2a0dd915,
    0xb6636521, 0xe7b9f9b6, 0xff34052e, 0xc5855664,
    0x53b02d5d, 0xa99f8fa1, 0x08ba4799, 0x6e85076a,
    0x4b7a70e9, 0xb5b32944, 0xdb75092e, 0xc4192623,
    0xad6ea6b0, 0x49a7df7d, 0x9cee60b8, 0x8fedb266,
    0xecaa8c71, 0x699a17ff, 0x5664526c, 0xc2b19ee1,
    0x193602a5, 0x75094c29, 0xa0591340, 0xe4183a3e,
    0x3f54989a, 0x5b429d65, 0x6b8fe4d6, 0x99f73fd6,
    0xa1d29c07, 0xefe830f5, 0x4d2d38e6, 0xf0255dc1,
    0x4cdd2086, 0x8470eb26, 0x6382e9c6, 0x021ecc5e,
    0x09686b3f, 0x3ebaefc9, 0x3c971814, 0x6b6a70a1,
    0x687f3584, 0x52a0e286, 0xb79c5305, 0xaa500737,
    0x3e07841c, 0x7fdeae5c, 0x8e7d44ec, 0x5716f2b8,
    0xb03ada37, 0xf0500c0d, 0xf01c1f04, 0x0200b3ff,
    0xae0cf51a, 0x3cb574b2, 0x25837a58, 0xdc0921bd,
    0xd19113f9, 0x7ca92ff6, 0x94324773, 0x22f54701,
    0x3ae5e581, 0x37c2dadc, 0xc8b57634, 0x9af3dda7,
    0xa9446146, 0x0fd0030e, 0xecc8c73e, 0xa4751e41,
    0xe238cd99, 0x3bea0e2f, 0x3280bba1, 0x183eb331,
    0x4e548b38, 0x4f6db908, 0x6f420d03, 0xf60a04bf,
    0x2cb81290, 0x24977c79, 0x5679b072, 0xbcaf89af,
    0xde9a771f, 0xd9930810, 0xb38bae12, 0xdccf3f2e,
    0x5512721f, 0x2e6b7124, 0x501adde6, 0x9f84cd87,
    0x7a584718, 0x7408da17, 0xbc9f9abc, 0xe94b7d8c,
    0xec7aec3a, 0xdb851dfa, 0x63094366, 0xc464c3d2,
    0xef1c1847, 0x3215d908, 0xdd433b37, 0x24c2ba16,
    0x12a14d43, 0x2a65c451, 0x50940002, 0x133ae4dd,
    0x71dff89e, 0x10314e55, 0x81ac77d6, 0x5f11199b,
    0x043556f1, 0xd7a3c76b, 0x3c11183b, 0x5924a509,
    0xf28fe6ed, 0x97f1fbfa, 0x9ebabf2c, 0x1e153c6e,
    0x86e34570, 0xeae96fb1, 0x860e5e0a, 0x5a3e2ab3,
    0x771fe71c, 0x4e3d06fa, 0x2965dcb9, 0x99e71d0f,
    0x803e89d6, 0x5266c825, 0x2e4cc978, 0x9c10b36a,
    0xc6150eba, 0x94e2ea78, 0xa5fc3c53, 0x1e0a2df4,
    0xf2f74ea7, 0x361d2b3d, 0x1939260f, 0x19c27960,
    0x5223a708, 0xf71312b6, 0xebadfe6e, 0xeac31f66,
    0xe3bc4595, 0xa67bc883, 0xb17f37d1, 0x018cff28,
    0xc332ddef, 0xbe6c5aa5, 0x65582185, 0x68ab9802,
    0xeecea50f, 0xdb2f953b, 0x2aef7dad, 0x5b6e2f84,
    0x1521b628, 0x29076170, 0xecdd4775, 0x619f1510,
    0x13cca830, 0xeb61bd96, 0x0334fe1e, 0xaa0363cf,
    0xb5735c90, 0x4c70a239, 0xd59e9e0b, 0xcbaade14,
    0xeecc86bc, 0x60622ca7, 0x9cab5cab, 0xb2f3846e,
    0x648b1eaf, 0x19bdf0ca, 0xa02369b9, 0x655abb50,
    0x40685a32, 0x3c2ab4b3, 0x319ee9d5, 0xc021b8f7,
    0x9b540b19, 0x875fa099, 0x95f7997e, 0x623d7da8,
    0xf837889a, 0x97e32d77, 0x11ed935f, 0x16681281,
    0x0e358829, 0xc7e61fd6, 0x96dedfa1, 0x7858ba99,
    0x57f584a5, 0x1b227263, 0x9b83c3ff, 0x1ac24696,
    0xcdb30aeb, 0x532e3054, 0x8fd948e4, 0x6dbc3128,
    0x58ebf2ef, 0x34c6ffea, 0xfe28ed61, 0xee7c3c73,
    0x5d4a14d9, 0xe864b7e3, 0x42105d14, 0x203e13e0,
    0x45eee2b6, 0xa3aaabea, 0xdb6c4f15, 0xfacb4fd0,
    0xc742f442, 0xef6abbb5, 0x654f3b1d, 0x41cd2105,
    0xd81e799e, 0x86854dc7, 0xe44b476a, 0x3d816250,
    0xcf62a1f2, 0x5b8d2646, 0xfc8883a0, 0xc1c7b6a3,
    0x7f1524c3, 0x69cb7492, 0x47848a0b, 0x5692b285,
    0x095bbf00, 0xad19489d, 0x1462b174, 0x23820e00,
    0x58428d2a, 0x0c55f5ea, 0x1dadf43e, 0x233f7061,
    0x3372f092, 0x8d937e41, 0xd65fecf1, 0x6c223bdb,
    0x7cde3759, 0xcbee7460, 0x4085f2a7, 0xce77326e,
    0xa6078084, 0x19f8509e, 0xe8efd855, 0x61d99735,
    0xa969a7aa, 0xc50c06c2, 0x5a04abfc, 0x800bcadc,
    0x9e447a2e, 0xc3453484, 0xfdd56705, 0x0e1e9ec9,
    0xdb73dbd3, 0x105588cd, 0x675fda79, 0xe3674340,
    0xc5c43465, 0x713e38d8, 0x3d28f89e, 0xf16dff20,
    0x153e21e7, 0x8fb03d4a, 0xe6e39f2b, 0xdb83adf7,
    0xe93d5a68, 0x948140f7, 0xf64c261c, 0x94692934,
    0x411520f7, 0x7602d4f7, 0xbcf46b2e, 0xd4a20068,
    0xd4082471, 0x3320f46a, 0x43b7d4b7, 0x500061af,
    0x1e39f62e, 0x97244546, 0x14214f74, 0xbf8b8840,
    0x4d95fc1d, 0x96b591af, 0x70f4ddd3, 0x66a02f45,
    0xbfbc09ec, 0x03bd9785, 0x7fac6dd0, 0x31cb8504,
    0x96eb27b3, 0x55fd3941, 0xda2547e6, 0xabca0a9a,
    0x28507825, 0x530429f4, 0x0a2c86da, 0xe9b66dfb,
    0x68dc1462, 0xd7486900, 0x680ec0a4, 0x27a18dee,
    0x4f3ffea2, 0xe887ad8c, 0xb58ce006, 0x7af4d6b6,
    0xaace1e7c, 0xd3375fec, 0xce78a399, 0x406b2a42,
    0x20fe9e35, 0xd9f385b9, 0xee39d7ab, 0x3b124e8b,
    0x1dc9faf7, 0x4b6d1856, 0x26a36631, 0xeae397b2,
    0x3a6efa74, 0xdd5b4332, 0x6841e7f7, 0xca7820fb,
    0xfb0af54e, 0xd8feb397, 0x454056ac, 0xba489527,
    0x55533a3a, 0x20838d87, 0xfe6ba9b7, 0xd096954b,
    0x55a867bc, 0xa1159a58, 0xcca92963, 0x99e1db33,
    0xa62a4a56, 0x3f3125f9, 0x5ef47e1c, 0x9029317c,
    0xfdf8e802, 0x04272f70, 0x80bb155c, 0x05282ce3,
    0x95c11548, 0xe4c66d22, 0x48c1133f, 0xc70f86dc,
    0x07f9c9ee, 0x41041f0f, 0x404779a4, 0x5d886e17,
    0x325f51eb, 0xd59bc0d1, 0xf2bcc18f, 0x41113564,
    0x257b7834, 0x602a9c60, 0xdff8e8a3, 0x1f636c1b,
    0x0e12b4c2, 0x02e1329e, 0xaf664fd1, 0xcad18115,
    0x6b2395e0, 0x333e92e1, 0x3b240b62, 0xeebeb922,
    0x85b2a20e, 0xe6ba0d99, 0xde720c8c, 0x2da2f728,
    0xd0127845, 0x95b794fd, 0x647d0862, 0xe7ccf5f0,
    0x5449a36f, 0x877d48fa, 0xc39dfd27, 0xf33e8d1e,
    0x0a476341, 0x992eff74, 0x3a6f6eab, 0xf4f8fd37,
    0xa812dc60, 0xa1ebddf8, 0x991be14c, 0xdb6e6b0d,
    0xc67b5510, 0x6d672c37, 0x2765d43b, 0xdcd0e804,
    0xf1290dc7, 0xcc00ffa3, 0xb5390f92, 0x690fed0b,
    0x667b9ffb, 0xcedb7d9c, 0xa091cf0b, 0xd9155ea3,
    0xbb132f88, 0x515bad24, 0x7b9479bf, 0x763bd6eb,
    0x37392eb3, 0xcc115979, 0x8026e297, 0xf42e312d,
    0x6842ada7, 0xc66a2b3b, 0x12754ccc, 0x782ef11c,
    0x6a124237, 0xb79251e7, 0x06a1bbe6, 0x4bfb6350,
    0x1a6b1018, 0x11caedfa, 0x3d25bdd8, 0xe2e1c3c9,
    0x44421659, 0x0a121386, 0xd90cec6e, 0xd5abea2a,
    0x64af674e, 0xda86a85f, 0xbebfe988, 0x64e4c3fe,
    0x9dbc8057, 0xf0f7c086, 0x60787bf8, 0x6003604d,
    0xd1fd8346, 0xf6381fb0, 0x7745ae04, 0xd736fccc,
    0x83426b33, 0xf01eab71, 0xb0804187, 0x3c005e5f,
    0x77a057be, 0xbde8ae24, 0x55464299, 0xbf582e61,
    0x4e58f48f, 0xf2ddfda2, 0xf474ef38, 0x8789bdc2,
    0x5366f9c3, 0xc8b38e74, 0xb475f255, 0x46fcd9b9,
    0x7aeb2661, 0x8b1ddf84, 0x846a0e79, 0x915f95e2,
    0x466e598e, 0x20b45770, 0x8cd55591, 0xc902de4c,
    0xb90bace1, 0xbb8205d0, 0x11a86248, 0x7574a99e,
    0xb77f19b6, 0xe0a9dc09, 0x662d09a1, 0xc4324633,
    0xe85a1f02, 0x09f0be8c, 0x4a99a025, 0x1d6efe10,
    0x1ab93d1d, 0x0ba5a4df, 0xa186f20f, 0x2868f169,
    0xdcb7da83, 0x573906fe, 0xa1e2ce9b, 0x4fcd7f52,
    0x50115e01, 0xa70683fa, 0xa002b5c4, 0x0de6d027,
    0x9af88c27, 0x773f8641, 0xc3604c06, 0x61a806b5,
    0xf0177a28, 0xc0f586e0, 0x006058aa, 0x30dc7d62,
    0x11e69ed7, 0x2338ea63, 0x53c2dd94, 0xc2c21634,
    0xbbcbee56, 0x90bcb6de, 0xebfc7da1, 0xce591d76,
    0x6f05e409, 0x4b7c0188, 0x39720a3d, 0x7c927c24,
    0x86e3725f, 0x724d9db9, 0x1ac15bb4, 0xd39eb8fc,
    0xed545578, 0x08fca5b5, 0xd83d7cd3, 0x4dad0fc4,
    0x1e50ef5e, 0xb161e6f8, 0xa28514d9, 0x6c51133c,
    0x6fd5c7e7, 0x56e14ec4, 0x362abfce, 0xddc6c837,
    0xd79a3234, 0x92638212, 0x670efa8e, 0x406000e0,
    0x3a39ce37, 0xd3faf5cf, 0xabc27737, 0x5ac52d1b,
    0x5cb0679e, 0x4fa33742, 0xd3822740, 0x99bc9bbe,
    0xd5118e9d, 0xbf0f7315, 0xd62d1c7e, 0xc700c47b,
    0xb78c1b6b, 0x21a19045, 0xb26eb1be, 0x6a366eb4,
    0x5748ab2f, 0xbc946e79, 0xc6a376d2, 0x6549c2c8,
    0x530ff8ee, 0x468dde7d, 0xd5730a1d, 0x4cd04dc6,
    0x2939bbdb, 0xa9ba4650, 0xac9526e8, 0xbe5ee304,
    0xa1fad5f0, 0x6a2d519a, 0x63ef8ce2, 0x9a86ee22,
    0xc089c2b8, 0x43242ef6, 0xa51e03aa, 0x9cf2d0a4,
    0x83c061ba, 0x9be96a4d, 0x8fe51550, 0xba645bd6,
    0x2826a2f9, 0xa73a3ae1, 0x4ba99586, 0xef5562e9,
    0xc72fefd3, 0xf752f7da, 0x3f046f69, 0x77fa0a59,
    0x80e4a915, 0x87b08601, 0x9b09e6ad, 0x3b3ee593,
    0xe990fd5a, 0x9e34d797, 0x2cf0b7d9, 0x022b8b51,
    0x96d5ac3a, 0x017da67d, 0xd1cf3ed6, 0x7c7d2d28,
    0x1f9f25cf, 0xadf2b89b, 0x5ad6b472, 0x5a88f54c,
    0xe029ac71, 0xe019a5e6, 0x47b0acfd, 0xed93fa9b,
    0xe8d3c48d, 0x283b57cc, 0xf8d56629, 0x79132e28,
    0x785f0191, 0xed756055, 0xf7960e44, 0xe3d35e8c,
    0x15056dd4, 0x88f46dba, 0x03a16125, 0x0564f0bd,
    0xc3eb9e15, 0x3c9057a2, 0x97271aec, 0xa93a072a,
    0x1b3f6d9b, 0x1e6321f5, 0xf59c66fb, 0x26dcf319,
    0x7533d928, 0xb155fdf5, 0x03563482, 0x8aba3cbb,
    0x28517711, 0xc20ad9f8, 0xabcc5167, 0xccad925f,
    0x4de81751, 0x3830dc8e, 0x379d5862, 0x9320f991,
    0xea7a90c2, 0xfb3e7bce, 0x5121ce64, 0x774fbe32,
    0xa8b6e37e, 0xc3293d46, 0x48de5369, 0x6413e680,
    0xa2ae0810, 0xdd6db224, 0x69852dfd, 0x09072166,
    0xb39a460a, 0x6445c0dd, 0x586cdecf, 0x1c20c8ae,
    0x5bbef7dd, 0x1b588d40, 0xccd2017f, 0x6bb4e3bb,
    0xdda26a7e, 0x3a59ff45, 0x3e350a44, 0xbcb4cdd5,
    0x72eacea8, 0xfa6484bb, 0x8d6612ae, 0xbf3c6f47,
    0xd29be463, 0x542f5d9e, 0xaec2771b, 0xf64e6370,
    0x740e0d8d, 0xe75b1357, 0xf8721671, 0xaf537d5d,
    0x4040cb08, 0x4eb4e2cc, 0x34d2466a, 0x0115af84,

    0xe1b00428, 0x95983a1d, 0x06b89fb4, 0xce6ea048,
    0x6f3f3b82, 0x3520ab82, 0x011a1d4b, 0x277227f8,
    0x611560b1, 0xe7933fdc, 0xbb3a792b, 0x344525bd,
    0xa08839e1, 0x51ce794b, 0x2f32c9b7, 0xa01fbac9,
    0xe01cc87e, 0xbcc7d1f6, 0xcf0111c3, 0xa1e8aac7,
    0x1a908749, 0xd44fbd9a, 0xd0dadecb, 0xd50ada38,
    0x0339c32a, 0xc6913667, 0x8df9317c, 0xe0b12b4f,
    0xf79e59b7, 0x43f5bb3a, 0xf2d519ff, 0x27d9459c,
    0xbf97222c, 0x15e6fc2a, 0x0f91fc71, 0x9b941525,
    0xfae59361, 0xceb69ceb, 0xc2a86459, 0x12baa8d1,
    0xb6c1075e, 0xe3056a0c, 0x10d25065, 0xcb03a442,
    0xe0ec6e0e, 0x1698db3b, 0x4c98a0be, 0x3278e964,
    0x9f1f9532, 0xe0d392df, 0xd3a0342b, 0x8971f21e,
    0x1b0a7441, 0x4ba3348c, 0xc5be7120, 0xc37632d8,
    0xdf359f8d, 0x9b992f2e, 0xe60b6f47, 0x0fe3f11d,
    0xe54cda54, 0x1edad891, 0xce6279cf, 0xcd3e7e6f,
    0x1618b166, 0xfd2c1d05, 0x848fd2c5, 0xf6fb2299,
    0xf523f357, 0xa6327623, 0x93a83531, 0x56cccd02,
    0xacf08162, 0x5a75ebb5, 0x6e163697, 0x88d273cc,
    0xde966292, 0x81b949d0, 0x4c50901b, 0x71c65614,
    0xe6c6c7bd, 0x327a140a, 0x45e1d006, 0xc3f27b9a,
    0xc9aa53fd, 0x62a80f00, 0xbb25bfe2, 0x35bdd2f6,
    0x71126905, 0xb2040222, 0xb6cbcf7c, 0xcd769c2b,
    0x53113ec0, 0x1640e3d3, 0x38abbd60, 0x2547adf0,
    0xba38209c, 0xf746ce76, 0x77afa1c5, 0x20756060,
    0x85cbfe4e, 0x8ae88dd8, 0x7aaaf9b0, 0x4cf9aa7e,
    0x1948c25c, 0x02fb8a8c, 0x01c36ae4, 0xd6ebe1f9,
    0x90d4f869, 0xa65cdea0, 0x3f09252d, 0xc208e69f,
    0xb74e6132, 0xce77e25b, 0x578fdfe3, 0x3ac372e6
  };

  for (i=0;i<NUM_SUBKEYS;i++)
    PA[i] = PA_Init[i];

  for (j=0;j<NUM_S_BOXES;j++)
    for (i=0;i<NUM_ENTRIES;i++)
      SB[j][i] = SB_Init[j][i];
}

void Blowfish::Set_Passwd(char *Passwd)
{
  char New_Passwd[MAX_STRING];
  uint32_t i,len;

	if (Passwd == NULL)
	{
	do
	{
	cout << "\aEnter your password: ";
	cin.get(New_Passwd,MAX_STRING,'\n');
	len = strlen(New_Passwd);
	}
	while (len > MAX_PASSWD);
	Passwd = New_Passwd;
  }
  else
	len = strlen(Passwd);

  Reset();
  if (len > 0)
	Gen_Subkeys(Passwd);

  for (i=0;i<MAX_STRING;i++)
    New_Passwd[i] = '\0';
   Passwd = NULL;
   len = 0;
}

void Blowfish::Encrypt(void *Ptr,uint32_t N_Bytes)
{
  uint32_t i;
  DWord *Work;

  if (N_Bytes%8)
  {
    cerr << "\aBlowfish requires the input to be a multiple of 8 bytes (64 bits) to work.\n";
    return;
  }

	N_Bytes /= 8;
  	Work = (DWord *)Ptr;


  for (i=0;i<N_Bytes;i++)
  {
	/*
	// Before encrypt, need to reorder bytes for uniform endianness
	if (TestForBigEndian()) {
		ReorderBytesToLittleEndian(&Work->word0, &Work->word1);
	}
	*/
    BF_En(&Work->word0,&Work->word1);
    Work++;
  }

  Work = NULL;
}

void Blowfish::Decrypt(void *Ptr,uint32_t N_Bytes)
{
  uint32_t i;
  DWord *Work;

  if (N_Bytes%8)
  {
    cerr << "\aBlowfish requires the input to be a multiple of 8 bytes (64 bits) to work.\n";
    return;
  }

	N_Bytes /= 8;
 	Work = (DWord *)Ptr;

  for (i=0;i<N_Bytes;i++)
  {
    BF_De(&Work->word0,&Work->word1);
	// After decrypt, need to reorder bytes for uniform endianness
	if (TestForBigEndian()) {
		ReorderBytesToLittleEndian(&Work->word0, &Work->word1);
	}
    Work++;
  }

  Work = NULL;
}


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             blowfish.h                                                                                          0000700 �    Q��   ����00000002151 11472046214 015134  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              /********** blowfish.h **********/

#ifndef ___BLOWFISH_H___
#define ___BLOWFISH_H___


#define NUM_SUBKEYS  18
#define NUM_S_BOXES  4
#define NUM_ENTRIES  256

#define MAX_STRING   256
#define MAX_PASSWD   56  // 448bits

#if defined(__SVR4) && defined(__sun)
 #define BIG_END
#else
 #define LITTLE_END
#endif

#ifdef BIG_END
struct WordByte
{
	unsigned int zero:8;
	unsigned int one:8;
	unsigned int two:8;
	unsigned int three:8;
};
#endif

#ifdef LITTLE_END
struct WordByte
{
	unsigned int three:8;
	unsigned int two:8;
	unsigned int one:8;
	unsigned int zero:8;
};
#endif
union Word
{
	unsigned int word;
	WordByte byte;
};

struct DWord
{
	Word word0;
	Word word1;
};


class Blowfish
{
private:
  unsigned int PA[NUM_SUBKEYS];
  unsigned int SB[NUM_S_BOXES][NUM_ENTRIES];

  void Gen_Subkeys(char *);
  inline void BF_En(Word *,Word *);
  inline void BF_De(Word *,Word *);

public:
  Blowfish();
  ~Blowfish();

  void Reset();
  void Set_Passwd(char * = NULL);
  void Encrypt(void *,unsigned int);
  void Decrypt(void *,unsigned int);
};


#endif

                                                                                                                                                                                                                                                                                                                                                                                                                       client.cc                                                                                           0000700 �    Q��   ����00000042006 11472626664 014753  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #include "client.h"
#include "common.h"
#include "PracticalSocket.h"

// --------------------------------------------
// Constructors/destructors
// --------------------------------------------

Client::Client(uint64_t n, char* k, bool initiator, int size) {
	this->init(n, k, initiator, size);
}

Client::Client(){
	this->init(12345,"this is a key", false, 100);
}

Client::~Client() {
	free(this->marked);
}

void Client::init(uint64_t n, char* k, bool i, int s) {
	if (s%16384 != 0) {
		this->arraySize = (s/16384)*16384 + 16384;	// truncate and add 16384 
	} else {
		this->arraySize = s;
	}
	this->userInputSize = s;
	this->foundPrimes = new vector<long>();
	this->marked = (char*) calloc(arraySize, sizeof (char));
	this->key = k;
	this->nonce = n;
	this->request = "request for ks";
	this->initiator = i;
	this->prime = 2;
	this->done = false;
	this->listenPort = 36537;
	this->sendPort = listenPort+1;
	this->recvBuffSize = RCVBUFSIZE;
	this->cipherToSendToPeer1= NULL;
	this->cipherToSendToPeer2= NULL;
	this->cipherToReceiveFromPeer1= NULL;
	this->cipherToReceiveFromPeer2= NULL;
	this->bf = new Blowfish();
	if (i) {
		initiate();	// starts the whole thing if we're the initiator
	} else {
		listenForCommunication();	// otherwise, just sits and waits
	}
}

// --------------------------------------------
// Initiator (A): Begin
// --------------------------------------------

void Client::initiate(){
	if (userInputSize < 2) {
		cout << "invalid size entered... exiting..." << endl;
		exit(1);
	}
	//this->marked = (uint64_t*) calloc(arraySize, sizeof (uint64_t));
	if (marked == NULL) {
		cerr << "Not enough memory space for array size specified... exiting..." << endl;
		exit(1);
	}
	marked[0] = 1;	// marking our 2 invalid numbers
	marked[1] = 1;
	
	// Start convo with the KDC
	getSessionKeyFromKDC();
	// Auth with peer
	getAuthenticationFromPeer();

	// If we've made it this far, we're authenticated.
	// start the sieve
	sieve();
}

// --------------------------------------------
// Initiator (A): Communicate with KDC
// --------------------------------------------

void Client::getSessionKeyFromKDC() {
	TCPSocket* kdcSocket = getConnectionWithKDC();
	sendInfoToKDC(kdcSocket);
	getInfoFromKDC(kdcSocket);
	delete (kdcSocket);
}

TCPSocket* Client::getConnectionWithKDC() {
	try {
		// Establish connection with the echo server
		string servAddress = SHIVA_ADDRESS;
		unsigned short echoServPort = KDC_PORT;
		TCPSocket* clientSocket = new TCPSocket(servAddress, echoServPort);
		return clientSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}
	
void Client::sendInfoToKDC(TCPSocket* sock) {
	
	//cout << "Sending request to KDC" << endl;

	// Our 2 buffers we want to send and their lengths
	unsigned int firstTransLen = strlen(request);
	char* firstTrans = new char [firstTransLen];
	memcpy(firstTrans, request, firstTransLen);
	unsigned int secondTransLen = 8;
	// char* secondTrans = new char [secondTransLen];
	// memcpy(secondTrans, nonce, secondTransLen);

	try {
		// Send the length to the socket, and then the buffer, for both items
		cout << "sending " << firstTransLen << " bytes:" << firstTrans << endl;
		sock->send(&firstTransLen, 4);
		sock->send(firstTrans, firstTransLen);
		cout << "sending " << secondTransLen << " bytes:" << nonce << endl;
		sock->send(&secondTransLen, 4);
		sock->send(&nonce, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to KDC: (Step 1)" << endl;
	cout << "  Req = '" << firstTrans << "'" << endl;
	cout << "  N1 = '" << nonce << "'" << endl;
	//cout << "sent stuff" << endl;
	delete (firstTrans);
	//delete (secondTrans);
	
}

void Client::getInfoFromKDC(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int i=0;
	int transCount = 5;
	char** recvBuff = new char*[transCount];
	uint64_t recvNonce;
	
	try {
		// Loops until we receive 5 different buffers
		while (i<transCount){
			unsigned int s;					// size of the incoming buffer
			if (i==2){
				recvBuff[i] = new char [4];
				sock->recv(&s, 4);
				sock->recv(&recvNonce, s);
			} else {
				sock->recv(&s, 4);
				recvBuff[i] = new char [s];		// create the buffer with that size
				sock->recv(recvBuff[i],s);		// receive the buffer
				cout << "received:" << recvBuff[i] << ", size:" << s << ",iteration " << i << endl;
			}
			i++;
		}
		cout << "Received from KDC: (Step 2)" << endl;
		cout << "  recvBuff[0] = '" << recvBuff[0] << "'" << endl;
		cout << "  recvBuff[1] = '" << recvBuff[1] << "'" << endl;
		cout << "  recvBuff[2] = '" << recvNonce << "'" << endl;
		cout << "  recvBuff[3] = '" << recvBuff[3] << "'" << endl;
		cout << "  recvBuff[4] = '" << recvBuff[4] << "'" << endl;
		cout << "  Eka[Ks] = '" << recvBuff[0] << "'" << endl;
		// TODO DECRYPT RECVBUFF[0-4] WITH Ka
		sessionKey = recvBuff[0];
		if (strcmp(recvBuff[1], request)) {
			cerr << "Did not receive our request back.  Terminating" << endl;
			exit(1);
		}
		if (recvNonce != nonce) {
			cerr << "Did not receive our nonce back.  Terminating" << endl;
			exit(1);	
		}
		cipherToSendToPeer1 = recvBuff[3];
		cipherToSendToPeer2 = recvBuff[4];
		
		// change this after decryption done:
		cout << "  Ks(decrypted) = '" << recvBuff[0] << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

// --------------------------------------------
// Initiator (A): Authenticate with Peer (B)
// --------------------------------------------

void Client::getAuthenticationFromPeer() {
	TCPSocket* peerSocket = getConnectionWithPeer();
	sendCyphers(peerSocket);
	receiveNonce2(peerSocket);
	sendMutatedNonce2(peerSocket);
	receiveOkay(peerSocket);
	cout << "Finished receiving authentication from peer" << endl;
	activeSocket = peerSocket;
}

TCPSocket* Client::getConnectionWithPeer() {
	try {
		string servAddress = CLARK_ADDRESS; // peer (B) is always clark
		unsigned short echoServPort = PEER_PORT;
		TCPSocket* peerSocket = new TCPSocket(servAddress, echoServPort);
		return peerSocket;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	return NULL;
}

void Client::sendCyphers(TCPSocket* sock) {
	
	//cout << "Sending cyphers to peer" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "  cipherToSendToPeer1='" << cipherToSendToPeer1 << "'" << endl;
	unsigned int firstTransLen = strlen(cipherToSendToPeer1);
	char* firstTrans = new char [firstTransLen];
	memcpy(firstTrans, cipherToSendToPeer1, firstTransLen);
	cout << "  cipherToSendToPeer2='" << cipherToSendToPeer2 << "'" << endl;
	unsigned int secondTransLen = strlen(cipherToSendToPeer2);
	char* secondTrans = new char [secondTransLen];
	memcpy(secondTrans, cipherToSendToPeer2, secondTransLen);


	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(firstTrans, firstTransLen);
		sock->send(&secondTransLen, 4);
		sock->send(secondTrans, secondTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "Sent to B: (Step 3)" << endl;
	cout << "  first cypher  = '" << firstTrans << "'" << endl;
	cout << "  second cypher = '" << secondTrans << "'" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	//cout << "sent stuff" << endl;
	delete (firstTrans);
	delete (secondTrans);
	
}

void Client::receiveNonce2(TCPSocket* sock) {
	
	//cout << "Receiving key from KDC..." << endl;
	
	// Prepare to receive the 5 items sent back from authentication with KDC
	int i=0;
	int transCount = 1;
	char** recvBuff = new char*[transCount];
	
	try {
		// Loops until we receive 5 different buffers
		// while (i<transCount){
			// unsigned int s;					// size of the incoming buffer
			// sock->recv(&s, 4);
			// recvBuff[i] = new char [s];		// create the buffer with that size
			// sock->recv(recvBuff[i],s);		// receive the buffer
			// //cout << "received:" << recvBuff[i] << ", size:" << s << ",iteration " << i << endl;
			// i++;
		// }
		uint64_t recvNonce;
		uint32_t nonceSize;
		sock->recv(&nonceSize,4);
		sock->recv(&recvNonce,nonceSize);
		cout << "Received from B: (Step 4)" << endl;
		cout << "  Eks[N2] = '" << recvNonce << "'" << endl;
		// TODO DECRYPT RECVBUFF[0] WITH Ks
		// sessionKey = recvNonce;
		// if (strcmp(recvBuff[1], request)) {
		// 	cerr << "Did not receive our request back.  Terminating" << endl;
		// 	exit(1);
		// }
		// if (strcmp(recvBuff[2], nonce)) {
		// 	cerr << "Did not receive our nonce back.  Terminating" << endl;
		// 	exit(1);	
		// }
		nonce2ToReceiveFromPeer = recvNonce;
		// change this after decryption done:
		cout << "  N2(decrypted) = '" << recvNonce << "'" << endl;
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	delete [] recvBuff;
}

void Client::sendMutatedNonce2(TCPSocket* sock) {
	
	
	mutatedNonceToSend = hashF(nonce2ToReceiveFromPeer);
	
	//cipherToSendToPeer1 = recvBuff[3];
	
	
	//cout << "Sending cyphers to peer" << endl;

	// Our 2 buffers we want to send and their lengths
	cout << "Sent to B: (Step 5)" << endl;
	cout << "  mutatedNonceToSend='" << mutatedNonceToSend << "'" << endl;
	
	// TODO: Encrypt this with Eks
	
	
	cout << "  mutatedNonce(encrypted)='" << mutatedNonceToSend << "'" << endl;
	
	unsigned int firstTransLen = 8;
	//char* firstTrans = new char [firstTransLen];
	//memcpy(firstTrans, mutatedNonceToSend, firstTransLen);

	try {
		// Send the length to the socket, and then the buffer, for both items
		sock->send(&firstTransLen, 4);
		sock->send(&mutatedNonceToSend, firstTransLen);
	
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	cout << "  Eks[f(N2)]" << endl;
	//cout << "sent stuff" << endl;
	//delete (firstTrans);
	
}

void Client::receiveOkay(TCPSocket* sock) {
	try {
		unsigned int s;					// size of the incoming buffer
		sock->recv(&s, 4);
		
		if (s!=1) {
			cout << "Denied \"okay\" message by B! (Step 6)" << endl;
			exit(1);
		}
		cout << "Received \"okay\" message from B (Step 6)" << endl;
		
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}


// --------------------------------------------
// Peer (B): Begin
// --------------------------------------------

void Client::listenForCommunication() {
	//cout << "Client initiated " << endl;
	unsigned short myPort = PEER_PORT;
	try {
		peerServerSocket = new TCPServerSocket(myPort);
		
		// Accept incoming connections (blocking)
		initiatorClientSocket = peerServerSocket->accept();
		
		// Connection initiated, time to do things.
		cout << "Client connected..." << endl;
		
		receiveCyphers(initiatorClientSocket);
		sendNonce2(initiatorClientSocket);
		receiveMutatedNonce2(initiatorClientSocket);
		sendOkay(initiatorClientSocket);
		
		cout << "Finished giving authentication to peer" << endl;
		delete(peerServerSocket);
	
		activeSocket = initiatorClientSocket;
		sieve();
	
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}

void Client::receiveCyphers(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	cipherToReceiveFromPeer1 = new char [f];
	sock->recv(cipherToReceiveFromPeer1, f);
	cout << "cipherToReceiveFromPeer1='" << cipherToReceiveFromPeer1 << "'" << endl;
	// memcpy(cipherToReceiveFromPeer1,recvBuf1,f);
	
	//cout << "grabbed cypher 1:" << cipherToReceiveFromPeer1 << endl;
	
	// Same as above, except for second transmission
	unsigned int s;
	sock->recv(&s, 4);
	//cout << "received len:" << s << endl;
	cipherToReceiveFromPeer2 = new char [s];
	sock->recv(cipherToReceiveFromPeer2, s);
	cout << "cipherToReceiveFromPeer2='" << cipherToReceiveFromPeer2 << "'" << endl;
	//memcpy(cipherToReceiveFromPeer2, recvBuf2, s);
	//cout << "grabbed cypher 2:" << cipherToReceiveFromPeer2 << endl;
	
	cout << "Received from A: (Step 3)" << endl;
	cout << "  Ekb[Ks, Ida]" << endl;
	
	// TODO: Decrypt
	
	// Change this next line after decryption is done:
	cout << "  Ks(decrypted) = '" << cipherToReceiveFromPeer1 << "'" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	
	sessionKey = cipherToReceiveFromPeer1;
	
	// delete (recvBuf1);
	// delete (recvBuf2);
	
	//cout << "finished recv" << endl;

}

void Client::sendNonce2(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t nonceSize = 8;
	uint64_t cipherNonce = nonce;
	uint32_t cipher1Size = 8;

	//char* cipherToPeer1 = new char [cipher1Size];
	//cipherToPeer1 = nonce + '\0';

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// SEND 1
		//shivaSize = reorderBytesToLittleEndian(&cipher1Size);
		// TODO ENCRYPT WITH Ks
		sock->send(&nonceSize, 4);
		sock->send(&cipherNonce, cipher1Size);
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 4)" << endl;
	cout << "  N2 = '" << nonce << "'" << endl;
	cout << "  Eks[N2] = '" << cipherNonce << endl;
}

void Client::receiveMutatedNonce2(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	//cout << "received len:" << f << endl;
	//mutatedNonceToReceive = new char [f];
	uint64_t mutatedNonceToReceive;
	sock->recv(&mutatedNonceToReceive, f);
	cout << "mutatedNonceToReceive='" << mutatedNonceToReceive << "'" << endl;
	// memcpy(mutatedNonceToReceive,recvBuf1,f);
	
	//cout << "grabbed cypher 1:" << mutatedNonceToReceive << endl;
	
	cout << "Received from A: (Step 5)" << endl;
	cout << "  Eks[f(N2)]" << endl;
	
	// TODO: Decrypt
	
	// Change this next line after decryption is done:
	cout << "  f(N2) (decrypted) = '" << mutatedNonceToReceive << "'" << endl;
	
	// delete (recvBuf1);
	// delete (recvBuf2);
	
	//cout << "finished recv" << endl;
	uint64_t tempnonce = hashF(nonce);
	
	if (mutatedNonceToReceive != tempnonce) {
		cout << "Received f(N2) != local f(N2).  Exiting..." << endl;
		exit(1);
	}

}

void Client::sendOkay(TCPSocket* sock) {
	cout << "Sending \"okay\" message to A (Step 6)" << endl;
	int k = 1;
	sock->send(&k, 4);
}

uint64_t Client::hashF(uint64_t nonce) {
	const long A = 48271; 
	const long M = 2147483647; 
	const long Q = M/A; 
	const long R = M%A; 

	static long state = 1; 
	long t = A * (state % Q) - R * (state / Q); 

	//cout << t << "=t" << endl;

	if (t > 0) 
		state = t; 
	else 
		state = t + M; 

	uint64_t retl = (uint64_t)(((double) state/M) * nonce);
	return retl;

}



// --------------------------------------------
// Perform the Sieve 
// --------------------------------------------


void Client::sieve() {
	cout << "Starting the sieve..." << endl;

	// Set our BF key
	bf->Set_Passwd(sessionKey);
	
	if (!initiator) {
		while (!done) {
			recvPrimes();	
			calcPrimes();
			sendPrimes();
		}
	} else {
		while (!done) {
			calcPrimes();
			if (!done) {
				sendPrimes();
				if (!done)	recvPrimes();
			}
		}
	} 
	
	if (initiator) printPrimes();
	if (activeSocket!=NULL) delete activeSocket;

}

void Client::calcPrimes() {
	prime = fetchNextPrime(prime);	// sets prime to the next prime value
	// CHECK TO SEE IF WE'VE BROKEN THE NECESSARY BOUNDS
	if (prime*prime>arraySize) {
		done = true;
	//	cout << "done" << endl;
	} else {
		if (prime < 20 ) cout << "grabbed prime " << prime << endl;
		// WE HAVEN'T BROKEN THE BOUNDS, SAVE THE PRIME AND PERFORM THE SIEVE
		if (initiator)	{	// only the initiator keeps track
			foundPrimes->push_back(prime);
		}
		for (long l=prime;l<arraySize;l+=prime) {
			if (!marked[l]) marked[l] = 1;
		}
		long otherClientsPrime = fetchNextPrime(prime);
		//if (prime < 10) cout << "next prime is " << otherClientsPrime << endl;
		if (otherClientsPrime*otherClientsPrime>arraySize) {
			done = true;
	//		cout << "done" << endl;
		} else {
			if (initiator) {
				foundPrimes->push_back(otherClientsPrime);	// add it to our prime list
			}
		}
	}
}

long Client::fetchNextPrime(long currPrime) {
	bool foundPrime = false;
	long nextPrime = currPrime;
	while (!foundPrime && nextPrime < arraySize) {
		if (!marked[nextPrime]) {
			foundPrime = true;
		} else {
			nextPrime++;
		}
	}
	return nextPrime;
}
void Client::sendPrimes() {
	int chunkSize = 16384;
	char* tempchars;
	int size;
	bf->Encrypt(marked, arraySize);
	for (long i=0;i<arraySize;i+=chunkSize) {
		tempchars = new char [chunkSize];
		memcpy(tempchars, &marked[i], chunkSize);
		activeSocket->send(tempchars, chunkSize);
		delete tempchars;
	}
	
}

void Client::printPrimes() {
	long count = (long)foundPrimes->size();
	for (long l=0;l<userInputSize;l++) {
		if (!marked[l]){
			count++;
//			cout << l << endl;
		}
	}
	cout << "There are " << count << " primes between 2 and " << userInputSize << "." << endl;
}

void Client::recvPrimes() {
	int chunkSize = 16384;
	char* tempchars;
	int size;
	
	for (long i=0;i<arraySize;i+=chunkSize) {
		tempchars = new char [chunkSize];
		activeSocket->recv(tempchars, chunkSize);
		memcpy(&marked[i], tempchars, chunkSize);
		delete tempchars;
	}
	bf->Decrypt(marked, arraySize);
}



                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          client.h                                                                                            0000700 �    Q��   ����00000005676 11472552374 014625  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <vector>
#include "blowfish.h"
#include "PracticalSocket.h"

#define BACKLOG 100


using namespace std;

class Client {
	public:
		Client();
		Client(uint64_t, char*, bool, int);
		~Client();
		
		//void greg_execute();
		
	private:
		// Sieve and constructor stuff
		void init(uint64_t, char*, bool, int);	// used in constructor chaining
		void initiate();				// called when an initiator is created
		void sieve();					// main sieve function, calls functions below
		void calcPrimes();				// calculates the primes
		void sendPrimes();				// sends our filteredList to other client
		void recvPrimes();				// blocks until ready for receive
		void printPrimes();				// prints the primes to stdout
		long fetchNextPrime(long);			// sets local prime to next prime in array
		//uint64_t* marked;
		char* marked;					// marked array
		vector<long>* foundPrimes;		// primes we've used to mark
		uint32_t userInputSize;			// the highest number we output through
		uint32_t arraySize;				// the size of our array   (this must be %8)
		long prime;						// our current prime
		bool done;						// bool to determine when we should exit(0)
		bool initiator;					// is this the client that is starting the sieve?

		// Auth, Crypt and Socket stuff
		uint64_t hashF(uint64_t);					// function given to us by dr. tan
		void listenForCommunication();
		TCPSocket* getConnectionWithKDC();
		void sendInfoToKDC(TCPSocket*);
		void getInfoFromKDC(TCPSocket*);
		void getSessionKeyFromKDC();	// generates our session key
		void getAuthenticationFromPeer();
		
		TCPSocket* getConnectionWithPeer();
		void sendCyphers(TCPSocket*);
		void receiveNonce2(TCPSocket*);
		void sendMutatedNonce2(TCPSocket*);
		void receiveOkay(TCPSocket*);
		
		void receiveCyphers(TCPSocket*);
		void sendNonce2(TCPSocket*);
		void receiveMutatedNonce2(TCPSocket*);
		void sendOkay(TCPSocket*);
		
		TCPServerSocket* peerServerSocket;
		TCPSocket* initiatorClientSocket;
		TCPSocket* activeSocket;
		
		uint64_t nonce;					// our nonce (used in authenticating)
		char* key;						// our private key (used for authenticating)
		char* sessionKey;				// our session key (used for encrypting/decrypting lists)
		char* request;
		char* cipherToSendToPeer1;
		char* cipherToSendToPeer2;
		uint64_t mutatedNonceToSend;
		
		char* cipherToReceiveFromPeer1;
		char* cipherToReceiveFromPeer2;
		uint64_t nonce2ToReceiveFromPeer;
		uint64_t mutatedNonceToReceive;
		
		int localId;					// our ID (pid + host ip)
		int listenPort;					// the port we listen on
		int sendPort;					// the port we send to
		int connectedClientId;			// client's ID that we send our list to
		unsigned int recvBuffSize;
		Blowfish* bf;
		
};

#endif
                                                                  client_main.cc                                                                                      0000700 �    Q��   ����00000001603 11472376424 015751  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #include <iostream>
#include <stdlib.h>
#include "client.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 4) {
		cout << "usage: ./client \"size\" \"nonce\" \"key\" \"initiator\"" << endl;
	} else {
		long i = atoi(argv[1]);
		//char* jj = argv[2];
		uint64_t jj = (uint64_t)atoll(argv[2]);
		
		
		//char* k = argv[3];
		char* k = new char[16];
		int j;
		for (j = 0; j < 16; j++) {
			k[j] = '0';
		}
		for (j = 0; j < 16; j++) {
			if (argv[3][j] == '\0') { break; }
			else { k[j] = argv[3][j]; }
		}
		
		int l = atoi(argv[4]);
		bool b = (l ? true : false);
		cout << "Client ";
		if (b) cout << "(A) (initiator) ";
		else cout << "(B) (non-initiator) ";
		cout << "started with info(nonce:" << jj << ", key:" << k << ", i:" << b << ", size:" << i << ")" << endl;
		Client* c = new Client(jj, k, b, i);
		//c->listenForTraffic();
		//c->greg_execute();
	}
	return 0;
}
                                                                                                                             common.h                                                                                            0000700 �    Q��   ����00000000502 11472547453 014620  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #ifndef COMMON_H
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
                                                                                                                                                                                              kdc.cc                                                                                              0000700 �    Q��   ����00000016235 11472526412 014231  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #include "kdc.h"
#include "common.h"
#include "PracticalSocket.h"
#include "blowfish.h"
#include <iostream>
#include <pthread.h>
#include <cstdlib>


const uint32_t recvBuffSize = RCVBUFSIZE;

KDC::KDC(char* newSessionKey, int newClientCount, char** newClientKeys) {
	sessionKey = newSessionKey;
	clientCount = newClientCount;
	clientKeys = newClientKeys;
	
	size    = new char[recvBuffSize];
	request = new char[recvBuffSize];
	//nonce   = new char[recvBuffSize];

	ida     = "23547137288161";
	idb     = "23458137288161";
	
	kdcSocket = NULL;
}

void* proxy_function(void* kdc_ptr) {
	KDC* k = static_cast<KDC*>(kdc_ptr);
	k->thread_function(k->clientSocket);
	return NULL;
}

void KDC::execute() {
	cout << "KDC initiated" << endl;
	unsigned short myPort = KDC_PORT;
	try {
		kdcSocket = new TCPServerSocket(myPort);
		
		while (true) {
			// Accept incoming connections (blocking)
			clientSocket = kdcSocket->accept();
	
			// Connection initiated, time to do things.
			cout << "Client connected..." << endl;
			// Fork a new thread to handle the connection
			pthread_t threadID;
			if (pthread_create(&threadID, NULL, proxy_function, this) != 0) {
				cerr << "Unable to create thread" << endl;
				exit(1);
			}
		}
		// NEVER REACH THIS
		// would normally delete kdcSocket, but since we don't ever exit cleanly we don't.
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	
}

void KDC::getFromTCPClient(TCPSocket* sock) {

	// Grabs the lengths of the transmissions to come
	// Performs the receive and populates the proper char array
	unsigned int f;
	sock->recv(&f, 4);
	reorderBytesToBigEndian(&f);
	//cout << "received len:" << f << endl;
	char* recvBuf1 = new char [f];
	sock->recv(recvBuf1, f);
	memcpy(request,recvBuf1,f);
	//cout << "grabbed request:" << request << endl;
	cout << "Received from A: (Step 1)" << endl;
	cout << "  Req = '" << request << "' (len = " << f << ")" << endl;
	
	// Same as above, except for second transmission
	uint32_t s;
	sock->recv(&s, 4);
	reorderBytesToBigEndian(&s);
	//cout << "received len:" << s << endl;
	//char* recvBuf2 = new char [s];
	sock->recv(&nonce, 8);
	reorderLong(&nonce);
	//memcpy(nonce, recvBuf2, s); //NONCE_TODO
	//cout << "grabbed nonce:" << nonce << endl;
	cout << "  N1 = '" << nonce << "' (len = " << s << ")" << endl;

	delete (recvBuf1);
	//delete (recvBuf2);
	
	//cout << "finished recv" << endl;

}

void KDC::sendToTCPClient(TCPSocket* sock) {
	// Performs the sends
	//cout << "Preparing to send to client" << endl;
	uint32_t sessionKeySize = strlen(sessionKey);
	uint32_t requestSize = strlen(request);
	uint32_t nonceSize = 8;
	uint32_t cipher1Size = sessionKeySize+1;
	uint32_t idaSize = strlen(ida);

	char* cipherToPeer1 = new char [cipher1Size];
	cipherToPeer1 = sessionKey + '\0';
	char* cipherToPeer2 = new char [idaSize];
	cipherToPeer2 = ida;

	//cout << "Everything initialized, starting sends..." << endl;

	try {
		// 5 buffers being sent in total.  Reordering the sizes so
		// the goonies can understand, then sending them, then
		// sending the buffers.
		// Setting up variables we'll need for blowfish and endian stuff
		uint32_t shivaSize;
		uint32_t cryptedSendSize;
		char* cryptedSendBuff;
	//	bf->Set_Passwd(&clientKeys[0]);		// set the blowfish to keya
	/*	
		// SEND 1
		if (sessionKeySize%8) {
			cryptedSendSize = sessionKeySize/8 + sessionKeySize%8;
		} else {
			cryptedSendSize = sessionKeySize;
		}
		cryptedSendBuff = new char [cryptedSendSize];
		memcpy(cryptedSendBuff, sessionKey, sessionKeySize);
	//	bf->Encrypt((void*)&cryptedSendBuff, cryptedSendSize);
		shivaSize = reorderBytesToLittleEndian(&sessionKeySize);
		sock->send(&shivaSize,4);
		shivaSize = reorderBytesToLittleEndian(&cryptedSendSize);
		sock->send(&shivaSize,4);
		sock->send(cryptedSendBuff, reorderBytesToBigEndian(&cryptedSendSize));
		delete (cryptedSendBuff);
*/

		// SEND 1
		shivaSize = reorderBytesToLittleEndian(&sessionKeySize);
		sock->send(&shivaSize, 4);
		sock->send(sessionKey, reorderBytesToBigEndian(&sessionKeySize));
		
		// SEND 2
		shivaSize = reorderBytesToLittleEndian(&requestSize);
		// TODO ENCRYPT WITH Ka
		sock->send(&shivaSize, 4);
		sock->send(request, reorderBytesToBigEndian(&requestSize));

		// SEND 3
		shivaSize = reorderBytesToLittleEndian(&nonceSize);
		uint64_t sendNonce = nonce;
		reorderLong(&sendNonce);
		// TODO ENCRYPT WITH Ka
		sock->send(&shivaSize, 4);
		sock->send(&sendNonce, reorderBytesToBigEndian(&nonceSize));
		

		// SEND 4
		shivaSize = reorderBytesToLittleEndian(&cipher1Size);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&shivaSize, 4);
		sock->send(cipherToPeer1, reorderBytesToBigEndian(&cipher1Size));

		// SEND 5
		shivaSize = reorderBytesToLittleEndian(&idaSize);
		// TODO ENCRYPT WITH Kb, then Ka
		sock->send(&shivaSize, 4);
		sock->send(cipherToPeer2, reorderBytesToBigEndian(&idaSize));
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	//cout << "Finished sending" << endl;
	cout << "Sent to A: (Step 2)" << endl;
	cout << "  Eka[Ks||Request||N1||Ekb(Ks, IDa)]" << endl;
}

//void KDC::create_thread() {
	
//}

void* KDC::thread_function(void* clntSock) {
	// Guarantees that thread resources are deallocated upon return
	pthread_detach(pthread_self());
	TCPSocket* sock = (TCPSocket*) clntSock;

	// Error catching and screen output
	cout << "Handling client ";
	try {
		cout << sock->getForeignAddress() << ":";
	} catch (SocketException e) {
		cerr << "Unable to get foreign address" << endl;
	}
	try {
		cout << sock->getForeignPort();
	} catch (SocketException e) {
		cerr << "Unable to get foreign port" << endl;
	}
	cout << " with thread " << pthread_self() << endl;

	// Perform the authentication
	getFromTCPClient(sock);
	sendToTCPClient(sock);

	cout << "Authentication with client complete. Terminating thread " << pthread_self() << endl;
	// Close socket
	delete sock;
	clntSock = NULL;
	return NULL;
}

uint64_t KDC::reorderLong(uint64_t* i) {
	unsigned char* chars = (unsigned char*)i;
	uint64_t j = 0;
	
	// Performs the swap
	swap(chars[0],chars[7]);
	swap(chars[1],chars[6]);
	swap(chars[2],chars[5]);
	swap(chars[3],chars[4]);
	
	// Casts back to a uint32_t
	j = *((uint64_t*) chars);
	i = &j;
	return j;
	
}

uint32_t KDC::reorderBytes(uint32_t* i) {
	// WORKS BOTH WAYS
	// moving bytes like so
	// [0] [1] [2] [3]
	// to
	// [3] [2] [1] [0]
	//unsigned int temp = *i;
	unsigned char* chars = (unsigned char*)i;
	uint32_t j = 0;
	
	// Performs the swap
	swap(chars[0],chars[3]);
	swap(chars[1],chars[2]);
	
	// Casts back to a uint32_t
	j = *((uint32_t*) chars);
	i = &j;
	
/*
	// moving bytes like so
	// [a] [b] [c] [d]
	// to
	// [d] [c] [b] [a]
	unsigned char a = *i & 0xFF000000;
	unsigned char b = *i & 0xFF0000;
	unsigned char c = *i & 0xFF00;
	unsigned char d = *i & 0xFF;
	cout << "sizeof(char):" << sizeof(char) << endl;
	cout << "a:" << (int)a << endl;
	cout << "b:" << (int)b << endl;
	cout << "c:" << (int)c << endl;
	cout << "d:" << (int)d << endl;
	
	uint32_t j;
	j += a;
	j += (b<<8);
	j += (c<<16);
	j += (d<<24);
	cout << "i:" << *i << " reordered to " << j << endl;
	*i = j;
*/
	chars = NULL;
	return j;
}

uint32_t KDC::reorderBytesToLittleEndian(uint32_t* i) {
	return reorderBytes(i);
}

uint32_t KDC::reorderBytesToBigEndian(uint32_t* i){
	return reorderBytes(i);
}

                                                                                                                                                                                                                                                                                                                                                                   kdc.h                                                                                               0000700 �    Q��   ����00000001703 11472376710 014071  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #ifndef KDC_H
#define KDC_H

#include <iostream>
#include "PracticalSocket.h"

using namespace std;

class KDC {
private:
public:
	// Instance variables
	char* sessionKey;
	int clientCount;
	char** clientKeys;
	char* request;
	//char* nonce;
	uint64_t nonce;
	char* size;
	char* ida;
	char* idb;
	
	// Cosntructors/destructors
	KDC(char*, int, char**);

	// Socket Stuff 
	void getFromTCPClient(TCPSocket*);
	void sendToTCPClient(TCPSocket*);
	TCPServerSocket* kdcSocket;
	TCPSocket* clientSocket;

	// Thread functions
	//void create_thread();
	void* thread_function(void*);
	friend void* proxy_function(void*);	

	// Misc. functions
	void execute();
	uint32_t reorderBytes(uint32_t*);
	// These next 2 functions are to keep things straight in my head.
	// They simply call the function above, as it works both ways.
	uint32_t reorderBytesToLittleEndian(uint32_t*);
	uint32_t reorderBytesToBigEndian(uint32_t*);
	uint64_t reorderLong(uint64_t* i);

	
};

#endif
                                                             kdc_main.cc                                                                                         0000700 �    Q��   ����00000003053 11472376722 015236  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              #include <iostream>
#include "kdc.h"
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 4) {
		dieAndShowUsage();
	}
	
	cout << "KDC initiated" << endl;
	// Get session key from command line
	//char* sessionKey = argv[1];
	char* sessionKey = new char[16];
	int j;
	for (j = 0; j < 16; j++) {
		sessionKey[j] = '0';
	}
	for (j = 0; j < 16; j++) {
		if (argv[1][j] == '\0') { break; }
		else { sessionKey[j] = argv[1][j]; }
	}
	
	cout << "sessionKey = " << sessionKey << endl;
	
	// Get client keys from command line
	int clientCount = argc - 2;
	char** clientKeys = new char*[clientCount];
	int i;
	for (i = 0; i < clientCount; i++) {
		//clientKeys[i] = argv[i + 2];
		clientKeys[i] = new char[16];
		for (j = 0; j < 16; j++) {
			clientKeys[i][j] = '0';
		}
		for (j = 0; j < 16; j++) {
			if (argv[1][j] == '\0') { break; }
			else { clientKeys[i][j] = argv[i+2][j]; }
		}
		cout << "clientKeys[" << i << "] = " << clientKeys[i] << endl;
	}
	
	// Create KDC
	KDC* kdc = new KDC(sessionKey, clientCount, clientKeys);
	
	
	// Run the KDC
	kdc->execute();
	
	// Cleanup
	delete kdc;
	delete[] clientKeys;
	return 0;
}

void dieAndShowUsage() {
	cerr << "Usage:" << endl;
	cerr << "./kdc Ks Ka Kb [Kc [Kd ...]]" << endl;
	cerr << "Ks = Session key" << endl;
	cerr << "Ka = Encryption key for Client A" << endl;
	cerr << "Kb = Encryption key for Client B" << endl;
	cerr << "Kc = Encryption key for Client C (if there is a third client)" << endl;
	cerr << "Kd = Encryption key for Client D (if there is a fourth client)" << endl;
	exit(1);
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     lab3-fin-v2.rar                                                                                     0000700 �    Q��   ����00000064572 11472637605 015625  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              Rar! ϐs         �Tt �) @  }  R��o �t=3	     BFtest.ccQ���A�����$���%!\��ۂ+"���D�`U`�JD��*�TQ���ˍ�IK3t���s�@��#l��ts9�O��^�f�s9��������Տ�K�]Y�l(�*��`��*dΪ��a����|V�O.���a���V��v�h�Ë�������Y}�qݻ�{{���_�����!GK�*'/�{ٿxR��>��΢u��5 m����8�r'f�PZ���e�V��r��'���v�^�ϑ��ۏ���.�U�?oo[�p��m�!D�a�}D��p��f�7$�B
 ��/YX���.�GTJ��z�Z�G*��!F��:$(�py��M�S�'�Ȗ
��� *����d�GGH~-�ŧ>����t�dj�:<��[1�Q�ӒBs��AoMƹ������1 q��(��C�?����ge�r/����8����Wl\Y�i�S�8(\rl5�r��#�yE.�<j=�1��F`A�MX�T�_���SVĪ%��o���A��P*�S,s|�ee���pC?�Gӟ�us�hD?v��G�G}gt�F�$��G��<���Z��������U�/�j�A
��X�X�,��f��u�,��[���]*]�/i�ղ�����>�n%n-<��(OB�[��� b��SX���ށ_���<�������:5��N�-�~��hm�q�:��[��1��1;4��P&ED�_-�sn� < 0��S(�Tѫ����1�<5���"�\�
@E+��`��a��Ad�)+|nr�c��Y��]X��O��_�݂�@Hn<�|�q�iU� \0+
�,�E��rGF�V�}ŋ/n|��+Y�����; ��b��w��^�,���R++)���r��2֊ߊ5�]�n���?�|a�+�.PJ��6@�~jN���F3."��������i�������ED�������j���Rd�Z�Q�QD���j�6��F�P��BBԩ֞�Oe?��Hވ~�Cў
IW��0�hBnؤ�8�!9�lr�^(Sݭ��/\�%.8%y�F�(�a'����dO�UX�n�?e��cN��va<�T�΢jq5&f�
�Q3�<;�G���4D�n����Q����o���<'��x����^�'��C��%n���y��'fSv1�>,]�w���<��x�*��3�)�.���'X#�b2]P��[Y�޿�z��:P��7�;��:��6)H������<�����6M-��H�^�~Sw:<�Ђ���1��ۂ><�5T�AR:����6�T>w��^@�X�c�l�0_�*}��A�[d��.?�h��:ϔ2�2����?�.�t �0    J  ��sU�v=3     blowfish.cc ���2U�� ս�Dg��!�d�Ā�֊O!�E<q��P�|o��f !�H�+��K<��A���T$O��܂�)8�<h�`OV�}޶�S�G�ַ����W�zW����l���������x�]�s����ӗ��l��r�9�<�����}õ��m�=��3�������������M����&��o�����o����<;=��r}�ɜ<9���J�:<�M������tD����be���x�o��i;ϳ�����-��#����������;5x>/�������̶��{��n}�����e��Wp����W�g���>'�������nO��r����W�����_>�{=��}�u�����˳���w��5��󹾏7����#-���{=ݓ����&b�+�־V&y��~�����w�o�0os�������|�'�g���5���O�s�W��
w:<��Q�o^nnwL%����|�׾�}����l��S��k������wS����ޟ�}�V�?��>�$��m3�y�O��C �;�if��=�8�?��w��Xm������^ݿ��m��4絘o�����������i=z�5�N���#�="|����ݜ�`�w�g_0���?������*���sg�?wdzU���G;_��z1�/o���}������<��&����+kRrϦ���&n�޹]��{���G��L+�h�֮�W�(�+����g7���`��x���??��f�|l���\�߫��͞��S�䍗��5��}}�o7��4?�����B߳�l�������,�9}�������Я���׃��n���}9�� ���2������o�c���e��vA���]�vg�[��>O��r�v_d./�hw��;PA��]Y+��A��A ��{��gQ��|^nOe?�ࡑ�)�ͻP��S�},�Zot����۞.�����i�A=���>����n�P��K�\��>�MG��t��u�.ĩG�I�6G�d�gi�����5��c>�zֿQj}�?:��Q]?<s��D�7���τj���<S�6'��>�*��z��oN�U�����on�]���j�Cު�Ѻ����u�* v�C��zʻ��+��ޱW>m���6]{oW�^i�z6�[o>�Ͷ�z�ſ�������C���!�n��C�n=�%[�u*�zy:�+�g;:8^�\�?��W�[H��D~�{;Zu���ܽF�Y͌x^�fpTѕw�ot붫��ѵOD�[z�蚋&��i���/�k��Jڕu�f��GfW�1�����˥5+�O�oY�86,�X2�k�9C��������p���Ʃ��FVҺ��������sf��.�m>�G�����C�7n�+�TQ,X�f��l+��^�ޕ�K�K��m���n����kj�Uv:*�;�s��)Ս�]$X�o���pC��ֽ����泖<k��]����d�Xp�� ��e4�EX��7�Zm�E5+�8�b��8ޭd��	ͪ���������Ɔ�U�T�a̲亗>*�y-�Z���&�E�V덭�t�ix�Fr���3!׍��m�F˼s���~����]ӂ��l��y.�ʪ�uX�{��N+(i�܋���_��;v�A9V�-i��陴��Wzo�(�r�8��h�>q4�2������9cb�u��]�Z�L���ũn����.L�k��KW2|����ƕ>���W��;[M�L��uT[�Nv��KaCE���{qarj�x,���;���.�.W:�H!����˺[Ht7�3=-��F�$�6/M���U�a�����4��A��l�F�������M;��V�2N?��OI���3�иb7��hp�zd�8(q�̟!q�+��qYk�Ӭkln���&�zHh�_�̤�:��̸]��y�I�i�0%�1�E���НF�<_���i���U{fb�Oz�)�6�8�i�8o�(b5�4M+���6�A�i��`�!�٥t�\h����H!�f�*G,9,gQ �`���(�K}�tm,[��ފ�hM=8���U��s�"�+ރ�=*K���pVï3�Zf��|f�v��ub���6 ���*G�?�+�K��eF�ް�3R��t��)d��S@�@���ק$n" mRlw������i�,��׫�-.9#!�DOK傁8E3NUp8ה���4O>�hm|~
;�#�*�sjG�i/.r���ƽH��3d�f�4�hO~�.�g�Q�c��j�M��#��t8E�9U���(�{��f��tws�1D5f�	�A���nn�����ƣ�+��Ka�쨎t4���<A�3�G'��ݠ��姚!�ޗA�6E�4Q=�"�P�(j��ѷ�將�����L�?UZ+8�v���x�>)Y��d��;��D�)�Ƴ�Dn���CQ��[jlq�G���s�{.��?r�[�@u�ɋ�G�k�mZz���E�{۫�=���k���~� IuD%��u$I���9X�G�L�l��Yn��g�J'uB����E6�t��Zy�ᜩ�����G���?)�v�jAG�."q7݊����b%|��]N��ې�_�ZS:�*�ah?�[<�稆��O���q����>;��4��Q}����&��D�(����IKDܦ�-r~��pc�3�ə�� ѯmN�1���D�R޴�7�@��[ 9�M"�"~Q�v�WBM�U��'�t4e�J��Я�	2����N���f��NH�>P+�9J�����
z�%ƀ���{'e����ʪgK ͙ڒ)֙il�ӃRZ��R=��Wrr�e�.��3��Z;��(;.�m�m�J�XtY�sRE��:��L��ZtF,���� l���H���G��.\pt��cz ��Anr��"���O�YJ�َ3E�j�؜7�k��[�+]&��Iܒ���Mk?4��	���Z�&���iӮ	t
[[��V�2�5�I����׮lt��!A��M�2NCH_d����M���|�tr�g����/[Gr���n����:
���d���دx�4�\���7fPA�nIY��m@�w�Y�AW��HRL��5W���c���F�2�EmA9*}F&�JR�r!�W-=����d�����z����5/i�XЈ��y0�;�d�L� �9Y�c�Xn��*5�Ɍn�Y:k�F����-�ѹ�̨��y����}*�j�)���Hl�9#�3��ڪ�wl]4�VN����`K���7���2ӭ�Na6��M��s6@�Sc�KA��@��k%�!P�]�c��h�߄�\s,���C��iQ�BX��؝J=%�bDMh�@�,�%��B�Đ@���=)����&�8`Y$��n��F�����Ӛq�|0F[� ��6"d�r�ʒO�.�u�I����S���b�w��w7���=��9py�+��3͈��R�O �SH�-��@D��S�SOK����U'�D�^Q�"��$z�@�ĉ���V5�^���,t0rNtǧNAq�{[�%@�����B��&b^��Ec�	c�#�qG�5���M[�w�D��"#%�K멪�:m6�UWTG����J�a�Ğ�y&"�ʠ�v�~�X�#���k�W��G�K4����jIYU�Ej�6L�
l�����ۅ��D)���%r�{x:5���0�a��ы���`6>Y�����L��H�\.G{�3��ٞ h���ڪ�GBK��'(s�wj��;H,c���zZ�����
�:�Czr�D⚒IYu��:�$��P�*��(6-9�u$BL�oJȈ�T`(%��J��-�	� �M���=��Ul���bGu�	ސv
�	W�:�
��mK`�IJc,�.��
�����6��9�H"r;<Mw���4M�L��@"�ty	X� =�vq2�L@�i�]��<H�9b�H()&<]W ̒��d+�^�ą0��i:�N3���y�t�uP�u-�c�t��&�dI+&S�tRN}hR�t�ם��d1r���x.\F$O���9(3�NP��`\�
>��%�dQ3`��˙�� ,g4��3���K3�ޝM��Έwϔ@wZ�K� W1I���!娧Zp����&R�Mg�+�;ס����$�M��mA2�K�R�5�?ߔ���$��}c,CT��/�ym��[X�F!8*���25�|�[�54vE�/
A��tF�tt��''��]�$5(]V�Vs�d��F	 	��8���T�DQ�)�#N��J%j�VR�He��\��=9�e�m6S���[�JI�n'Q��`��m�j���l��,�$��W���Ga7 W6"=�35H(sC%��S�C�q��SC�5z��|�I�I����)�q�w�P��1��k�Mh�O?aHظ�P�]MrT�A,���~`�il]�rT��V��lr�:u�'�5PL���xgdNm4�8ȦC{�Lճ,J�pt�2d�H����
S��4��@x�7hJ�E:�F�O�3��Rܵ�V���'��C���S�~Fx��L�-d�D �u�gBbr{��.�@6U\��|T|�'���T������r�X" 3�l�ɻ��O.�����(��
��P�`u��j$��Q,���@�z�lI�oxd�n�z8x�:5�J�#f��{��i��k"�ا0�* ���օՂ	�QM���؏�`�w�w`�PQ��U�j��`VD��D��$��Ѕ�ӌ��^�Wi-�(w��G�܍���d��+BJYk�MLuCި�¶�<�pB*��s!�|PB���a�^�PahrG_x)��*�|)T��z]��t*޺$��q���bt�:��_��ܖj��X
Hi%�@H ^`6a,���+'<��5�")�X�a�z�����4���*�jF)�ވP��%&��}ֽ�ц��d �ךu���b�񉮌ǀ?7F)E!b&���E�+c����RS�਱hI����e�ÚYjH<* ��+��n�"�>�,�R�iD:��ֵ$B�IҜ�����)���P�d-��t]�
�.E{(���\ĨN��"�V�1%#L �ڊ�����RpG�oti�>N�ՏX2�
C��&,*�u���T� ��9&j?"kQT"��tM�v�0(�3��Σ3�:�1%{WD�ޅ�3���T�-����{��CDZHZ4Pi)L[�5*�S��`D9��<����8>��֯UY���4`
�^@1�uJ4&M���X�f��k�/��0 ���G ��Jg҄NT�וH�e1�m����9�<,��O�AB13�L#�\��*��6�{�M٦�!r6.�e���r@s<Ǡr^Q�($����5�hA)�$`�� ��U	�q������Fpf"O�qxH#��}\��L�S&û�(;�:, _U 2�������1���i�+뢃�:�HSr�!�{"Dx'@� �6G�P)�;��Ѕ��K��Rg���K����=<��z��2�U��J��q�S�NX�l�D�\��&*�3�a���K�5:�@CDE�����z�Q���`��I2<���<�%鋮�S�B�N���U�Y����;!hX�D�p	�`��O��?H��ж���-"�>S�d����I�q���!t<�夲 ��*U!�f�[�T�T/5�$�&�{A��$s1^By�K)�u�֠��k���[!ur;܁���H�/?ǂK$� 6�3â�Q�W�LNˣY)Mċ�o4%=" �R�=+�@��MH�~᪘� ~4�&`��Z������L`7� �sh/�Cb$��{0t9��vqģ�H�I�uq9T(��]g�d�Ѱ�d�b��� ��w������@&�z��Ph�A��QI� ��I��mfY�D���au�O�:-�@��֛|!���=��3רr�`�MP�L~���;'�v�/W�<���(�*��q"+��\\V�hO���U%OW��qDH߂5�AoN�㪤�ivc�zz�����'�L�7�S>��̪F�u�G&	B����um6��)��(�B@�-�C�uD'@ȹ*'��z���$�"�I�D_�Z�Z@R5G�Y�b�a���'~��}K�ǃG���H��d!��P���n���阹�6$��=�TN��h@��ww-*���,�
h�ӥ*�����'G��m�.�q��Ʒ��)�.�Ĵ�Z�
���Nm<4���+@��{D��:���c%{�n8{�ԯ&�@D7Pܴ�_��7�j=�P�q�z�t3@1����E����#�^����2I,��0�?���Ci����5���H�Y5QTv�0�����A�_c�@G���٪H���z?�j)�8�W$5
��5�z��`�`�Bd��t%���
E���;8��s�C =����	0ñ?
�!biO
m���(E%d��0�\D�*i�7\yp_CJ>�F�B��#ӛ�-����[�f�l�-��K1��d��ƫ(��S�<��]r�<��j�R�ƛa��eU���"���M���>����)]��v�<��%dR�%���@�4-i��a-t����!�Xi^�����|��:! �=��^���ྎ�e%I��#ޅ$��D�%� �ZRb�@G&��i��LF�H����*��(��$z��^����t^�KQ�k����B�(��)TQk�|� ���xɪ�؞ͧOJ'��d�����r}짩?]1�d�����l�n��WJ(�GPĀ!d�!R�0:�(-g�ߛ�aGQ �P�"&�5��V)��<2a#I�/M��H���ĴN�����C�F*�Ҳ"%9z�@�J��F��$|�_MC�F��
�˅*b�n���[i��v�A�� 0S�?,�C�)��M]q�q����D��X,�=!�v����A�#ص��p�IJ*�/(@l�5�ү7���1�.;��ҒWg�3��/�v�u�X+c�$�6V��Mv�I�E�ѱl��c���mSoL�U��+���.�J�8`S�(MYKN�S]�`�+��5Ie��[���@�����R��)A'/�b�������� �B�+��N`E�5�0&cR���m2��#Z7�c��F��y!E�Ѹ<�	5��UwMT�8ȥ�%
*Aj��	>5{��e�f�p�ؾ��J��B���{I5�)��*��/`ΒisM�®^��N�� |_I$�	3Y����1�L��d�|��L� �������Caߠ&�&�o���<T��z�F��J(��أ5��Qk��뎞┓_�.jR7�r���2�n[WJ�2TJE
l���G��W�]�M$��{tRMefDtx3f��{|�!��a�O���%	����(u�@�Z�bQ�2�r4�C�S!)o���xUͩ��>�L<��a�L�����G�4�5Oz�\�Ȥ"g���Ԕm�GG�>5��ut[�Sf�" �=soM��F���=��:�mb��^�J7L���N���DAPq�!��ɔ7�ɒ�V��PH�
��<�EZ�þ��A$�1NI�P���v��(���8�����m�]!3D!���SO+��I�x[5-�G_K�p���<Z�c�k�*�<�����C
S��>-=�0R ��
���Uz���<��&��K�%��#�ɯW�{�?pd~θ�ϼ&�Ϭ�2�/[�e��:�}��oa������gG�c����!���獏����豚|�����u�9���G�Y�f1���f_�=�<�����{��@cb����}��:��]��gk~����Ig��Gw��No����h�}����oן���1�|�������?g���w�����O��X���K��_�"�j���t��r�ڹ
Z)9�M�����^l|�w����s�p]��/��O����I!�������@cs�V����b�/�<�לl���C�9�t},�<�a͝M�pw���oG/�p���Z���䮞��Ce����7��d?E��X����;ka��9D	�i�7>���do�����C�[���b"���f1oўX~��:��g����?��?�mY���+���K�ۇ��;^��u�����ǯ���_���{����������3�N~H�;2S/R��6��RO�R(����~�TOۓݒ��U������lt �* �  i  ;Tk��t=3
     blowfish.h	��=~�Bx:9,������FOⲻ,#���)	�[,��,'q���st������앒��G7?��kS�tX�	jK�0_L�T�/�˝�Y�<l��ԡű�����1�W�����0�6(3C�covuc[�>�}� U��A3��n	[t{w��Q����#L����^��"�&����i��:�
�j�Zxq�B%L%��8+R
4Pz[�C�I�t�{Fr,�*�*ې��*��C�%do;�w������������khj�	�9����Q8+ڧU�ˢ�o���1W���¿�R�qÈ��ާ.uhiFC)������3�'o�a5em�_>A��s��C�y�*���˫�{��^>�&�E��l�+a]��D]�{M9|��i.�GB�#�
xt��'p�m#�S�E�y=�?�`
�
��i1��k���X�O(�[�c�n���r`�7��UA6��w^�T��=A�A����g��p�t �. g  D  ;%�<�v=3	     client.cc �k~[���<�ݿ�����L��I6�	TzТ@9�wkziR�m1���;h�ura��e��bX� �	�]�+tm�]�^^_�b���	^y��,�ʻ�����޴�l7�e��=sh��a������q{X��V���Q��7���?f���
m�tq~�8�Gd��g��+a�NV���S5����nN�e��68g'��� γ�X�oލYw��ՀO�� �5h��ѩG�WOS�O4pL���Fk���N��T�ҎD�I"��q�X�\���g�m�fH�h���F�"{7��}OV��l�ciu�����jX4^jY��1|���i|�{�^ݳ��L�m)�� �/��9�z�
f&�p9o��yXt1�1�nf�=�7{�ܵ(B���-��@+�㴴:����� ��'���zm	�y�����
�y��d]���l P�83-�>�۷��\�#L��/p?�д�^�?\9O���db�B��ծm�7.!W���?��ڴ 6��u�8[��ǁ��Q��~?�xU�k�k�1��|��"@4�m��䭆�繌n�#
�2uBLajD��/ݛ�����m��׸��g8�dB��+�і��d;���2���w9�(����� �K�θ}Egz~��ې�R�����1Ǒ�G��GBk8�a�5S��M��۳_�2���/����X�����"�#� ��z�8�T�v��T�QK�;�UH%$�bG�Ϩ82�&�o k�.�nV9[s��i�er.\1�l����O�����T�
�f���/a峐�a����"'`UǺ��cf�ۤ�[�[8�w#x�u��^ �%�]|8.�;&�6Ac�	�
��%��xd"^�Ev]9M��C������hd�����
^F��1K�c���ofј���O[��p8|�� O�)�^����80�P	�JKѲ�����b,S]P��q,t��`�+ 嫦�\JM��`i�aP,��qW���۶� `��=�����_kj�kp��u�8	�n`"�C� ���o���3�
��j�j�����0�z1�����>o���my��׌z9�O4"��~�t~DSh:%����졄��$��b�p	0��6퐈H6�1�?���O�e�۸�K�bNC(e�s<.>g~�d��,�Hv�H��:���{�A�V��-=4����t�d9.S�����g��͌`G�8�E2���n�����q\�Yԇ��q54����Bq�O��QbA���)�q�q�m�YƳ��]�H�UV�W<�y����) g�&��N:�j2�3��i��C��V+K{���3�Q��	1���P��nt���HDKkKQ��.AG� K��O�㨉�t˂Z���zy����~k�,(rD�D��1}q�5�8B��c�u�1l�+�ȋƎ�Q)�-�ζ����dl�b�J\��d�l�b����L���)������w�0̴.�I�l�J�w� =��	}�lZ-!#�s���h�W��"u��tR��*S��kU9�vɞ(Θ��A,#�b����C��������aׁlkO�dɉ�I�F5-�$��>���CUC�@10�3���K4�-�����u7,VUm�$��4�ҫ�+�F:�(�U^��a	�S��G7��|�r���{5W�.a����8}y5��}nE�[�娨g�k|�U�k��&�X�|�}�p�?=�y�W wa���ʔ��V_�9k���%�d-�N�R�m�����Ѩ��t磸N�V
�{�1ܤ�t�Z��s5��,�P ��N��>b�:�?M�	U���O`��d��1^�2���g:!���˛��� �k���F���qC��%5�T��k�,w�K�鳝��E7W\�7'E7[=ˮ���ZB��Ց�"���_�Y���_�FL<�O�kh\!�4����u^{�01؈��Tu-Ll���k��{f6��B+ís�5�Zh,SxUpV�^f��(}MD����X����e�Z8��<���5{3u�+�ɖ䜦�قG�T�0W��s��u^���T�?�ݕ^�/��f�9���/�`�����U1(��L�	�Nv�R�hi�xڛϳ:����`��d%ͦm�k�yTymT1���UG�OTX�UGT���TFCQ,r�_Q?Y���%E`[EE!��&�.�W�Or>n��-�"�Ѥ�s��՟�b	�OU-[�ߖ�YP��,���
�*�hw�\��p�*Z����s�5W�%��� �aP��!����3�j��ު�DU�~��ϔ�B<��EO"l̀;wmi�yI��j��vk�Q���T��|��K�)jh&�x��pOE��5f/%M�V��N�	������N{,˗-������I��)�����oU���a�ћ��S[�^��X�;��SR&ܦf�D�N��x2��݈x�upy���f!��OFU�Iq3�M<Q&��Oȝ4~���m.�KZs�<o�2��9> Fj�9Dyz&,��IJ�9y�eYSc��Eq�^Цh�/?��Z�@*���\Џ盓�y�ˑIe4H�E�i��r��6�.�����Iex��d)zm���\��w(�y1�A6<R��5�����ˈ�G�i)���oh�*zhQ3	����]��Z�(�G��@u.�*JyY)�JB�X�K��<�F�Ph���-�&������Ɋ�����(�m{57��`�<]���Ӣ�8֥�A�!���*C�z#S��%��aF@<a��X�ĥOg���)����	�l��B�*�G���s$�iR8yױ:�
�kŊ�<���U{�W���t5��$�벫��z3�gk�V��m3�8���Fʔ�^c��v����˩����b������F?��wEN�L�ic�Z�8�e2��<4<�k�T����IxH�;I=�!��i<����j�K�B�6W�����)�EϝR!���JD��8���B�N�6w륝�F6ح��)F��n�v��\y�Qk醥�, <d�? -�z�6���-x.96q���o#���ߦK�"
��u[���|#�)���|����P���-̺S�1��=��ú�1XpT��DI[G0��['�>"s4����� ��������2�K�S^n�O�n.�ܪ��!	�5#�!��&֎�z�P�9rl��)��4��>&�i�S���hC���4F�QN��dͅ/�E0�cL�C,J�b��J��/���\�9$lRٚ͜���R�0���W���?����MFVz���R�w��&�v�[�#�*��9�`cB�eC�	����z�f����W�ƍ��xթ���zo��#3��~::�1k����pK6��?v� ���`�<�+��y�^,��R�f�+$���{�שU�H���b��!WĊjx�p4���Io��ZJB�?ўhU��.�yc��ѡ?�Q`ki7�"�W,Ii
��:��I����`/
���[����pa��u�{kE7tLǙ�S�Z<�9���u�E�أX�܊�ըpG V�:U���
r�,܎(��sI�b������]|
�r�N�5���C��ʘ��S�N�{ �m����"�X����c�L_��-'����@��}m�Ѐ L9Xۛf��g~]�b���m�m����X��_�ƛ���Ճ�#g��j���n ��i��J�x�BS]�H�*%g�!�����2no~�Nw��U?yS�p2����Ǳ�	��+W�߯�,[���9
��%������b��*��APX���?��n��Ȏ}��A��:٭b��k�
8�z�:A:���Dk��C�'��D��[Τ��w�&H�T�SIDʢ���'�ZQ8�"P|�*7-G'�uU�˅�:�%�a �QY)��Շ�c|�V�w$���	,��?�8!����2b�!�2�Fͭƭ]:�ϛ̠o��Ҫ'7R�XR�d�{�p�!��ʒW:�����W"*9zQ=f�|~��FwEvO �y0�_SSm��Q����ʌ,Ά�:w[áƠ�A�e"yXB��̄n�8��a%^�B���ԫ��f<kџ�'����@���-Ȣu����-{U�$�zN�����/-�)B�mFd��F~b@���·7� �46,�s��CD	� �Fo}V77�)�~r��T�T��(��)\�9K����t �- �  �  ��yQ�tv=3     client.h �Zf$P��́W�?
#`�F�䖊(ףh���X��b��4�h��J�T�m��9#i�	E$ܑ�}�s�s�W�s?'?g�g^s�ў�g`p��>/ף�����MlA0�Ϳ$�O���+Y�V��rɶ�q99��l"|�u���6�)����<�,w�,E�ǈ�o���OI�B/�<m͇U���*<�֍��٠i�t}�m��vW��v�����Iq���f��J�es�燛������d�$~&b&���Cm͡-pL���&�XO���R&t���7�~�?�$~_�R8���{M�_҂U������Bg	����^�P�b��nƵo��$���5�n�H1�<$u�LT�m�|,zb(��chM G6�iNN��З��~ר�F ��r��r1z*2&��mi�O
gG_=G)B����O�Au���t�H���΅\� Qθd�'L���79�'�30h����f�Lo�!Q> 4�*����d���iI-�#ۅ�Pp�I\-�ҡ��ܳ{��?^��f��r W���v�U�����?]2�*�3��<<�2�;��~�^^FL,
S��]�®��
�`h{?Ï���T���b���f�=�M�I�)NP�� �,>�5�b)�a�0Fo�uT�@�ߚ��9�	�~wY2�(�	F���U�P#����(�B��U{`���B~�����u�k�*�sT��v���)莍P:Ij������$��L'�$+2ש*�<ʚD�@ܵ�]���2&`�8 a䴌~��r��f�Z-�|������Nk��$M$���������o���ؠv�E=��)�6�B�t9�+/QSRz:�Li�kI�-�c,ڪ��=��e�+�p�g�~��Vk�h4�~�!a:������j]�� ��h�~�K��X���(Jt�vL(�X��Ŗ׀k��>�O�[�H��o)Ԅ���M�U�6^�Bĵ�z�FEEGGs��u���%ItL�lroC}³��O�cA�0�͵6a�0�	٥w�ڬ�����=���»����3 ��{ ��eN�?0]��P����$�9�P�}	�[?�G�긔u��<p��W��@;fZ�C�t���X�Sk�j(]í�~�[�ތ]En%�E�}~�_�~��nHW�����y9*�Ax�]Ts�����1L���`t �. �  �  K�F�u=3     client_main.cc����=��Z��9�r���nYE�����@�e&RQ**�,��n�R7I)�-��t�aë0�㻘w4�7t}�3@��y�5zG��Rh<:���m#�c�ϧV�]CN�G�v�����L�}�*lM��I�< ڑtALe��ۆ!��۝��u��XW�a��]��=z�Fl�ύ�Hw=|��mF��Y��o&k�tQM��򮂰PNzZ&��U5��> 'g#0r�Ǎx� �E��=��~��t��F�M۟��,Fo�ȥܑ�p�9�X�_�K[*E�����z3$���^NF��%�8���Ƴ�O�sB��s���]�!T�)h�5��4e��&��ld��E	xV�m�@�k�������ձ��η
��>v�o�_i,�8�Y�5s�7��
Y�)�!qb�f�l��nIG/�Sy6̸X;wCg��͔�G��B�~%-����{������E�֚R)ϲ���9I�#�����b��2�RH���D���2`t �- �   B  +%{�qv=3     common.h ���r	������#�SD�"��<���0�"L�
�L�&�
��A7vn����/����e�5t�(�4¼�e����+�D�֬�hB���ތY񘺈 �ް�#�n,��0����x��P�V�S1�kˁ<=1c��0��/3�{��WW�A@�f*�p�W_j�o�������Џ|�{I��]��Z�\�W�)8�
��zy��ȷc�H��2��g?t �+ F	  �  U'p�^v=3     kdc.cc ���!��<���|*�j�nd�$��"`$x!�#DI��{8Bq�����	�����ێ��۶�٘.���Y	*��W�����UU�_���ڧ����8V\T;VWZ�	��dj���'a�u�Tf�J��쳖�k��(����u�Խ����}�U���̄_���v�N���������,/�t]?�ώ�'��[�r�p�n�J�Q�"��++��_�_! 		p���OX}��d-�g��9��ݖ�[��<@�N�� h�Ȅce`~��� ���`��*����*��򰲂2��J�[>�u}�6����ЬC�ƛ;[c�"i��?�����柭`m��}W!�(0��'�`u�����@5��QR�e�J$`Q��w�?$
F��]�;����!u/��]]"��k*��	`h'[���Z�	@r̀D��]��2�Zm���`����X��Y����E�\6,��vH�R6����6�u�WO�
�b����UU���ߡ� �x	+�����TȘ��
��~��
���G�����~�B[$��sX���TL���#�1�	��P�a��� 3ut�h�@�y���Eb���ҡ�oR$
E�>d�;�F�6��!�`��6����}Kl��0aՒ۹����T�/��y���sW�������V�<�LWXY����yFY�%���W�9f:�C7Z<��Kf�W.\'����3>7�����&fU�� ��IbI=�z��_[�Ғh��7@�K����m��O����*9��'���[Rۅ�Y�����m\/�ע���/�"@r��X̕��&҇k�"	�,`��F�	S�v�����cChD�2�6�b�����-K���m�@���Zk��W�� x�E�u.u��nw5~Đt���*hq�/�Ιk��3 U3^W4��-(��_��:���?�"$�����&E����
�t4)����՚-CU���|�J,���oQG2Vڃ8Y\���Z��3���N	��$������5�ו&*x�:�ԞF����9�.+���R���n~n�V����R�S4��h�O58�Dh�$3�� O��t�f�aZ��XƯ��9t���Q�� �������a4nJU؂<
6K0:�O�k(��B�c� ���2!R��h���������c�m�aV0f�g5-}즊HaY-NG&�Յ�:U:��%?�HC�`��ƭm���;48��j��R�X�n�{�2��K���B�����&a8����.@#���O�2��Pα8H.�Hh�
ϩƍ�!��}�&����K�����O��bF� ^%����]Ҷ�p�)p����K��B%�/�g�[$Pj ��!M�X�,H��Ua� ad4�<���2�.�g[���C�9�d�$�{�ҝc�N�Ɖ��v� ��u���Ny���P��l�= ��U~��R�Y�.���`���ǲU?L�&���L�R@v��JrD
�˧H�է��1b��9zܻ��u�W���P�ܔ�`Զ}P���ҋ�o9���~MR&.xf�:��Rw� ���g�9��UPxP��:�Z%�� �=��B�rZ���OP��GH�"�/>��5G�;B������!�m�����th?�|$�>eb���>�#�����s���iWJ�9(�R������\J���|�o���C~8�=�t]���Ik��/T��T���]��Q�O&�[K�y8�O�@d���2|7u�x���x
�7�lƕd�qe?��P:C�Y{)��FW3RڠU��!��'� ���ԟ�C�uEn��x���H�"fb�H(fP�li��v�i�K�a~L&��(�!����"!�Ox���R!;�@����}e��pӶ��-L�t<3 <W!��8|>�O'ē��{��.���L;v)�TIrrM2=)��*�C��=���_��^�௛���aw��<���6��A�y�(\���/�C-6X@}z	�&��J��|gzA�e��$�f��,�����p�˄�Ie�`��v��u�Ħ	��_n2fH�Oxf(�1ϸ|T�������@z�Iu<�Q��Ľ�i��#♝�jXZ��C�}���j��%(�Eu����.p�����);�ws[&�L��IhS�t��<a ��9��,C�Q+�"�`#��o]+������#�Z��~���[�W�a�c�!�:fAq(X]����ǰ_�i�v�s�U3��=���;�5p��3x��GCr���B��Р[�eF����U�y�IE���T�蕷Ѷ��FS�I�XeM��jF�,yDXe�_R��Ar���/KIš��4;+i�+3��8,��?xNL\�Ӈ���t �%   �  �pͦ�u=3     kdc.h�L���{��,�v�$�/F�n�6�an�g|
6�hGy|&�a��0���e�ݷ'��O	�'Ǹ~��<$s�O��Y�)&g�>��X�"���f��B�b��R=����E�tw�M�VI����Fj�Y�#k.f+`4�븿1R�����/U�~,�E�3P3�S8lv��$�>gbrG7a*p��ڄ��J��]fk��^'�~�n՚��1�'XZ:�t��ӥ�zHYG�V��.�>(]�	�)�ف�q��;^�VA�˚'��W)sL&�1k`|�}�3�g{�a�Q��it�V�H����ŲY!���&�Ѣp�\��0lS�7�I uJ��� 4�B7�w���o�mC¯��(+}J�2z�^���n�>[���Ѽ�Ήm_��Bc�W�=�bFn~�K`��,����H8�D0�N���i��P��Ʌ�̒�����y��'Q�������'e���؇Rrq��kA>&��7���P�������:��@�/�h�A�g�����t �+ p  +  �l�v��u=3     kdc_main.ccM��w'�D9�'�r
.��J�B�`��ZŎ<m4�^�P�ވ��5�C��i7P��i"zzI�ß8|zIÇ�s�A�߂�3�n�&u�+::�rP��v�{-������B�k�K$��ְ�����eTz2�y.�2�al}�b�~�(l��I�|��F�^������{ʭ�	SV�k%��7I�en�A�4f�e�x6!���N7�S�d3��2=,������
�@�@u�=@�S��&�xz���4E��|�wܵs�wq����r��=$K�`B������N���L8�~��v�g:D�B�#�ǎGLBC�v�TcJ��E#f��B����r�����&�9������A*%��CХ���Y�j#���aso�$��q���_��2�lp�����uF?�Jf�nto�I��ʐiǦKV(ɜU2���!�`��ӭ����l�節<�U]�qi<I_�:2�X%<�e�)%�m��\�C�����ģ90��Ŭ�A�#�r������B�B��od��Uߝ�_E���f}1��H}��WA��*�����#�z`����/�M�_�)�b��O�&D/O/,���ju2�y�QO�-Gp��V���+�i8��t �- �   �  }G��]v=3     makefile �d�b	A���S���~�x��(^G�9##�1��M����,r:)%����of��ǜ�%�����ϸ_�o=i�KHP.�륈w�`��@��՞���^�F!I��D�v�.ِ>�s<us��ȼ�ڄ���U^<��D���2.&�2y�é��ک�`�����*,k?��+G^H�(�����b�����<�c���'�e�@k��;G1a�����BD�I	����t �8 >  �/  ZN�iv=3     PracticalSocket.cpp �>/V!��������Gey9�dݚ(���@r97`�$a�,h���.7y4�I99���Qѽ����>�i������zbd�Þ�3
_����1�p��	 �\ۄ�'�����q���]p�2K,����7I׃�k��iő���"ܓ������9�_��16�ck� y^l���{��h�#X�rɝ�{�;x�{�:;M�O5�4�.>�#���o"���t����Ļ���.�Q�q�\7��q�h��"G�(=~��Z����	 g�/o��~�Ù �?0���?O�T>�L�M2��"��S�mжL� ���]�Z{�1	v��pa{���u����Y�J]?,�j�=�������͘0�@=��,��C��9kr|@��=H
�d���yk��z�@�*���~K5<l��j�U�w߭r�4S�����5�uxBr&���$lhN�J�������p ��yG3<�3�e���jTa���ӳ�`����`����vno������A�>��/O|bb��}y�N::��O�����=^Ǜ�gvk�w���� lc��n��5���
��n���ͼ�hc����"���@��>Oy����h�ZӠ��2[�����x��;k�1O��a�Nэ�r�:�����k1.�����n��aF��!6s#�lr$�4�l�"���qg���ԻmF��v=7��[���K(�v�/���@�!Ĵ�ۭ�!��rv�Z��|J]4TM�L�b~��Frݭ`���dR4={��l�!.�6[���Ac�b� +�eD���Km4�1���`����~s�j*� �x����$�� �w�V-
�����(g���,��P�_#�v7/��rӉ@X�}Ԭ���A�щ�H����bP݀M��u9���#i���gQ���L)���:r�o��]7w��;�vP}D�W�ド��@�=�D�/2��CV��\$`0��ix����Mx�����]Խ�C�����G<&���KtF�����$��h�9��~ᄧV6�C�̭^X�S4LXk���~���Ca�WZ�����k����n�ed�0n8�@B����=g��;Jh�D�9��(3%����t٩Pꙍ���ǜ�I�8Ͽ^�7��Rdd9� [��5l��Sf�%�em�`�9n��n2�E�n3�Q1����ye��M4]��f~�}�h�$��o_�<s ��	����'QK�����m@ 8�0�	ϳ����uN@$Ø.�Fg�X�<�'���̦w*j�z���d}0�u�V�N��zck��(A�)ͯJ�+���j2�?�D ��R�9DڷF�qkF�kН�;�������<Ki`C҄G�/�h��*�/uF�u* ݪ��,�m����Rǁ���<����ŷt/�M%�D��q�����q#!j��4�<�<�O�T��* ��e53d�x�Y4Nݞd�X�H�t�G��f��|��|��.�_����i�!I��*iV�^q�)	�{��]R�2jgK�H���?#����jW5��0*'�y���ph���F����@Uv��UV �6�ܾ͞5�*�F�OV:-�*b&�v]i�y�J�<��T1?�9��jh��︴��;@5�f�w�)�0��sN )��Z�~ C��3���LvэAz�#�i�'�]_����2f 2�ag�J�:ւ�����vD���[�T�TjR}t����a����<dE'�)�z����d3ο�]3�O7Q~j��л�J�2B'>-w�n
]i[[����ŜKm�f��Y��@��rN���Ҟ��NR�>��-Ҡ����_�K@1����N&��|Jg���`@�C�	�1���A ��&���=z#�G�EDt�|fU�&:�	e���Jp�#8��+b���@Y#��/h
 �EY�����b[N�9��z���Y�#Xp�D��^Y�Zd�M
��َ��"~�[*��ݵoaQ���5��:�������OQ�Y(F^�2Ѣ�j�C'H3%"��9�!8g��a��2�,)�=��H�H��>�yP|�bnDo��	�u!�4��|wFm&��w��Ù������p1�2ڞ�,���l��;�݊Ų͎ ��J��$�f����Y���0�vu�����].{�/6ڲ{��o˴U�"�ʉ���":V����鈥o���5���6^�������T��	k�[U�Y�"��VD����A^^DQЙqO����^�nZ]��V�U��@wD���t������G)"d�����:�Jw�(��!�<��9hش(J��\�ql	��>P��&���]����h|,��L�\�k-m:e���*�	(�λ��oi0%�
1�
)����A� ��F{;.�1b�9�\��z4Pˋ�Gt�x�ܗS�Q�����"����hI���"f�0���rD=_��������°Cl��N(q���]�Ճ5����h��!�Du��ؒ�o4�jA��<׳Y��2Z�hٌ���&��{��>44׸�pz�/�=�+�k�*���#�� kL�?��oX����C���O����d}g6Ӻ"��`��<.?�B�����.og��hu~ꕼ0^?7=�\G�mHj�U-ZDߏk��U*|��ؗ�ّ[��ceU�ss���׵3}٩�g]��Ï�;��q?�.ѻ=5���J��$_f��;���s]Z���VHuS��K�����z�XI��i�&�(���ʚ��"��Ե����k˻1���ڭ2�CwX����!�f��{B�9R�p �?�Ѿ��[M��qgf��qG�?k�1ֵ7�F{#w�4�[\�U���6��[�č^q�E���V��mH�ѧM:����T�߮�����ҧ}���\����L��N�)��Q�AJ�T���b{�$jp�F����1�=ێرR*&����+ʧ�jOɞO�>v�,Ա��{@q�qreW#�lNK �fJ�#2���_����A<R�\��͙�d��`�kv�D������������e�O�I_>�.��I�8�V�/p�G�ez�Ik�������=]��!�!�����`����=J����~��?.���O���c~��Pt �1   �-  ��4t=3     PracticalSocket.h����<�~����:�6��6Y�rn�l�(�*H���z�RW�7}�FΦ�o�$���#n˚3DYD��q�~-�������
��.�<��8?đ�ԧ�����{�F�p���tĽ��c���%n
��ooz]^+��K�P�-a N�R����k!�Lf����dnI�@w�<��ą�hI��3����"�QJXp���~�}��HM>NX��d�֐"�����)��}a#���U����hr�W�Ήz;���OGf�%c�
��K?�VoG�\Ld~D��gL��Kr��	�@���qJz^1w�xу��c�����w��^�M��#�������vT�0yJ��Y��GQ��"���\��q7����W`'��s��9��9P��N����7/���kp�m@Z���Y�-
v�
�� 8�ʞh���X����s��(F4������K�{hd�:͞�Ci�|�7g���"](*�S��H�@��~_��E�4Gϻ~\�z<�Ǜ�#���������X��������q�n�Ã�q]��{�-����e l^̟g�
�s�ʢ1q���d�����$W���\���>�v�u��$?A�$a,X�' �-H��U��`�߈�
Ӿ�p̹JEn>�%ir�}�)P� �u��+z�-�2�ⴽR؉,r>����5�
��ĉ2�|�c  5��f�L�*�[C�cr
��8��` J.uLr�Y�M���Xrk��טo�;�y׾�rgp��U�C�ʅ���2��lSu�2�l!��8�Y��#��\(g��{[�U�,���>8�8#��rN���%PDg6?�&Θ��)��K�g.$<x�I�b�V�O��j�Ҡ���h���S��=�������T�7��J5O/�A҈�A��/6�8c� i S��&9���^��*j
�B%Xj�r�i���X��}��12��y��څ�ۅ5�}�df����.sUkK�lH;� N����F��3�|>0z�A*�0�	�?yC�W:�4\E������uE���̚􃘖��̗_�֤Fؓغ�z�9�(��<
��aNNa~ܦ*����u6��$e��T�
���ϼо�ڳ�\{]���hg�CY����/1>�g����ҳ�������k`�Je+�'�qZ!�J��W�m0�D��*�&�w�L�B遻z&�h�ar�;�8T�5�o��z�-C�n��,傣� �������y�Yf�RS�|n�'�ͧgxR�l&���:7�L�P��PL��x��M#iVu�yԍ���8�Y���N�|����9l�k'�D�.{�4�o���l(��Ұ��"�cD�y0�A���3B����4q��g�Z�&.�_��Kߣ�k���F�[�}�ak38c�`��?��9bPN�̽�4g�;���'�WÎ�[��Ο�mn��g��z�cŝNp��j���bרS���AX�xY	�#Ru����gBF}s���"�"����Q�\�q�T�IP;k��&%��J�Q�X�pK�'9ϋ�]�<�L������֣|��C��l��X���7ĵA��Mo�6s�֣)����T{�0a=쩾�_cy�:d���*�]�������(�=�>U���׵4�C�$#��ף��	ҚJ���7�Kv���<��p<���t#�>4Ua S'�w��Ș�^�[�ղT���0p�*h� �Eax#�����+�x(�J	����� r��b��mE)��5���&��T�5�|��WRɾS�krr����ۯ�MS;og�8&Ye�+�"�`�؁�E�aj�ʔ\�/�X;'[�Of�HЎ�����p�m�U��	�.;Z��C+�N �������1S����R�u��K�??*�J�)��~�S[9o'�h%�ID�RT+�`m[���ߕ���&B�4qe)Q^ �&�LČ�L+��Գ �bh��� �����H��ڙ���//K'����`���_t��*����Hd���5f��ʸ�LUĖ�v��%-�T����w@`�9JhQV3I=����r���N{�X��u�N��c�CG�[����X2E޹�_�o2�m��D���\OW�����I���S�����=�.e��f�J
.$��i���w�; �ǠjݡD�Rb+ʄ�w^4]��f��7V�³8�.��2qoL���_�;��Kp��CE �w4���H8ΕMGr{@iJԜ�`�%g�{�_� .���T���&*��:U�'�À�ۜ�uz�/=�t���Fxy4(	��|�Z�^;��ϵ9Y{{���݃[u�`�� ���Fa4�k�Q���O�=n��z
�{`h���s�C�X�|��j�2�,���A�f�����債5�� ��ID/pա�(������/ټk ��p���43������F�Ɗ�`��jm�\�?�z~v5E���d��S�]��x�䄿����#����Q�F��]���G��䍡Ri��-��3��Zr��3����ν��iݨ�<_̞u}�=��q��:@���85���E��"eǟ�V�<j�	��S�-��Űc�SZ�8�1��a�f�Hw�����@���W5�s;~9T���+��ˮ�� 9[�O_z�?�y���'�v>�0dյN�3��Q�3�C�/}�}l!u�X��v����0�i7,>Qͥ�=ԉV~��3]F���g���-*Q�#;��8�m���!U?t�ت�SS]N핮���<|F���<�
|��'��=��.�ˬ��? ��S�?��]t �+   �  ��P���v=3     README �.[ ����{G�G��@��˷o	Ɖ-�yQF��MH��a$����%�M�ܖ��m0�f�oǎf$qR ~�}�]:��v��Du����5�a��E�hjR�ȅ��@uOJf@)�1��Nr�����18��2�QL�;���5$ ´�P�t!���2�?�e��圹uk���BZ�6�>9z���z\yc8��������x���/����$��Ů4X����x<YNG?����<��3L�;�p�m�qL(wZ�Ƈȫ��X�κ�k��c��<CT��}#�G��}�H�B��#�V^��<�	q�G��F����f��i�!F�À�;��Xl��c�)�Zu��a�!��h�7ܹʄL��8�5~ی��j�E�Ϡ�UL���F���KU�[BRg	?J�{]���IVM~5O�~����Ǚ��s�p�"W{�J����	aV?7���-mH�ռ7���FH�/a�v+��B�Ɍ��3���?־�XY��\�T������2g������]B��z�����8?h�={ @                                                                                                                                       makefile                                                                                            0000700 �    Q��   ����00000000674 11472526033 014660  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              # Makefile for our encrypted, cross platform, socketed sieve

goonies:	client 
shiva:	kdc

client:	client.cc blowfish.cc PracticalSocket.cpp client_main.cc
	g++ -o client client.cc blowfish.cc PracticalSocket.cpp client_main.cc 

kdc:	kdc.cc blowfish.cc kdc_main.cc PracticalSocket.cpp
	g++ -o kdc kdc.cc blowfish.cc kdc_main.cc PracticalSocket.cpp -lsocket -lnsl -lpthread

clean:
	rm -f *~
	rm -f *.o
	rm -f kdc
	rm -f client
	rm -f kdc-lin

                                                                    PracticalSocket.cpp                                                                                 0000700 �    Q��   ����00000027761 11472537670 016756  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              /*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"

#ifdef WIN32
  #include <winsock.h>         // For socket(), connect(), send(), and recv()
  typedef int socklen_t;
  typedef char raw_type;       // Type used for raw data on this platform
#else
  #include <sys/types.h>       // For data types
  #include <sys/socket.h>      // For socket(), connect(), send(), and recv()
  #include <netdb.h>           // For gethostbyname()
  #include <arpa/inet.h>       // For inet_addr()
  #include <unistd.h>          // For close()
  #include <netinet/in.h>      // For sockaddr_in
  typedef void raw_type;       // Type used for raw data on this platform
#endif

#include <errno.h>             // For errno

using namespace std;

#ifdef WIN32
static bool initialized = false;
#endif

// SocketException Code

SocketException::SocketException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) {
  if (inclSysMsg) {
    userMessage.append(": ");
    userMessage.append(strerror(errno));
  }
}

SocketException::~SocketException() throw() {
}

const char *SocketException::what() const throw() {
  return userMessage.c_str();
}

// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr) {
  memset(&addr, 0, sizeof(addr));  // Zero out address structure
  addr.sin_family = AF_INET;       // Internet address

  hostent *host;  // Resolve name
  if ((host = gethostbyname(address.c_str())) == NULL) {
    // strerror() will not work for gethostbyname() and hstrerror() 
    // is supposedly obsolete
    throw SocketException("Failed to resolve name (gethostbyname())");
  }
  addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

  addr.sin_port = htons(port);     // Assign port in network byte order
}

// Socket Code

Socket::Socket(int type, int protocol) throw(SocketException) {
  #ifdef WIN32
    if (!initialized) {
      WORD wVersionRequested;
      WSADATA wsaData;

      wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
      if (WSAStartup(wVersionRequested, &wsaData) != 0) {  // Load WinSock DLL
        throw SocketException("Unable to load WinSock DLL");
      }
      initialized = true;
    }
  #endif

  // Make a new socket
  if ((sockDesc = socket(PF_INET, type, protocol)) < 0) {
    throw SocketException("Socket creation failed (socket())", true);
  }
}

Socket::Socket(int sockDesc) {
  this->sockDesc = sockDesc;
}

Socket::~Socket() {
  #ifdef WIN32
    ::closesocket(sockDesc);
  #else
    ::close(sockDesc);
  #endif
  sockDesc = -1;
}

string Socket::getLocalAddress() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of local address failed (getsockname())", true);
  }
  return inet_ntoa(addr.sin_addr);
}

unsigned short Socket::getLocalPort() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of local port failed (getsockname())", true);
  }
  return ntohs(addr.sin_port);
}

void Socket::setLocalPort(unsigned short localPort) throw(SocketException) {
  // Bind the socket to its port
  sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(localPort);

  if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
    throw SocketException("Set of local port failed (bind())", true);
  }
}

void Socket::setLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) throw(SocketException) {
  // Get the address of the requested host
  sockaddr_in localAddr;
  fillAddr(localAddress, localPort, localAddr);

  if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
    throw SocketException("Set of local address and port failed (bind())", true);
  }
}

void Socket::cleanUp() throw(SocketException) {
  #ifdef WIN32
    if (WSACleanup() != 0) {
      throw SocketException("WSACleanup() failed");
    }
  #endif
}

unsigned short Socket::resolveService(const string &service,
                                      const string &protocol) {
  struct servent *serv;        /* Structure containing service information */

  if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
    return atoi(service.c_str());  /* Service is port number */
  else 
    return ntohs(serv->s_port);    /* Found port (network byte order) by name */
}

// CommunicatingSocket Code

CommunicatingSocket::CommunicatingSocket(int type, int protocol)  
    throw(SocketException) : Socket(type, protocol) {
}

CommunicatingSocket::CommunicatingSocket(int newConnSD) : Socket(newConnSD) {
}

void CommunicatingSocket::connect(const string &foreignAddress,
    unsigned short foreignPort) throw(SocketException) {
  // Get the address of the requested host
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Try to connect to the given port
  if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0) {
    throw SocketException("Connect failed (connect())", true);
  }
}

void CommunicatingSocket::send(const void *buffer, int bufferLen) 
    throw(SocketException) {
  if (::send(sockDesc, (raw_type *) buffer, bufferLen, 0) < 0) {
    throw SocketException("Send failed (send())", true);
  }
}

int CommunicatingSocket::recv(void *buffer, int bufferLen) 
    throw(SocketException) {
  int rtn;
  if ((rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, MSG_WAITALL)) < 0) {
    throw SocketException("Received failed (recv())", true);
  }

  return rtn;
}

string CommunicatingSocket::getForeignAddress() 
    throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of foreign address failed (getpeername())", true);
  }
  return inet_ntoa(addr.sin_addr);
}

unsigned short CommunicatingSocket::getForeignPort() throw(SocketException) {
  sockaddr_in addr;
  unsigned int addr_len = sizeof(addr);

  if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
    throw SocketException("Fetch of foreign port failed (getpeername())", true);
  }
  return ntohs(addr.sin_port);
}

// TCPSocket Code

TCPSocket::TCPSocket() 
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, 
    IPPROTO_TCP) {
}

TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort)
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) {
  connect(foreignAddress, foreignPort);
}

TCPSocket::TCPSocket(int newConnSD) : CommunicatingSocket(newConnSD) {
}

// TCPServerSocket Code

TCPServerSocket::TCPServerSocket(unsigned short localPort, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP) {
  setLocalPort(localPort);
  setListen(queueLen);
}

TCPServerSocket::TCPServerSocket(const string &localAddress, 
    unsigned short localPort, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP) {
  setLocalAddressAndPort(localAddress, localPort);
  setListen(queueLen);
}

TCPSocket *TCPServerSocket::accept() throw(SocketException) {
  int newConnSD;
  if ((newConnSD = ::accept(sockDesc, NULL, 0)) < 0) {
    throw SocketException("Accept failed (accept())", true);
  }

  return new TCPSocket(newConnSD);
}

void TCPServerSocket::setListen(int queueLen) throw(SocketException) {
  if (listen(sockDesc, queueLen) < 0) {
    throw SocketException("Set listening socket failed (listen())", true);
  }
}

// UDPSocket Code

UDPSocket::UDPSocket() throw(SocketException) : CommunicatingSocket(SOCK_DGRAM,
    IPPROTO_UDP) {
  setBroadcast();
}

UDPSocket::UDPSocket(unsigned short localPort)  throw(SocketException) : 
    CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setLocalPort(localPort);
  setBroadcast();
}

UDPSocket::UDPSocket(const string &localAddress, unsigned short localPort) 
     throw(SocketException) : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP) {
  setLocalAddressAndPort(localAddress, localPort);
  setBroadcast();
}

void UDPSocket::setBroadcast() {
  // If this fails, we'll hear about it when we try to send.  This will allow 
  // system that cannot broadcast to continue if they don't plan to broadcast
  int broadcastPermission = 1;
  setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST, 
             (raw_type *) &broadcastPermission, sizeof(broadcastPermission));
}

void UDPSocket::disconnect() throw(SocketException) {
  sockaddr_in nullAddr;
  memset(&nullAddr, 0, sizeof(nullAddr));
  nullAddr.sin_family = AF_UNSPEC;

  // Try to disconnect
  if (::connect(sockDesc, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0) {
   #ifdef WIN32
    if (errno != WSAEAFNOSUPPORT) {
   #else
    if (errno != EAFNOSUPPORT) {
   #endif
      throw SocketException("Disconnect failed (connect())", true);
    }
  }
}

void UDPSocket::sendTo(const void *buffer, int bufferLen, 
    const string &foreignAddress, unsigned short foreignPort) 
    throw(SocketException) {
  sockaddr_in destAddr;
  fillAddr(foreignAddress, foreignPort, destAddr);

  // Write out the whole buffer as a single message.
  if (sendto(sockDesc, (raw_type *) buffer, bufferLen, 0,
             (sockaddr *) &destAddr, sizeof(destAddr)) != bufferLen) {
    throw SocketException("Send failed (sendto())", true);
  }
}

int UDPSocket::recvFrom(void *buffer, int bufferLen, string &sourceAddress,
    unsigned short &sourcePort) throw(SocketException) {
  sockaddr_in clntAddr;
  socklen_t addrLen = sizeof(clntAddr);
  int rtn;
  if ((rtn = recvfrom(sockDesc, (raw_type *) buffer, bufferLen, 0, 
                      (sockaddr *) &clntAddr, (socklen_t *) &addrLen)) < 0) {
    throw SocketException("Receive failed (recvfrom())", true);
  }
  sourceAddress = inet_ntoa(clntAddr.sin_addr);
  sourcePort = ntohs(clntAddr.sin_port);

  return rtn;
}

void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(SocketException) {
  if (setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_TTL, 
                 (raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0) {
    throw SocketException("Multicast TTL set failed (setsockopt())", true);
  }
}

void UDPSocket::joinGroup(const string &multicastGroup) throw(SocketException) {
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                 (raw_type *) &multicastRequest, 
                 sizeof(multicastRequest)) < 0) {
    throw SocketException("Multicast group join failed (setsockopt())", true);
  }
}

void UDPSocket::leaveGroup(const string &multicastGroup) throw(SocketException) {
  struct ip_mreq multicastRequest;

  multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
                 (raw_type *) &multicastRequest, 
                 sizeof(multicastRequest)) < 0) {
    throw SocketException("Multicast group leave failed (setsockopt())", true);
  }
}
               PracticalSocket.h                                                                                   0000700 �    Q��   ����00000026753 11471676624 016425  0                                                                                                    ustar   vanzeeaj                        nfsnobody                                                                                                                                                                                                              /*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PRACTICALSOCKET_INCLUDED__
#define __PRACTICALSOCKET_INCLUDED__

#include <string>            // For string
#include <exception>         // For exception class

using namespace std;

/**
 *   Signals a problem with the execution of a socket call.
 */
class SocketException : public exception {
public:
  /**
   *   Construct a SocketException with a explanatory message.
   *   @param message explanatory message
   *   @param incSysMsg true if system message (from strerror(errno))
   *   should be postfixed to the user provided message
   */
  SocketException(const string &message, bool inclSysMsg = false) throw();

  /**
   *   Provided just to guarantee that no exceptions are thrown.
   */
  ~SocketException() throw();

  /**
   *   Get the exception message
   *   @return exception message
   */
  const char *what() const throw();

private:
  string userMessage;  // Exception message
};

/**
 *   Base class representing basic communication endpoint
 */
class Socket {
public:
  /**
   *   Close and deallocate this socket
   */
  ~Socket();

  /**
   *   Get the local address
   *   @return local address of socket
   *   @exception SocketException thrown if fetch fails
   */
  string getLocalAddress() throw(SocketException);

  /**
   *   Get the local port
   *   @return local port of socket
   *   @exception SocketException thrown if fetch fails
   */
  unsigned short getLocalPort() throw(SocketException);

  /**
   *   Set the local port to the specified port and the local address
   *   to any interface
   *   @param localPort local port
   *   @exception SocketException thrown if setting local port fails
   */
  void setLocalPort(unsigned short localPort) throw(SocketException);

  /**
   *   Set the local port to the specified port and the local address
   *   to the specified address.  If you omit the port, a random port 
   *   will be selected.
   *   @param localAddress local address
   *   @param localPort local port
   *   @exception SocketException thrown if setting local port or address fails
   */
  void setLocalAddressAndPort(const string &localAddress, 
    unsigned short localPort = 0) throw(SocketException);

  /**
   *   If WinSock, unload the WinSock DLLs; otherwise do nothing.  We ignore
   *   this in our sample client code but include it in the library for
   *   completeness.  If you are running on Windows and you are concerned
   *   about DLL resource consumption, call this after you are done with all
   *   Socket instances.  If you execute this on Windows while some instance of
   *   Socket exists, you are toast.  For portability of client code, this is 
   *   an empty function on non-Windows platforms so you can always include it.
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF, and -1 for error
   *   @exception SocketException thrown WinSock clean up fails
   */
  static void cleanUp() throw(SocketException);

  /**
   *   Resolve the specified service for the specified protocol to the
   *   corresponding port number in host byte order
   *   @param service service to resolve (e.g., "http")
   *   @param protocol protocol of service to resolve.  Default is "tcp".
   */
  static unsigned short resolveService(const string &service,
                                       const string &protocol = "tcp");

private:
  // Prevent the user from trying to use value semantics on this object
  Socket(const Socket &sock);
  void operator=(const Socket &sock);

protected:
  int sockDesc;              // Socket descriptor
  Socket(int type, int protocol) throw(SocketException);
  Socket(int sockDesc);
};

/**
 *   Socket which is able to connect, send, and receive
 */
class CommunicatingSocket : public Socket {
public:
  /**
   *   Establish a socket connection with the given foreign
   *   address and port
   *   @param foreignAddress foreign address (IP address or name)
   *   @param foreignPort foreign port
   *   @exception SocketException thrown if unable to establish connection
   */
  void connect(const string &foreignAddress, unsigned short foreignPort)
    throw(SocketException);

  /**
   *   Write the given buffer to this socket.  Call connect() before
   *   calling send()
   *   @param buffer buffer to be written
   *   @param bufferLen number of bytes from buffer to be written
   *   @exception SocketException thrown if unable to send data
   */
  void send(const void *buffer, int bufferLen) throw(SocketException);

  /**
   *   Read into the given buffer up to bufferLen bytes data from this
   *   socket.  Call connect() before calling recv()
   *   @param buffer buffer to receive the data
   *   @param bufferLen maximum number of bytes to read into buffer
   *   @return number of bytes read, 0 for EOF, and -1 for error
   *   @exception SocketException thrown if unable to receive data
   */
  int recv(void *buffer, int bufferLen) throw(SocketException);

  /**
   *   Get the foreign address.  Call connect() before calling recv()
   *   @return foreign address
   *   @exception SocketException thrown if unable to fetch foreign address
   */
  string getForeignAddress() throw(SocketException);

  /**
   *   Get the foreign port.  Call connect() before calling recv()
   *   @return foreign port
   *   @exception SocketException thrown if unable to fetch foreign port
   */
  unsigned short getForeignPort() throw(SocketException);

protected:
  CommunicatingSocket(int type, int protocol) throw(SocketException);
  CommunicatingSocket(int newConnSD);
};

/**
 *   TCP socket for communication with other TCP sockets
 */
class TCPSocket : public CommunicatingSocket {
public:
  /**
   *   Construct a TCP socket with no connection
   *   @exception SocketException thrown if unable to create TCP socket
   */
  TCPSocket() throw(SocketException);

  /**
   *   Construct a TCP socket with a connection to the given foreign address
   *   and port
   *   @param foreignAddress foreign address (IP address or name)
   *   @param foreignPort foreign port
   *   @exception SocketException thrown if unable to create TCP socket
   */
  TCPSocket(const string &foreignAddress, unsigned short foreignPort) 
      throw(SocketException);

private:
  // Access for TCPServerSocket::accept() connection creation
  friend class TCPServerSocket;
  TCPSocket(int newConnSD);
};

/**
 *   TCP socket class for servers
 */
class TCPServerSocket : public Socket {
public:
  /**
   *   Construct a TCP socket for use with a server, accepting connections
   *   on the specified port on any interface
   *   @param localPort local port of server socket, a value of zero will
   *                   give a system-assigned unused port
   *   @param queueLen maximum queue length for outstanding 
   *                   connection requests (default 5)
   *   @exception SocketException thrown if unable to create TCP server socket
   */
  TCPServerSocket(unsigned short localPort, int queueLen = 5) 
      throw(SocketException);

  /**
   *   Construct a TCP socket for use with a server, accepting connections
   *   on the specified port on the interface specified by the given address
   *   @param localAddress local interface (address) of server socket
   *   @param localPort local port of server socket
   *   @param queueLen maximum queue length for outstanding 
   *                   connection requests (default 5)
   *   @exception SocketException thrown if unable to create TCP server socket
   */
  TCPServerSocket(const string &localAddress, unsigned short localPort,
      int queueLen = 5) throw(SocketException);

  /**
   *   Blocks until a new connection is established on this socket or error
   *   @return new connection socket
   *   @exception SocketException thrown if attempt to accept a new connection fails
   */
  TCPSocket *accept() throw(SocketException);

private:
  void setListen(int queueLen) throw(SocketException);
};

/**
  *   UDP socket class
  */
class UDPSocket : public CommunicatingSocket {
public:
  /**
   *   Construct a UDP socket
   *   @exception SocketException thrown if unable to create UDP socket
   */
  UDPSocket() throw(SocketException);

  /**
   *   Construct a UDP socket with the given local port
   *   @param localPort local port
   *   @exception SocketException thrown if unable to create UDP socket
   */
  UDPSocket(unsigned short localPort) throw(SocketException);

  /**
   *   Construct a UDP socket with the given local port and address
   *   @param localAddress local address
   *   @param localPort local port
   *   @exception SocketException thrown if unable to create UDP socket
   */
  UDPSocket(const string &localAddress, unsigned short localPort) 
      throw(SocketException);

  /**
   *   Unset foreign address and port
   *   @return true if disassociation is successful
   *   @exception SocketException thrown if unable to disconnect UDP socket
   */
  void disconnect() throw(SocketException);

  /**
   *   Send the given buffer as a UDP datagram to the
   *   specified address/port
   *   @param buffer buffer to be written
   *   @param bufferLen number of bytes to write
   *   @param foreignAddress address (IP address or name) to send to
   *   @param foreignPort port number to send to
   *   @return true if send is successful
   *   @exception SocketException thrown if unable to send datagram
   */
  void sendTo(const void *buffer, int bufferLen, const string &foreignAddress,
            unsigned short foreignPort) throw(SocketException);

  /**
   *   Read read up to bufferLen bytes data from this socket.  The given buffer
   *   is where the data will be placed
   *   @param buffer buffer to receive data
   *   @param bufferLen maximum number of bytes to receive
   *   @param sourceAddress address of datagram source
   *   @param sourcePort port of data source
   *   @return number of bytes received and -1 for error
   *   @exception SocketException thrown if unable to receive datagram
   */
  int recvFrom(void *buffer, int bufferLen, string &sourceAddress, 
               unsigned short &sourcePort) throw(SocketException);

  /**
   *   Set the multicast TTL
   *   @param multicastTTL multicast TTL
   *   @exception SocketException thrown if unable to set TTL
   */
  void setMulticastTTL(unsigned char multicastTTL) throw(SocketException);

  /**
   *   Join the specified multicast group
   *   @param multicastGroup multicast group address to join
   *   @exception SocketException thrown if unable to join group
   */
  void joinGroup(const string &multicastGroup) throw(SocketException);

  /**
   *   Leave the specified multicast group
   *   @param multicastGroup multicast group address to leave
   *   @exception SocketException thrown if unable to leave group
   */
  void leaveGroup(const string &multicastGroup) throw(SocketException);

private:
  void setBroadcast();
};

#endif
                     README                                                                                              0000644 �    Q�0303237 00000001615 11472637554 012242  0                                                                                                    ustar   vanzeeaj                        SFU_Users                                                                                                                                                                                                              LAB3	-	Andy Van Zeeland && Greg Boettcher

To compile on andy or clark, run the command: make.
To compile on shiva, run the command: make shiva.

To run a client:  ./client 'size' 'nonce' 'keyA' 'initiator?'
To run a server:  ./kdc 'sessionKey' 'keyA' 'keyB'

The KDC is hard coded to run on shiva.  The clients are hard coded to run on andy and clark.
All sieve traffic sent between the two clients is encrypted.  The actual authentication process 
with the KDC is not, due to technicalities with encrypting/decrypting little to big endian
and vice versa.  If we had more time (and no midterm on the same day), I'm certain we could 
have gotten it working and portable.  Andy spent around 50-60 hours total over the 2 week period
working on this project, to give you some perspective as to how much time was spent on it.

Runtimes:
primes up to 1,000,000:		10.1 secs
primes up to 10,000,000:	4 mins 53 secs

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   