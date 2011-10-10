#define byte0(a)	a >> 24
#define byte1(a)	(a & 0xFF0000) >> 16
#define byte2(a)	(a & 0xFF00) >> 8
#define byte3(a)	(a & 0xFF)

#define byte0_2_word(b, a) a = (a & 0xFFFFFF) | b << 24
#define byte1_2_word(b, a) a = (a & 0xFF00FFFF) | b << 16
#define byte2_2_word(b, a) a = (a & 0xFFFF00FF) | b << 8
#define byte3_2_word(b, a) a = (a & 0xFFFFFF00) | b

#define bytes2word(b0, b1, b2, b3, a) a = (b0 << 24) | (b1 << 16) | (b2 << 8) | (b3)

void
switchblock(unsigned char *block)
{
	int i;
	unsigned char aux[16*sizeof(char)];
	//aux = (unsigned char *)malloc(16*sizeof(char));
 
	*(aux) = *(block);
	*(aux + 1) = *(block + 4);
	*(aux + 2) = *(block + 8);
	*(aux + 3) = *(block + 12);
	*(aux + 4) = *(block + 1);
	*(aux + 5) = *(block + 5);
	*(aux + 6) = *(block + 9);
	*(aux + 7) = *(block + 13);
	*(aux + 8) = *(block + 2);
	*(aux + 9) = *(block + 6);
	*(aux + 10) = *(block + 10);
	*(aux + 11) = *(block + 14);
	*(aux + 12) = *(block + 3);
	*(aux + 13) = *(block + 7);
	*(aux + 14) = *(block + 11);
	*(aux + 15) = *(block + 15);
 
	for(i = 0; i < 16; i++)
		*(block + i) = *(aux + i);		
 
}
 
/*----------------------------------------------------------------------------*/
static const unsigned char SenK[] = {
    99, 124, 119, 123, 242, 107, 111, 197,
    48,   1, 103,  43, 254, 215, 171, 118,
    202, 130, 201, 125, 250,  89,  71, 240,
    173, 212, 162, 175, 156, 164, 114, 192,
    183, 253, 147,  38,  54,  63, 247, 204,
    52, 165, 229, 241, 113, 216,  49,  21,
    4, 199,  35, 195,  24, 150,   5, 154,
    7,  18, 128, 226, 235,  39, 178, 117,
    9, 131,  44,  26,  27, 110,  90, 160,
    82,  59, 214, 179,  41, 227,  47, 132,
    83, 209,   0, 237,  32, 252, 177,  91,
    106, 203, 190,  57,  74,  76,  88, 207,
    208, 239, 170, 251,  67,  77,  51, 133,
    69, 249,   2, 127,  80,  60, 159, 168,
    81, 163,  64, 143, 146, 157,  56, 245,
    188, 182, 218,  33,  16, 255, 243, 210,
    205,  12,  19, 236,  95, 151,  68,  23,
    196, 167, 126,  61, 100,  93,  25, 115,
    96, 129,  79, 220,  34,  42, 144, 136,
    70, 238, 184,  20, 222,  94,  11, 219,
    224,  50,  58,  10,  73,   6,  36,  92,
    194, 211, 172,  98, 145, 149, 228, 121,
    231, 200,  55, 109, 141, 213,  78, 169,
    108,  86, 244, 234, 101, 122, 174,   8,
    186, 120,  37,  46,  28, 166, 180, 198,
    232, 221, 116,  31,  75, 189, 139, 138,
    112,  62, 181, 102,  72,   3, 246,  14,
    97,  53,  87, 185, 134, 193,  29, 158,  
    225, 248, 152,  17, 105, 217, 142, 148, 
    155,  30, 135, 233, 206,  85,  40, 223, 
    140, 161, 137,  13, 191, 230,  66, 104, 
    65, 153,  45,  15, 176,  84, 187,  22   
};

/*----------------------------------------------------------------------------*/

void
RotWord(unsigned char *a)
{
unsigned char aux;
aux = *(a + 0);
*(a + 0) = *(a + 1);
*(a + 1) = *(a + 2);
*(a + 2) = *(a + 3);
*(a + 3) = aux;
}

/*----------------------------------------------------------------------------*/

void
KeySchedule(unsigned char *key, unsigned char *rcon)
{
unsigned char aux1[4];
unsigned char aux2[4];
unsigned char aux3[4];
int i = 0;
for(i = 0; i < 4; i++)
    {
    aux1[i] = *(key + i*4 + 3);
    aux2[i] = *(key + i*4);
    }
RotWord((unsigned char *)aux1);
 
//SubBytes
for(i = 0; i < 4; i++)
    {
    aux1[i] = SenK[aux1[i]];
    }
for(i = 0; i < 4; i++)
    {
    aux3[i] = aux2[i] ^ aux1[i] ^ *(rcon + i);
    *(key + i*4) = aux3[i];
    }
 
for(i = 0; i < 4; i++)
    {
    aux3[i] = *(key + i*4 + 1) ^ aux3[i];
    *(key + i*4 + 1) = aux3[i];
    }
 
for(i = 0; i < 4; i++)
    {
    aux3[i] = *(key + i*4 + 2) ^ aux3[i];
    *(key + i*4 + 2) = aux3[i];
    }
 
for(i = 0; i < 4; i++)
    {
    aux3[i] = *(key + i*4 + 3) ^ aux3[i];
    *(key + i*4 + 3) = aux3[i];
    }
 
}

/*----------------------------------------------------------------------------*/

void
KeyExpand2(unsigned char *key, unsigned char *expandedkey)
{
int i = 0;
int j = 0;
 
unsigned char aux[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
unsigned char rcon[4] = {0x00, 0x00, 0x00, 0x00};

switchblock(key);
 
for(i = 0; i < 16; i++)
    *(expandedkey + i) = *(key + i);
 
for(i = 0; i < 10; i++)
    {
    rcon[0] = aux[i];
    KeySchedule((unsigned char *)(key), (unsigned char *)rcon);
    for(j = 0; j < 16; j++)
        *(expandedkey + 16*(i + 1) + j) = *(key + j);
    }
}

void
Key2Int(unsigned char *expandedkey, unsigned int *expandedkeyInt)
{
int i = 0;

for(i = 0; i < 11; i++)
{
	bytes2word((unsigned int)expandedkey[i*16],(unsigned int)expandedkey[i*16+1],(unsigned int)expandedkey[i*16+2],(unsigned int)expandedkey[i*16+3],expandedkeyInt[i*4]);
	bytes2word((unsigned int)expandedkey[i*16+4],(unsigned int)expandedkey[i*16+5],(unsigned int)expandedkey[i*16+6],(unsigned int)expandedkey[i*16+7],expandedkeyInt[i*4+1]);
	bytes2word((unsigned int)expandedkey[i*16+8],(unsigned int)expandedkey[i*16+9],(unsigned int)expandedkey[i*16+10],(unsigned int)expandedkey[i*16+11],expandedkeyInt[i*4+2]);
	bytes2word((unsigned int)expandedkey[i*16+12],(unsigned int)expandedkey[i*16+13],(unsigned int)expandedkey[i*16+14],(unsigned int)expandedkey[i*16+15],expandedkeyInt[i*4+3]);
}

}



