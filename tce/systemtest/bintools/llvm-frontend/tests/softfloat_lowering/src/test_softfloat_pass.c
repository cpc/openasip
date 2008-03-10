/**
 * Test float to softfloat conversion pass.
 */

/* #include <stdlib.h> */
/* #include <stdio.h> */
/* #include "softfloat.h" */

enum {
    numInputs = 32
};

typedef long int32;
typedef unsigned long uint32;
typedef uint32 float32;
typedef char int8;
typedef int8 flag;
#define EXIT_SUCCESS 0

static const int32 inputs_int32[ numInputs ] = {
    0xFFFFBB79, 0x405CF80F, 0x00000000, 0xFFFFFD04,
    0xFFF20002, 0x0C8EF795, 0xF00011FF, 0x000006CA,
    0x00009BFE, 0xFF4862E3, 0x9FFFEFFE, 0xFFFFFFB7,
    0x0BFF7FFF, 0x0000F37A, 0x0011DFFE, 0x00000006,
    0xFFF02006, 0xFFFFF7D1, 0x10200003, 0xDE8DF765,
    0x00003E02, 0x000019E8, 0x0008FFFE, 0xFFFFFB5C,
    0xFFDF7FFE, 0x07C42FBF, 0x0FFFE3FF, 0x040B9F13,
    0xBFFFFFF8, 0x0001BF56, 0x000017F6, 0x000A908A
};

static const float32 inputs_float32[ numInputs ] = {
    0x4EFA0000, 0xC1D0B328, 0x80000000, 0x3E69A31E,
    0xAF803EFF, 0x3F800000, 0x17BF8000, 0xE74A301A,
    0x4E010003, 0x7EE3C75D, 0xBD803FE0, 0xBFFEFF00,
    0x7981F800, 0x431FFFFC, 0xC100C000, 0x3D87EFFF,
    0x4103FEFE, 0xBC000007, 0xBF01F7FF, 0x4E6C6B5C,
    0xC187FFFE, 0xC58B9F13, 0x4F88007F, 0xDF004007,
    0xB7FFD7FE, 0x7E8001FB, 0x46EFFBFF, 0x31C10000,
    0xDB428661, 0x33F89B1F, 0xA3BFEFFF, 0x537BFFBE
};

static const float32 inputs_float32_pos[ numInputs ] = {
    0x4EFA0000, 0x41D0B328, 0x00000000, 0x3E69A31E,
    0x2F803EFF, 0x3F800000, 0x17BF8000, 0x674A301A,
    0x4E010003, 0x7EE3C75D, 0x3D803FE0, 0x3FFEFF00,
    0x7981F800, 0x431FFFFC, 0x4100C000, 0x3D87EFFF,
    0x4103FEFE, 0x3C000007, 0x3F01F7FF, 0x4E6C6B5C,
    0x4187FFFE, 0x458B9F13, 0x4F88007F, 0x5F004007,
    0x37FFD7FE, 0x7E8001FB, 0x46EFFBFF, 0x31C10000,
    0x5B428661, 0x33F89B1F, 0x23BFEFFF, 0x537BFFBE
};

    
static char *functionName, *roundingModeName, *tininessModeName;

#define cast_to_float(x) *((float*)&x)
#define cast_to_int32(x) *((int32*)&x)

void checkReturn(int32 lib, int32 native, char* functionName) {
    if (lib != native) {
//        printf("Function %s Results did not match lib: %i, float: %i", functionName, lib, native);
//        exit(0);
    }
} 


int main( )
{
    int32 i;
    int8 inputNum = 0;        
    
    // check one input functions
    // int32_to_float32
    // float32_to_int32
    // float32_sqrt

    for ( i = 0; i < numInputs; i++) { 
        int32   result = int32_to_float32( inputs_int32[ inputNum ] ); 
        float resFloat = inputs_int32[ inputNum ];         
        checkReturn(result, cast_to_int32(resFloat), "cif");

        result = float32_to_int32( inputs_float32[ inputNum ] ); 
        int32 resInt = cast_to_float(inputs_float32[ inputNum ]);
        checkReturn(result, resInt, "cif");
        
        inputNum = ( inputNum + 1 ) & ( numInputs - 1 ); 
    } 

    int8 inputNumA=0, inputNumB=0;
    
    // check two input functions
    // float32_add
    // float32_sub
    // float32_mul
    // float32_div
    // float32_rem
    // float32_eq
    // float32_le
    // float32_lt
    
    for ( i = 0; i < numInputs*numInputs; i++ ) {

        // ********** ADD *************        
        float32 res = float32_add(inputs_float32[ inputNumA ], 
                                  inputs_float32[ inputNumB ] );        
        float native = 
            cast_to_float(inputs_float32[ inputNumA ]) + 
            cast_to_float(inputs_float32[ inputNumB ]);        
        checkReturn(res, cast_to_int32(native), "add");
        
        // ********** SUB *************
        res = float32_sub(inputs_float32[ inputNumA ], 
                          inputs_float32[ inputNumB ] );
        native = 
            cast_to_float(inputs_float32[ inputNumA ]) -
            cast_to_float(inputs_float32[ inputNumB ]);        
        checkReturn(res, cast_to_int32(native), "sub");

        // ********** MUL *************
        res = float32_mul(inputs_float32[ inputNumA ], 
                          inputs_float32[ inputNumB ] );        
        native = 
            cast_to_float(inputs_float32[ inputNumA ]) *
            cast_to_float(inputs_float32[ inputNumB ]);        
        checkReturn(res, cast_to_int32(native), "mul");

        // ********** DIV *************
        res = float32_div(inputs_float32[ inputNumA ], 
                          inputs_float32[ inputNumB ] );        
        native = 
            cast_to_float(inputs_float32[ inputNumA ]) /
            cast_to_float(inputs_float32[ inputNumB ]);        
        checkReturn(res, cast_to_int32(native), "div");

// float32 resRem = float32_rem(inputs_float32[ inputNumA ],  
//                              inputs_float32[ inputNumB ] );
        
        // ********** EQ *************
        flag resFlag = float32_eq(inputs_float32[ inputNumA ], 
                                  inputs_float32[ inputNumB ] );        
        flag nativeFlag = 
            (cast_to_float(inputs_float32[ inputNumA ]) ==
             cast_to_float(inputs_float32[ inputNumB ]));        
        checkReturn(resFlag, nativeFlag, "eq");
        
        // ********** LE *************
        resFlag = float32_le(inputs_float32[ inputNumA ], 
                         inputs_float32[ inputNumB ] );       
        nativeFlag = 
            (cast_to_float(inputs_float32[ inputNumA ]) <=
             cast_to_float(inputs_float32[ inputNumB ]));        
        checkReturn(resFlag, nativeFlag, "le");

        // ********** LT *************        
        resFlag = float32_lt(inputs_float32[ inputNumA ], 
                         inputs_float32[ inputNumB ] );        
        nativeFlag = 
            (cast_to_float(inputs_float32[ inputNumA ]) <
             cast_to_float(inputs_float32[ inputNumB ]));        
        checkReturn(resFlag, nativeFlag, "lt");


        inputNumA = ( inputNumA + 1 ) & ( numInputs - 1 );        
        if ( inputNumA == 0 ) ++inputNumB;
        inputNumB = ( inputNumB + 1 ) & ( numInputs - 1 );
    }

    return EXIT_SUCCESS;
}

