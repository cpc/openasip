/*
 * benchmark program:   convolution.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         convolution - filter benchmarking
 * 
 * reference code:      target assembly
 * 
 * f. verification:     none
 * 
 *  organization:        Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Vojin Zivojnovic
 * 
 * history:             14-1-94 creation (Vojin Zivojnovic)
 *                      18-3-94 asm labels included (Martinez Velarde)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/01/30 07:24:54 $
 *                      $Revision: 1.2 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE  int
#define LENGTH 16

volatile TYPE init_value = 1;

void pin_down(TYPE * px, TYPE * ph)
{
	int i;

	for (i = 0; i < LENGTH; ++i) {
		*px++ = init_value;
		*ph++ = init_value;
	}

}


TYPE main()
{

	static TYPE     x[LENGTH];
	static TYPE     h[LENGTH];

	int i;
	STORAGE_CLASS TYPE y;
	STORAGE_CLASS TYPE *px = x;
	STORAGE_CLASS TYPE *ph = &h[LENGTH - 1];
        

	pin_down(&x[0], &h[0]);

	START_PROFILING;

	y = 0;

	for (i = 0; i < LENGTH; ++i)
		y += *px++ * *ph--;

	END_PROFILING;

    init_value = y;
    OUTPUT_VAR(init_value);

	return ((TYPE) y);

}
