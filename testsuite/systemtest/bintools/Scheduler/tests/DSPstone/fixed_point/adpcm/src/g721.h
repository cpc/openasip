#ifndef __G721_H__
#define __G721_H__

#include "portab.h"

#define u_LAW 0
#define A_LAW 1

extern int LAW;

void reset_encoder(void);
U16BIT encoder(U16BIT);

void reset_decoder(void);
U16BIT decoder(U16BIT);

#endif
