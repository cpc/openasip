/************************************************************************
 * The possible overflow in every addition is avoided by always
 * dividing the result of addition by 2. This means the final result of 
 * FFT algorithm is heavily scaled but the final user can compensate
 * this in his own implementation.
 ************************************************************************
 * File: sfus.h
 * Description: Declarations of SFUs if gcc-move compiler is used. 
 * If gcc compiler is used this file declares the c-language functions
 * simulating SFUs. 
 ************************************************************************ 
 * Project: FlexDSP                                  
 * Author: Risto Mäkinen <rmmakine@cs.tut.fi>                           
 ************************************************************************/

# ifndef SFUS_H
# define SFUS_H

#ifdef __TCE_V1__

  /* Customops with the new tce llvm style */
  #include "tceops.h"

  #define ag(r0,r1,a,b,c,d) _TCE_AG(a,b,c,d, r0, r1)
  #define cmul(a,b) ({ int result; _TCE_CMUL(a,b, result); result; })
  #define fgen(a,b) ({ int result; _TCE_FGEN(a,b, result);  result; })
  #define cadd(a,b,c,d,e) ({ int result; _TCE_CADD(a,b,c,d,e, result); result; })

#else

# include "userdef.h"

/* Declarations of custom operation execution macros */
# define ag(r0,r1,a,b,c,d) {\
        WRITETO(ag.1, a);\
        WRITETO(ag.2, b);\
        WRITETO(ag.3, c);\
        WRITETO(ag.4, d);\
        READINT(ag.5, r0);\
        READINT(ag.6, r1);}

# define cmul(a,b) ({                           \
        int result;\
        WRITETO(cmul.1, a);\
        WRITETO(cmul.2, b);\
        READINT(cmul.3, result);\
        result;})

# define cadd(a,b,c,d,e) ({                     \
        int result;\
        WRITETO(cadd.1, a);\
        WRITETO(cadd.2, b);\
        WRITETO(cadd.3, c);\
        WRITETO(cadd.4, d);\
        WRITETO(cadd.5, e);\
        READINT(cadd.6, result);\
        result;})

# define fgen(a,b) ({                           \
        int result;\
        WRITETO(fgen.1, a);\
        WRITETO(fgen.2, b);\
        READINT(fgen.3, result);\
        result;})

# endif /* __TCE_V1__ */

# endif /* SFUS_H */
