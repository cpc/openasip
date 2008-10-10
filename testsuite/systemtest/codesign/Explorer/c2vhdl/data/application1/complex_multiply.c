/*
 * benchmark program:   complex_multiply.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         complex_multiply - filter benchmarking
 * 
 * reference code:      target assembly
 * 
 * f. verification:     simulator based
 * 
 *  organization:        Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Juan Martinez Velarde
 * 
 * history:             9-5-94 creation (Martinez Velarde)
 * 
 *                      $Author: schraut $
 *                      $Revision: 1.2 $
 *                      $Date: 1995/02/01 11:54:14 $
 */

#define STORAGE_CLASS static
#define TYPE  int

TYPE _Output[6];

void
pin_down(TYPE *ar, TYPE *ai, TYPE *br, TYPE *bi, TYPE *cr, TYPE *ci)
{
  *ar = 2 ; 
  *ai = 1 ; 
  *br = 2 ; 
  *bi = 5 ; 
}

TYPE ar = 2, ai = 1;
TYPE br = 2, bi = 5;
TYPE cr, ci ;

void
main()
{
    int i;
  
    for(i = 0; i < 6; i++){
        cr  = ar*br - ai*bi;
        ci  = ar*bi + ai*br;
    }

    _Output[0] = cr;
    _Output[1] = ci;
}
