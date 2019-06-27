/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * OSAL behavior definition file.
 */

#define byte0(a)	a >> 24
#define byte1(a)	(a & 0xFF0000) >> 16
#define byte2(a)	(a & 0xFF00) >> 8
#define byte3(a)	(a & 0xFF)

#define byte0_2_word(b, a) a = (a & 0xFFFFFF) | b << 24
#define byte1_2_word(b, a) a = (a & 0xFF00FFFF) | b << 16
#define byte2_2_word(b, a) a = (a & 0xFFFF00FF) | b << 8
#define byte3_2_word(b, a) a = (a & 0xFFFFFF00) | b

#define bytes2word(b0, b1, b2, b3, a) a = (b0 << 24) | (b1 << 16) | (b2 << 8) | (b3)

#include "OSAL.hh"

OPERATION(SHIFTROWS)
TRIGGER

unsigned long state[4];
state[0] = (UINT(1));
state[1] = (UINT(2));
state[2] = (UINT(3));
state[3] = (UINT(4));

unsigned int aux;
  aux = byte0(state[1]);
  byte0_2_word(byte1(state[1]),state[1]);
  byte1_2_word(byte2(state[1]),state[1]);
  byte2_2_word(byte3(state[1]),state[1]);
  byte3_2_word(aux,state[1]);
  
  aux = byte0(state[2]);
  byte0_2_word(byte2(state[2]),state[2]);
  byte2_2_word(aux,state[2]);
  aux = byte1(state[2]);
  byte1_2_word(byte3(state[2]),state[2]);
  byte3_2_word(aux,state[2]);
  
  aux = byte3(state[3]);
  byte3_2_word(byte2(state[3]),state[3]);
  byte2_2_word(byte1(state[3]),state[3]);
  byte1_2_word(byte0(state[3]),state[3]);
  byte0_2_word(aux,state[3]);

IO(5) = static_cast<unsigned> (state[0]);
IO(6) = static_cast<unsigned> (state[1]);
IO(7) = static_cast<unsigned> (state[2]);
IO(8) = static_cast<unsigned> (state[3]);

return true;
END_TRIGGER;
END_OPERATION(SHIFTROWS)


///////////////////////////////////////////////////////////
OPERATION(SUBSHIFT)
TRIGGER
unsigned char Sen[] = {
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

unsigned long state[4];
state[0] = (UINT(1));
state[1] = (UINT(2));
state[2] = (UINT(3));
state[3] = (UINT(4));

	bytes2word(Sen[byte0(state[0])],Sen[byte1(state[0])],Sen[byte2(state[0])],Sen[byte3(state[0])],state[0]);
	bytes2word(Sen[byte1(state[1])],Sen[byte2(state[1])],Sen[byte3(state[1])],Sen[byte0(state[1])],state[1]);
	bytes2word(Sen[byte2(state[2])],Sen[byte3(state[2])], Sen[byte0(state[2])], Sen[byte1(state[2])],state[2]);
	bytes2word(Sen[byte3(state[3])],Sen[byte0(state[3])],Sen[byte1(state[3])],Sen[byte2(state[3])], state[3]);

IO(5) = static_cast<unsigned> (state[0]);
IO(6) = static_cast<unsigned> (state[1]);
IO(7) = static_cast<unsigned> (state[2]);
IO(8) = static_cast<unsigned> (state[3]);

return true;
END_TRIGGER;
END_OPERATION(SUBSHIFT)
////////////////////////////////////////////////////////////

OPERATION(MUL_GAL)
TRIGGER

unsigned int Alogtable[] = {
1, 3, 5, 15, 17, 51, 85, 255, 26, 46, 114, 150, 161, 248, 19, 53,
95, 225, 56, 72, 216, 115, 149, 164, 247, 2, 6, 10, 30, 34, 102, 170,
229, 52, 92, 228, 55, 89, 235, 38, 106, 190, 217, 112, 144, 171, 230, 49,
83, 245, 4, 12, 20, 60, 68, 204, 79, 209, 104, 184, 211, 110, 178, 205,
76, 212, 103, 169, 224, 59, 77, 215, 98, 166, 241, 8, 24, 40, 120, 136,
131, 158, 185, 208, 107, 189, 220, 127, 129, 152, 179, 206, 73, 219, 118, 154,
181, 196, 87, 249, 16, 48, 80, 240, 11, 29, 39, 105, 187, 214, 97, 163,
254, 25, 43, 125, 135, 146, 173, 236, 47, 113, 147, 174, 233, 32, 96, 160,
251, 22, 58, 78, 210, 109, 183, 194, 93, 231, 50, 86, 250, 21, 63, 65,
195, 94, 226, 61, 71, 201, 64, 192, 91, 237, 44, 116, 156, 191, 218, 117,
159, 186, 213, 100, 172, 239, 42, 126, 130, 157, 188, 223, 122, 142, 137, 128,
155, 182, 193, 88, 232, 35, 101, 175, 234, 37, 111, 177, 200, 67, 197, 84,
252, 31, 33, 99, 165, 244, 7, 9, 27, 45, 119, 153, 176, 203, 70, 202,
69, 207, 74, 222, 121, 139, 134, 145, 168, 227, 62, 66, 198, 81, 243, 14,
18, 54, 90, 238, 41, 123, 141, 140, 143, 138, 133, 148, 167, 242, 13, 23,
57, 75, 221, 124, 132, 151, 162, 253, 28, 36, 108, 180, 199, 82, 246, 1,
};
/*----------------------------------------------------------------------------*/
unsigned int Logtable[] = {
0, 0, 25, 1, 50, 2, 26, 198, 75, 199, 27, 104, 51, 238, 223, 3,
100, 4, 224, 14, 52, 141, 129, 239, 76, 113, 8, 200, 248, 105, 28, 193,
125, 194, 29, 181, 249, 185, 39, 106, 77, 228, 166, 114, 154, 201, 9, 120,
101, 47, 138, 5, 33, 15, 225, 36, 18, 240, 130, 69, 53, 147, 218, 142,
150, 143, 219, 189, 54, 208, 206, 148, 19, 92, 210, 241, 64, 70, 131, 56,
102, 221, 253, 48, 191, 6, 139, 98, 179, 37, 226, 152, 34, 136, 145, 16,
126, 110, 72, 195, 163, 182, 30, 66, 58, 107, 40, 84, 250, 133, 61, 186,
43, 121, 10, 21, 155, 159, 94, 202, 78, 212, 172, 229, 243, 115, 167, 87,
175, 88, 168, 80, 244, 234, 214, 116, 79, 174, 233, 213, 231, 230, 173, 232,
44, 215, 117, 122, 235, 22, 11, 245, 89, 203, 95, 176, 156, 169, 81, 160,
127, 12, 246, 111, 23, 196, 73, 236, 216, 67, 31, 45, 164, 118, 123, 183,
204, 187, 62, 90, 251, 96, 177, 134, 59, 82, 161, 108, 170, 85, 41, 157,
151, 178, 135, 144, 97, 190, 220, 252, 188, 149, 207, 205, 55, 63, 91, 209,
83, 57, 132, 60, 65, 162, 109, 71, 20, 42, 158, 93, 86, 242, 211, 171,
68, 17, 146, 217, 35, 32, 46, 137, 180, 124, 184, 38, 119, 153, 227, 165,
103, 74, 237, 222, 197, 49, 254, 24, 13, 99, 140, 128, 192, 247, 112, 7,
};

unsigned int a, b, d, ret;
a = (UINT(1));
b = (UINT(2));


d = 0;
ret = 0;

d = (Logtable[a] + Logtable[b]);

if (d >= 255)
    d -= 255;


ret = Alogtable[d];
if (!(a && b))
    ret = 0;


IO(3) = static_cast<unsigned> (ret);

return true;
END_TRIGGER;
END_OPERATION(MUL_GAL)
////////////////////////////////////////////////////////////

OPERATION(PACK)
TRIGGER

unsigned int a, b0, b1, b2, b3;
b0 = (UINT(1));
b1 = (UINT(2));
b2 = (UINT(3));
b3 = (UINT(4));

a = (b0 << 24) | (b1 << 16) | (b2 << 8) | (b3);

IO(5) = static_cast<unsigned> (a);

return true;
END_TRIGGER;
END_OPERATION(PACK)
