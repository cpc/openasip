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
 * history:             14-01-94 creation for fixed-point (Vojin Zivojnovic)
 *                      18-03-94 asm labels included (Martinez Velarde)
 *                      16-03-95 adaption for floating-point (Harald L. Schraut)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/04/11 05:44:57 $
 *                      $Revision: 1.2 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE float
#define LENGTH 16

volatile TYPE init_value = 1;

void pin_down(TYPE * px, TYPE * ph)
{
	int i;

	for (i = 0; i < LENGTH; ++i) {
		*px++ = 1;
		*ph++ = 1;
	}

}


int main()
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

    return 0;
}
