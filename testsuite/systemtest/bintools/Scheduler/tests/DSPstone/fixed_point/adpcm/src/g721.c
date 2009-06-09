/*
 * g721.c
 * ADPCM transcoder applying the CCITT recommendation G.721
 *
 * It has been successfully tested with all 32 kbits reset test sequences.
 * The tests were made on a SPARCstation10 with various compilers.
 *
 * This source code is released under a DISCLAIMER OF WARRANTY and a
 * Copyright (c) 1994 by the
 *
 * Institute for Integrated Systems in Signal Processing, IS2
 * DSP Tools Group
 * Aachen University of Technology
 *
 * IT MAY BE FREELY DUPLICATED BUT ANY COMMERCIAL USE WITHOUT PRIOR WRITTEN
 * PERMIT OF THE IS2 IS ILLEGAL!
 *
 * For further documentation of the source consult the recommendation.
 *
 * The implementation was done by Chris Schlaeger. Thanks to Juan Martinez
 * Velarde for his ideas and help during the optimization.
 * Last modification 25-Apr-94
 */

#include "g721.h"

#define FALSE 0
#define TRUE (!FALSE)

typedef struct
{
  U16BIT B[8], DQ[8], PK1, PK2;  /* A1 = B[6], A2 = B[7] */
  U16BIT AP, DMS, DML;           /* SR1 = DQ[6], SR2 = DQ[7] */
  U16BIT YU;
  U16BIT TD;
  U16BIT YL6;  /* YL >> 6 */
  U24BIT YL;
} STATES;

/*
 * Prototypes
 */

static void adapt_quant(void);
static void adpt_predict_1(void);
static void adpt_predict_2(void);
static void coding_adjustment(void);
static void diff_computation(void);
static void iadpt_quant(void);
static void input_conversion(void);
static void output_conversion(void);
static void reset_states(void);
static void scale_factor_1(void);
static void scale_factor_2(void);
static void speed_control_1(void);
static void speed_control_2(void);
static void tone_detector_1(void);
static void tone_detector_2(void);

/* global signals */
static U16BIT AL, A2P, D, DQ, I, SD, SE, SEZ, S, SL, SR, TDP, TR, Y;
static STATES* X;

/* ENCODER states */
static STATES E_STATES;

/* DECODER states */
static STATES D_STATES;

static U16BIT A_LAW_table[] =
{
   4784,  4752,  4848,  4816,  4656,  4624,  4720,  4688, 
   5040,  5008,  5104,  5072,  4912,  4880,  4976,  4944, 
   4440,  4424,  4472,  4456,  4376,  4360,  4408,  4392, 
   4568,  4552,  4600,  4584,  4504,  4488,  4536,  4520, 
   6848,  6720,  7104,  6976,  6336,  6208,  6592,  6464, 
   7872,  7744,  8128,  8000,  7360,  7232,  7616,  7488, 
   5472,  5408,  5600,  5536,  5216,  5152,  5344,  5280, 
   5984,  5920,  6112,  6048,  5728,  5664,  5856,  5792, 
   4139,  4137,  4143,  4141,  4131,  4129,  4135,  4133, 
   4155,  4153,  4159,  4157,  4147,  4145,  4151,  4149, 
   4107,  4105,  4111,  4109,  4099,  4097,  4103,  4101, 
   4123,  4121,  4127,  4125,  4115,  4113,  4119,  4117, 
   4268,  4260,  4284,  4276,  4236,  4228,  4252,  4244, 
   4332,  4324,  4348,  4340,  4300,  4292,  4316,  4308, 
   4182,  4178,  4190,  4186,  4166,  4162,  4174,  4170, 
   4214,  4210,  4222,  4218,  4198,  4194,  4206,  4202, 
    688,   656,   752,   720,   560,   528,   624,   592, 
    944,   912,  1008,   976,   816,   784,   880,   848, 
    344,   328,   376,   360,   280,   264,   312,   296, 
    472,   456,   504,   488,   408,   392,   440,   424, 
   2752,  2624,  3008,  2880,  2240,  2112,  2496,  2368, 
   3776,  3648,  4032,  3904,  3264,  3136,  3520,  3392, 
   1376,  1312,  1504,  1440,  1120,  1056,  1248,  1184, 
   1888,  1824,  2016,  1952,  1632,  1568,  1760,  1696, 
     43,    41,    47,    45,    35,    33,    39,    37, 
     59,    57,    63,    61,    51,    49,    55,    53, 
     11,     9,    15,    13,     3,     1,     7,     5, 
     27,    25,    31,    29,    19,    17,    23,    21, 
    172,   164,   188,   180,   140,   132,   156,   148, 
    236,   228,   252,   244,   204,   196,   220,   212, 
     86,    82,    94,    90,    70,    66,    78,    74, 
    118,   114,   126,   122,   102,    98,   110,   106
} ;

static U16BIT u_LAW_table[] =
{
  16223, 15967, 15711, 15455, 15199, 14943, 14687, 14431, 
  14175, 13919, 13663, 13407, 13151, 12895, 12639, 12383, 
  12191, 12063, 11935, 11807, 11679, 11551, 11423, 11295, 
  11167, 11039, 10911, 10783, 10655, 10527, 10399, 10271, 
  10175, 10111, 10047,  9983,  9919,  9855,  9791,  9727, 
   9663,  9599,  9535,  9471,  9407,  9343,  9279,  9215, 
   9167,  9135,  9103,  9071,  9039,  9007,  8975,  8943, 
   8911,  8879,  8847,  8815,  8783,  8751,  8719,  8687, 
   8663,  8647,  8631,  8615,  8599,  8583,  8567,  8551, 
   8535,  8519,  8503,  8487,  8471,  8455,  8439,  8423, 
   8411,  8403,  8395,  8387,  8379,  8371,  8363,  8355, 
   8347,  8339,  8331,  8323,  8315,  8307,  8299,  8291, 
   8285,  8281,  8277,  8273,  8269,  8265,  8261,  8257, 
   8253,  8249,  8245,  8241,  8237,  8233,  8229,  8225, 
   8222,  8220,  8218,  8216,  8214,  8212,  8210,  8208, 
   8206,  8204,  8202,  8200,  8198,  8196,  8194,     0, 
   8031,  7775,  7519,  7263,  7007,  6751,  6495,  6239, 
   5983,  5727,  5471,  5215,  4959,  4703,  4447,  4191, 
   3999,  3871,  3743,  3615,  3487,  3359,  3231,  3103, 
   2975,  2847,  2719,  2591,  2463,  2335,  2207,  2079, 
   1983,  1919,  1855,  1791,  1727,  1663,  1599,  1535, 
   1471,  1407,  1343,  1279,  1215,  1151,  1087,  1023, 
    975,   943,   911,   879,   847,   815,   783,   751, 
    719,   687,   655,   623,   591,   559,   527,   495, 
    471,   455,   439,   423,   407,   391,   375,   359, 
    343,   327,   311,   295,   279,   263,   247,   231, 
    219,   211,   203,   195,   187,   179,   171,   163, 
    155,   147,   139,   131,   123,   115,   107,    99, 
     93,    89,    85,    81,    77,    73,    69,    65, 
     61,    57,    53,    49,    45,    41,    37,    33, 
     30,    28,    26,    24,    22,    20,    18,    16, 
     14,    12,    10,     8,     6,     4,     2,     0
} ;

/*
 * "inline Functions"
 */
#define LSHIFT(a, b)   ((b) < 0 ? (a) << -(b) : (a) >> (b))
#define SIGNBIT(a, b)  ((a) & (((U16BIT) 1) << (b)) ? 1 : 0)
#define NEGATIVE(a, b) ((a) & (((U16BIT) 1) << (b)))
#define MSB0(a, b)     { register U16BIT tmp = (U16BIT) (a); \
                        (b) = 0; while(tmp) {tmp >>= 1; (b)++; }}
#define MSB1(a, b)     { register U16BIT tmp = (U16BIT) ((a) >> 1); \
                         (b) = 0; while(tmp) {tmp >>= 1; (b)++; }}

static void adapt_quant()
{
  /*
   * adaptive quantizer
   *
   * Input signals:  D, Y
   * Output signals: I
   */
  register U16BIT DLN, DS;
  U16BIT          DL;

  /* LOG */
  {
    U16BIT DQM;
    S16BIT EXP;

    DS = NEGATIVE(D, 15);
    DQM = IF_ELSE(DS, (-D) & 32767, D);
    MSB1(DQM, EXP);
    DL = (EXP << 7) + ((LSHIFT(DQM, EXP - 7)) & 127);
  }
    	
  /* SUBTB */
  DLN = (DL - (Y >> 2)) & 4095;
  
  /* QUAN */
  if (DLN > 299)
    if (DLN > 2047)
      if (DLN > 3971)
        I = DS ? 0xE : 0x1;
      else
        I = 0xF;
    else
      if (DLN > 399)
        I = DS ? 0x8 : 0x7;
      else
	if (DLN > 348)
          I = DS ? 0x9 : 0x6;
        else
	  I = DS ? 0xA : 0x5;
  else
    if (DLN > 177)
      if (DLN > 245)
        I = DS ? 0xB : 0x4;
      else
        I = DS ? 0xC : 0x3;
    else
      if (DLN > 79)
        I = DS ? 0xD : 0x2;
      else
        I = DS ? 0xE : 0x1;
}

static void adpt_predict_1(void)
{
  /*
   * adptive predictor
   *
   * Input signals:  none
   * Output signals: SE, SEZ
   * States:         B[], DQ[], B[6], B[6]
   */

  register STATES* x = X;
  register int    i;
  register U16BIT SEZI;

  /* FMULT and ACCUM */
  SEZI = 0;
  {
    register U16BIT* tmp1 = x->B;
    register U16BIT* tmp2 = x->DQ;

    for (i = 0; i < 8; i++)
    {
      {
	register S16BIT AnEXP, WAnEXP;
	register U16BIT AnMAG;
	U16BIT AnS, AnMANT, WAnMAG, WAnMANT;
	
	/* FMULT */
	AnMAG = (AnS = SIGNBIT(*tmp1, 15)) ?
	         (-(*tmp1++ >> 2)) & 8191 : *tmp1++ >> 2;
	MSB0(AnMAG, AnEXP);
	AnMANT = AnMAG ? LSHIFT(AnMAG, AnEXP - 6) : 1 << 5;
	WAnMANT = (((*tmp2 & 63) * AnMANT) + 48) >> 4;
	WAnEXP = ((*tmp2 >> 6) & 15) + AnEXP;
	WAnMAG = LSHIFT((U24BIT) WAnMANT, 19 - WAnEXP) & 32767;
	SEZI += IF_ELSE(SIGNBIT(*tmp2++, 10) ^ AnS, -WAnMAG, WAnMAG);
      }
      if (i == 5)
	SEZ = (SEZI & (U16BIT) 65535L) >> 1;
    }
  }
  SE = (SEZI & ((U16BIT) 65535L)) >> 1;
}

static void adpt_predict_2(void)
{
  /*
   * adaptive predictor
   *
   * Input signals:  DQ, SE, SEZ, TR
   * Output signals: A2P
   * States:         B[], DQ[], PK1, PK2
   */

  register STATES* x = X;
  U16BIT *a1 = &x->B[6];
  U16BIT *a2 = a1 + 1;
  U16BIT A1T, A2T, DQSEZ, PK0, SR0;


  /* ADDC */
  {
    DQSEZ = (IF_ELSE(NEGATIVE(DQ, 14), -(DQ & 16383), DQ)
              + IF_ELSE(NEGATIVE(SEZ, 14), (((U16BIT) 1) << 15) + SEZ, SEZ))
            & ((U16BIT) 65535L);
    PK0 = SIGNBIT(DQSEZ, 15);
  }

  /* ADDB */
  SR = (IF_ELSE(NEGATIVE(DQ, 14), -(DQ & 16383), DQ)
	+ IF_ELSE(NEGATIVE(SE, 14), (((U16BIT) 1) << 15) + SE, SE))
       & ((U16BIT) 65535L);
  
  /* FLOATB */
  {
    register S16BIT EXP;
    U16BIT SRS, MAG;

    SRS = NEGATIVE(SR, 15);
    MAG = IF_ELSE(SRS, (-SR) & 32767, SR);
    MSB0(MAG, EXP);
    SR0 = (SRS ? 1 << 10 : 0) + (EXP << 6) +
          (MAG ? LSHIFT(MAG, EXP - 6) : 1 << 5);
  }

  /* UPA2 */
  {
    register U16BIT PKS1, UGA2B, UGA2, ULA2;
    U24BIT FA1;

    PKS1 = PK0 ^ x->PK1;
    if (NEGATIVE(*a1, 15))
      FA1 = *a1 >= (U16BIT) 57345L ?
                     (((U24BIT) *a1) << 2) & ((U24BIT) 131071L) :
                     ((U24BIT) 24577) << 2;
    else
      FA1 = *a1 <= 8191 ? *a1 << 2 : 8191 << 2;
    UGA2B = ((U16BIT) (((PK0 ^ x->PK2 ? (U24BIT) 114688L : (U24BIT) 16384)
                       + IF_ELSE(PKS1, FA1, (-FA1) & ((U24BIT) 131071L)))
                       >> 7)) & 1023;
    UGA2 = IF_ELSE(DQSEZ, IF_ELSE(NEGATIVE(UGA2B, 9),
				  UGA2B + ((U16BIT) 64512L), UGA2B), 0);
    ULA2 = NEGATIVE(*a2, 15) ? -((*a2 >> 7) + ((U16BIT) 65024L)) :
                               -(*a2 >> 7);
    A2T = (*a2 + UGA2 + ULA2) & ((U16BIT) 65535L);
  }

  if (TR)
  {
    register int i;

    for (i = 0; i < 8; i++)
      x->B[i] = 0;
  }
  else
  {
    /* LIMC */
    *a2 = A2P = IF_ELSE(A2T <= 32767, IF_ELSE(A2T >= 12288, 12288, A2T),
			IF_ELSE(A2T <= (U16BIT) 53248L,
			(U16BIT) 53248L, A2T));
    
    /* UPA1 */
    {
      U16BIT PKS;

      PKS = PK0 ^ x->PK1;
      A1T = (*a1 + IF_ELSE(DQSEZ, IF_ELSE(PKS, (U16BIT) 65344L, 192), 0) +
	     (NEGATIVE(*a1, 15) ? -((*a1 >> 8) + ((U16BIT) 65280L)) :
	      -(*a1 >> 8))) & ((U16BIT) 65535L);
    }

    /* LIMD */
    {
      register U16BIT A1UL, A1LL;
      
      A1UL = (U16BIT) ((((U24BIT) 80896L) - *a2) & ((U16BIT) 65535L));
      A1LL = (*a2 + ((U16BIT) 50176L)) & ((U16BIT) 65535L);
      *a1 = IF_ELSE(A1T <= 32767,
		      IF_ELSE(A1T >= A1UL, A1UL, A1T),
		      IF_ELSE(A1T <= A1LL, A1LL, A1T));
    }

    /* UPB, XOR and TRIGB */
    {
      register int i;
      register U16BIT* tmp1 = x->B;
      U16BIT* tmp2 = x->DQ;
      U16BIT DQS, DQM;
      
      DQS = SIGNBIT(DQ, 14);
      DQM = DQ & 16383;
      for (i = 0; i < 6; i++, tmp1++)
	*tmp1 = (*tmp1 +
	         IF_ELSE(DQM,
	                 IF_ELSE(DQS ^ SIGNBIT(*tmp2++, 10),
		                 (U16BIT) 65408L, 128), 0) +
	         (NEGATIVE(*tmp1 ,15) ?
		 -((*tmp1 >> 8) + ((U16BIT) 65280L)) : -(*tmp1 >> 8)))
	        & ((U16BIT) 65535L);
    }
  }

  /* FLOATA */
  {
    register int i = 0;
    U16BIT MAG;
    S16BIT EXP;

    {
      register U16BIT* tmp = &x->DQ[5];

      for (i = 0; i < 5; i++)
	*tmp-- = *(tmp - 1);
    }

    MAG = DQ & 16383;
    MSB0(MAG, EXP);
    x->DQ[0] = (NEGATIVE(DQ, 14)? 1 << 10 : 0) + (EXP << 6) +
               (MAG ? LSHIFT(MAG, EXP - 6) : 1 << 5);
  }

  x->PK2 = x->PK1;
  x->PK1 = PK0;
  x->DQ[7] = x->DQ[6];
  x->DQ[6] = SR0;
}

static void coding_adjustment()
{
  /*
   * synchronous coding adjustment
   *
   * Input signals:  D, SP, Y, LAW, I
   * Output signals: SD
   */

  register U16BIT DLN, DS;

  /* LOG and SUBTB*/
  {
    register S16BIT EXP;
    U16BIT          DQM;

    DS = NEGATIVE(D, 15);
    DQM = IF_ELSE(DS, (-D) & 32767, D);
    MSB1(DQM, EXP);
    DLN = (((EXP << 7) + (LSHIFT(DQM, EXP - 7) & 127)) - (Y >> 2)) & 4095;
  }
    	
  /* SYNC */
  {
    U16BIT IM, ID;

    IM = NEGATIVE(I, 3) ? I & 7 : I + 8;
    if (DLN > 299)
      if (DLN > 2047)
	if (DLN > 3971)
	  ID = DS ? 6 : 9;
	else
	  ID = 7;
      else
	if (DLN > 399)
	  ID = DS ? 0 : 15;
	else
	  if (DLN > 348)
	    ID = DS ? 1 : 14;
	  else
	    ID = DS ? 2 : 13;
    else
      if (DLN > 177)
	if (DLN > 245)
	  ID = DS ? 3 : 12;
	else
	  ID = DS ? 4 : 11;
      else
	if (DLN > 79)
	  ID = DS ? 5 : 10;
	else
	  ID = DS ? 6 : 9;
    
    if (LAW)
    {
      SD = S ^ 0x55;
      if (ID > IM)
      {
	if (SD <= 126)
	  SD++;
        else if (SD >= 129)
	  SD--;
	else
	  SD = SD == 128 ? 0 : 127;
      }
      else if (ID < IM)
      {
	if (SD <= 127)
	  SD = SD ? SD - 1 : 128;
	else
	  SD = SD == 255 ? 255 : SD + 1;
      }
      SD ^= 0x55;
    }
    else
    {
      SD = S;
      if (ID > IM)
      {
	if (1 <= S && S <= 127)
	  SD--;
	else if (128 <= S && S <= 254)
	  SD++;
	else
	  SD = S ? 126 : 0;
      }
      else if (ID < IM)
      {
	if (S <= 126)
	  SD++;
	else if (128 < S && S <= 255)
	  SD--;
	else
	  SD = S == 127 ? 254 : 128;
      }
    }
  }
}

static void diff_computation()
{
  /*
   * difference signal computation
   *
   * Input signals:  SL, SE
   * Output signals: D
   */
  U16BIT SLI, SEI;

  /* SUBTA */
  SLI = IF_ELSE(NEGATIVE(SL, 13), ((U16BIT) 49152L) + SL, SL);
  SEI = IF_ELSE(NEGATIVE(SE, 14), ((U16BIT) 32768L) + SE, SE);
  D = (SLI - SEI) & ((U16BIT) 65535L);
}

static void iadpt_quant()
{
  /*
   * inverse adaptive quantizer
   * 
   * Input signals:  I, Y
   * Output signals: DQ
   */
 
  static U16BIT qtab[] =
  {
    2048, 4, 135, 213, 273, 323, 373, 425,
    425, 373, 323, 273, 213, 135, 4, 2048,
  } ;
  U16BIT DQL;

  /* RECONST and ADDA */
  DQL = (qtab[I] + (Y >> 2));

  /* ANTILOG */
  {
    S16BIT DEX;
    U16BIT DQT;

    DEX = (DQL >> 7) & 15;
    DQT = (1 << 7) + (DQL & 127);
    DQ = IF_ELSE(NEGATIVE(I, 3), 1 << 14, 0) 
         + (NEGATIVE(DQL, 11) ? 0 : LSHIFT(DQT, 7 - DEX));
  }
}

static void input_conversion()
{
  /*
   * convert to uniform PCM
   * Input signals:  S
   * Output signals: SL
   */

  U16BIT SS, SSS, SSQ;

  /* EXPAND */
  if (LAW)
  {
    SS = A_LAW_table[S];
    SSS = NEGATIVE(SS, 12);
    SSQ = (SS & 4095) << 1;
  }
  else
  {
    SS = u_LAW_table[S];
    SSS = NEGATIVE(SS, 13);
    SSQ = SS & 8191;
  }
  SL = IF_ELSE(SSS, (-SSQ) & 16383, SSQ);
}

static void output_conversion()
{
  /*
   * Output PCM format conversion
   *
   * Input signals:  SR
   * Output signals: S
   */
  U16BIT IS, IM;

  IS = NEGATIVE(SR, 15);
  IM = IF_ELSE(IS, (-SR) & 32767, SR);

  if (LAW)
  {
    U16BIT MASK, SEG, IMS;

    MASK = IS ? 0x55 : 0xD5;
    IMS = IF_ELSE(IS, ((IM + 1) >> 1) - 1, IM >> 1);
    if (IMS > 4095)
      S = 0x7F ^ MASK;
    else
    {
      register U16BIT tmp = IMS >> 4;

      for (SEG = 0; tmp >>= 1; SEG++)
	;
      S = ((SEG << 4) | ((SEG ? IMS >> SEG : IMS >> 1) & 0xF)) ^ MASK;
    }
  }
  else
  {
    U16BIT MASK, IMS, SEG;

    MASK = IS ? 0x7F : 0xFF;
    IMS = IM + 33;
    if (IMS > 8191)
      S = 0x7F ^ MASK;
    else
    {
      for (SEG = 5; IMS >> SEG; SEG++)
	;
      SEG -= 6;
      S = ((SEG << 4) | ((IMS >> (SEG + 1)) & 0xF)) ^ MASK;
    }
  }
}

static void reset_states(void)
{
  register STATES* x = X;
  int i;

  for (i = 0; i < 8; i++)
    x->B[i] = 0;
  for (i = 0; i < 6; i++)
    x->DQ[i] = 32;
  x->DQ[6] = x->DQ[7] = 32;
  x->PK1 = x->PK2 = 0;
  x->AP = x->DMS = x->DML = 0;
  x->YU = 544;
  x->TD = 0;
  x->YL6 = (U16BIT) ((x->YL = (U24BIT) 34816L) >> 6);
}

static void scale_factor_1(void)
{
  /*
   * quantizer scale factor adaptation (part 1)
   * 
   * Input signals:  AL
   * Output signals: Y
   * States:         YU, YL6
   */

  register STATES* x = X;

  /* MIX */
  {
    U16BIT DIF, DIFS, DIFM, PRODM;

    DIF = x->YU + (-(x->YL6));
    DIFS = NEGATIVE(DIF, 13);
    DIFM = IF_ELSE(DIFS, (-DIF) & 8191, DIF);
    PRODM = (U16BIT) (((U24BIT) DIFM * AL) >> 6);
    Y = (x->YL6 + (IF_ELSE(DIFS, -PRODM, PRODM))) & 8191;
  }
}

static void scale_factor_2(void)
{
  /*
   * quantizer scale factor adaptation (part 2)
   *
   * Input signals:  I
   * Output signals: Y, X->YL, X->YL6
   * States:         YL, YU
   */

  static U16BIT W[] = 
  {
    4084, 18, 41, 64, 112, 198, 355, 1122
  } ;
  register STATES* x = X;
  U16BIT YUT;

  /* FUNCTW and FILTD */
  {
    U16BIT DIF, DIFS;

    DIF = (U16BIT) (((((U24BIT) (W[((I >> 3) ? 15 - I : I) & 7]) << 5) +
	   (-((U24BIT) Y))) >> 5) & 4095);
    DIFS = NEGATIVE(DIF, 11);
    YUT = (Y + (IF_ELSE(DIFS, DIF + 4096, DIF))) & 8191;
  }

  /* LIMB */
  x->YU = IF_ELSE(NEGATIVE((YUT + 15840) & 16383, 13),
		  544, IF_ELSE(NEGATIVE((YUT + 11264) & 16383, 13),
			       YUT, 5120));

  /* FILTE */
  {
    U16BIT DIF;
    U24BIT DIFSX;

    DIF = (x->YU + ((U16BIT) ((-(x->YL)) >> 6))) & 16383;
    DIFSX = IF_ELSE(NEGATIVE(DIF, 13), DIF + ((U24BIT) 507904L), DIF);
    x->YL6 = (U16BIT) ((x->YL = (x->YL + DIFSX) & ((U24BIT) 524287L)) >> 6);
  }
}

static void speed_control_1(void)
{
  /*
   * adaption speed control
   *
   * Input signals:  none
   * Output signals: AL
   * States:         AP
   */

  /* LIMA */
  AL = X->AP > 255 ? 64 : X->AP >> 2;
}

static void speed_control_2(void)
{
  /*
   * adaption speed control
   *
   * Input signals:  TR, TDP, I, Y
   * Output signals: none
   * States:         AP, DMS, DML
   */

  static U16BIT F[] = { 0, 0, 0, 1 << 9, 1 << 9, 1 << 9, 3 << 9, 7 << 9 };
  register STATES* x = X;
  U16BIT FI, AX, APP;

  /* FUNTCF */
  FI = F[IF_ELSE(I >> 3, (15 - I), I) & 7] ; 

  /* FILTA */
  {
    U16BIT DIF;

    DIF = ((FI - x->DMS) & 8191) >> 5;
    x->DMS = ((NEGATIVE(DIF, 7) ? DIF + 3840 : DIF) + x->DMS) & 4095;
  }

  /* FILTB */
  {
    U16BIT DIF;

    DIF = (((FI << 2) - x->DML) & 32767) >> 7;
    x->DML = ((NEGATIVE(DIF, 7) ? DIF + 16128 : DIF) + x->DML) & 16383;
  }

  /* SUBTC */
  {
    U16BIT DIF, DIFM;

    DIF = ((x->DMS << 2) - x->DML) & 32767;
    DIFM = IF_ELSE(NEGATIVE(DIF, 14),
		   (((U16BIT) 32768L) - DIF) & 16383, DIF);
    AX = (Y >= 1536 && DIFM < (x->DML >> 3) && !TDP) ? 0 : 1 << 9;
  }

  /* FILTC */
  {
    U16BIT DIF;

    DIF = ((AX - x->AP) & 2047) >> 4;
    APP = ((NEGATIVE(DIF, 6) ? DIF + 896 : DIF) + x->AP) & 1023;
  }

  /* TRIGA */
  x->AP = IF_ELSE(TR, 256, APP);
}

static void tone_detector_1(void)
{
  /*
   * tone and transition detector
   *
   * Input signals:  X->YL, DQ
   * Output signals: TR
   * STATES:         YL, TD;
   */

  register STATES* x = X;

  /* TRANS */
  {
    U16BIT DQMAG, YLINT, YLFRAC, THR2;

    DQMAG = DQ & 16383;
    YLINT = x->YL6 >> 9;
    YLFRAC = (x->YL6 >> 4) & 31;
    THR2 = IF_ELSE(YLINT > 8, 31 << 9, (32 + YLFRAC) << YLINT);
    TR = x->TD && DQMAG > ((THR2 + (THR2 >> 1)) >> 1);
  }
}

static void tone_detector_2(void)
{
  /*
   * tone and transition detector
   *
   * Input signals:  TR, x->B[7]
   * Output signals: TDP;
   * States:         TD;
   */

  /* TONE */
  TDP = ((U16BIT) 32768L) <= A2P && A2P < ((U16BIT ) 53760L);

  /* TRIGB */
  X->TD = IF_ELSE(TR, 0, TDP);
}

/*************************** public part *********************************/

int   LAW = u_LAW;

void reset_encoder(void)
{
  X = &E_STATES;
  reset_states();
}

U16BIT encoder(U16BIT pcm)
{
  S = pcm;
  X = &E_STATES;

  input_conversion();
  adpt_predict_1();
  diff_computation();
  speed_control_1();
  scale_factor_1();
  adapt_quant();
 
  iadpt_quant();
  tone_detector_1();
  adpt_predict_2();
  tone_detector_2();
  scale_factor_2();
  speed_control_2();

  return (I);
}

void reset_decoder(void)
{
  X = &D_STATES;
  reset_states();
}

U16BIT decoder(U16BIT adpcm)
{

  I = adpcm;
  X = &D_STATES;

  speed_control_1();
  scale_factor_1();
  iadpt_quant();
  tone_detector_1();
  adpt_predict_1();
  adpt_predict_2();
  tone_detector_2();
  scale_factor_2();
  speed_control_2();
  output_conversion();
  input_conversion();
  diff_computation();
  coding_adjustment();

  return (SD);
}
