
/*============================================================================

This C source file is part of the SoftFloat IEC/IEEE Floating-point Arithmetic
Package, Release 2b.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page `http://www.cs.berkeley.edu/~jhauser/
arithmetic/SoftFloat.html'.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.

=============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "softfloat.h"

volatile int32 inputs_int32[] = {
    0xFFFFBB79, 0x405CF80F, 0x00000000, 0xFFFFFD04,
    0xFFF20002, 0x0C8EF795, 0xF00011FF, 0x000006CA,
    0x00009BFE, 0xFF4862E3, 0x9FFFEFFE, 0xFFFFFFB7,
    0x0BFF7FFF, 0x0000F37A, 0x0011DFFE, 0x00000006,
    0xFFF02006, 0xFFFFF7D1, 0x10200003, 0xDE8DF765,
    0x00003E02, 0x000019E8, 0x0008FFFE, 0xFFFFFB5C,
    0xFFDF7FFE, 0x07C42FBF, 0x0FFFE3FF, 0x040B9F13,
    0xBFFFFFF8, 0x0001BF56, 0x000017F6, 0x000A908A
};

volatile int32 inputs_int32_size = sizeof(inputs_int32) / sizeof(int32);

volatile float32 inputs_float32[] = {
    0x4EFA0000, 0xC1D0B328, 0x80000000, 0x3E69A31E,
    0x41D0B328, 0x00000000, 0xAF803EFF, 0x3F800000, 
    0x17BF8000, 0xE74A301A, 0x2F803EFF, 0x674A301A,
    0x4E010003, 0x7EE3C75D, 0xBD803FE0, 0xBFFEFF00,
    0x3D803FE0, 0x3FFEFF00, 0x7981F800, 0x431FFFFC, 
    0xC100C000, 0x3D87EFFF, 0x4100C000, 0x3D87EFFF,
    0x4103FEFE, 0xBC000007, 0xBF01F7FF, 0x4E6C6B5C,
    0x3C000007, 0x3F01F7FF, 0xC187FFFE, 0xC58B9F13, 
    0x4F88007F, 0xDF004007, 0x4187FFFE, 0x458B9F13, 
    0x5F004007, 0xB7FFD7FE, 0x7E8001FB, 0x46EFFBFF, 
    0x31C10000, 0x37FFD7FE, 0xDB428661, 0x33F89B1F, 
    0xA3BFEFFF, 0x537BFFBE, 0x5B428661, 0x23BFEFFF   
};

volatile int32 inputs_float32_size = sizeof(inputs_float32) / sizeof(float32);

volatile struct {
    bits32 high, low;
} inputs_float64[] = {
    { 0x422FFFC0, 0x08000000 },
    { 0xB7E00004, 0x80000000 },
    { 0xF3FD2546, 0x120B7935 },
    { 0x3FF00000, 0x00000000 },
    { 0xCE07F766, 0xF09588D6 },
    { 0x80000000, 0x00000000 },
    { 0x3FCE0004, 0x00000000 },
    { 0x8313B60F, 0x0032BED8 },
    { 0xC1EFFFFF, 0xC0002000 },
    { 0x3FB3C75D, 0x224F2B0F },
    { 0x7FD00000, 0x004000FF },
    { 0xA12FFF80, 0x00001FFF },
    { 0x3EE00000, 0x00FE0000 },
    { 0x00100000, 0x80000004 },
    { 0x41CFFFFE, 0x00000020 },
    { 0x40303FFF, 0xFFFFFFFD },
    { 0x3FD00000, 0x3FEFFFFF },
    { 0xBFD00000, 0x10000000 },
    { 0xB7FC6B5C, 0x16CA55CF },
    { 0x413EEB94, 0x0B9D1301 },
    { 0xC7E00200, 0x001FFFFF },
    { 0x47F00021, 0xFFFFFFFE },
    { 0xBFFFFFFF, 0xF80000FF },
    { 0xC07FFFFF, 0xE00FFFFF },
    { 0x001497A6, 0x3740C5E8 },
    { 0xC4BFFFE0, 0x001FFFFF },
    { 0x96FFDFFE, 0xFFFFFFFF },
    { 0x403FC000, 0x000001FE },
    { 0xFFD00000, 0x000001F6 },
    { 0x06404000, 0x02000000 },
    { 0x479CEE1E, 0x4F789FE0 },
    { 0xC237FFFF, 0xFFFFFDFE }
};

volatile struct {
    bits32 high, low;
} inputs_float64_pos[] = {
    { 0x422FFFC0, 0x08000000 },
    { 0x37E00004, 0x80000000 },
    { 0x73FD2546, 0x120B7935 },
    { 0x3FF00000, 0x00000000 },
    { 0x4E07F766, 0xF09588D6 },
    { 0x00000000, 0x00000000 },
    { 0x3FCE0004, 0x00000000 },
    { 0x0313B60F, 0x0032BED8 },
    { 0x41EFFFFF, 0xC0002000 },
    { 0x3FB3C75D, 0x224F2B0F },
    { 0x7FD00000, 0x004000FF },
    { 0x212FFF80, 0x00001FFF },
    { 0x3EE00000, 0x00FE0000 },
    { 0x00100000, 0x80000004 },
    { 0x41CFFFFE, 0x00000020 },
    { 0x40303FFF, 0xFFFFFFFD },
    { 0x3FD00000, 0x3FEFFFFF },
    { 0x3FD00000, 0x10000000 },
    { 0x37FC6B5C, 0x16CA55CF },
    { 0x413EEB94, 0x0B9D1301 },
    { 0x47E00200, 0x001FFFFF },
    { 0x47F00021, 0xFFFFFFFE },
    { 0x3FFFFFFF, 0xF80000FF },
    { 0x407FFFFF, 0xE00FFFFF },
    { 0x001497A6, 0x3740C5E8 },
    { 0x44BFFFE0, 0x001FFFFF },
    { 0x16FFDFFE, 0xFFFFFFFF },
    { 0x403FC000, 0x000001FE },
    { 0x7FD00000, 0x000001F6 },
    { 0x06404000, 0x02000000 },
    { 0x479CEE1E, 0x4F789FE0 },
    { 0x4237FFFF, 0xFFFFFDFE }
};

volatile int32_t i32_a, i32_b;
volatile float float_a, float_b;
volatile float32 f32_a, f32_b;

void check_results_i32_i32(int32_t lib, int32_t reference, const char* name) {
    if (lib != reference) {
        printf("Error calculating %s lib: %i(%8x) reference: %i(%8x) " 
               "float_a: %f(%8x) f32_a: %f(%8x), float_b: %f(%8x) f32_b: %f(%8x)\n", 
               name, lib, lib, reference, reference, 
               float_a, *(float32*)&float_a, *(float*)&f32_a, f32_a, 
               float_b, *(float32*)&float_b, *(float*)&f32_b, f32_b);
    }
}

void check_results_f32_float(float32 lib, float reference, const char* name, int int_params) {
    if (lib != (*((float32*)&reference))) {
        if (int_params) {
            printf("Error calculating %s lib: %f(%8x) reference: %f(%8x) " 
                   "i32_a: %8x, i32_b: %8x\n", 
                   name, *(float*)&lib, *(float32*)&lib, *(float*)&reference, *(float32*)&reference, i32_a, i32_b);
        } else {
            printf("Error calculating %s lib: %f(%8x) reference: %f(%8x) " 
                   "float_a/f32_a: %f(%8x), float_b/f32_b: %f(%8x)\n", 
                   name,
                   *(float*)&lib,     *(float32*)&lib,     *(float*)&reference, *(float32*)&reference, 
                   *(float*)&float_a, *(float32*)&float_a, *(float*)&float_b,   *(float32*)&float_b);
        }
    }
}

int main( )
{
    int i, a, b;
    // check functions that use one int parameters
    for (i = 0; i < inputs_int32_size;i++) {                
        i32_a = inputs_int32[i];

        // functions to check
        check_results_f32_float(int32_to_float32(i32_a),(float)i32_a, "cif", 1);
        check_results_f32_float(uint32_to_float32(i32_a), (float)((uint32_t)i32_a), "cifu", 1);
    }
    
    // check functions that use one float parameter
    for (i = 0; i < inputs_float32_size;i++) {        
        f32_a = inputs_float32[i];       
        float_a = *(float*)&f32_a;

        // functions to check
        check_results_i32_i32(float32_to_int32_round_to_zero(f32_a), (int32_t)float_a, "cfi"); 
        // check_results_i32_i32(float32_to_int32(f32_a), (int32_t)float_a, "cfi");
        check_results_i32_i32(float32_to_uint32(f32_a), (uint32_t)float_a, "cfiu");
        // check_results_i32_i32(float32_round_to_int(f32_a), (int32_t)float_a, "round");
        // check_results_f32_float(float32_sqrt(f32_a), sqrtf(float_a), "sqrt", 0);
    }
    

    // check functions that use two float parameters
    for (a = 0; a < inputs_float32_size; a++) {
        f32_a = inputs_float32[a];
        float_a = *((float*)&f32_a);
        for (b = 0; b < inputs_float32_size; b++) {
            f32_b = inputs_float32[b];
            float_b = *((float*)&f32_b);

            // functions to check
            check_results_f32_float(float32_add(f32_a, f32_b), float_a + float_b, "add", 0);
            check_results_f32_float(float32_sub(f32_a, f32_b), float_a - float_b, "sub", 0);
            check_results_f32_float(float32_mul(f32_a, f32_b), float_a * float_b, "mul", 0);
            check_results_f32_float(float32_div(f32_a, f32_b), float_a / float_b, "div", 0);
            // check_results_f32_float(float32_rem(f32_a, f32_b), float_a % float_b, "rem", 0);

            check_results_i32_i32(float32_eq(f32_a, f32_b), float_a == float_b, "eq");  
            check_results_i32_i32(float32_le(f32_a, f32_b), float_a <= float_b, "le");        
            check_results_i32_i32(float32_lt(f32_a, f32_b), float_a < float_b, "lt");       

            check_results_i32_i32(float32_eq_signaling(f32_a, f32_b), float_a == float_b, "eq_signaling");  
            check_results_i32_i32(float32_le_quiet(f32_a, f32_b), float_a <= float_b, "le_quiet");        
            check_results_i32_i32(float32_lt_quiet(f32_a, f32_b), float_a < float_b, "lt_quiet");       
        }
    }

    /* not supported 
    functionName = "int32_to_float64";
    time_a_int32_z_float64( int32_to_float64 );
    functionName = "float32_to_float64";
    time_a_float32_z_float64( float32_to_float64 );
    functionName = "float64_to_int32";
    time_a_float64_z_int32( float64_to_int32 );
    functionName = "float64_to_int32_round_to_zero";
    time_a_float64_z_int32( float64_to_int32_round_to_zero );
    functionName = "float64_to_float32";
    time_a_float64_z_float32( float64_to_float32 );
    functionName = "float64_round_to_int";
    time_az_float64( float64_round_to_int );
    functionName = "float64_add";
    time_abz_float64( float64_add );
    functionName = "float64_sub";
    time_abz_float64( float64_sub );
    functionName = "float64_mul";
    time_abz_float64( float64_mul );
    functionName = "float64_div";
    time_abz_float64( float64_div );
    functionName = "float64_rem";
    time_abz_float64( float64_rem );
    functionName = "float64_sqrt";
    time_az_float64_pos( float64_sqrt );
    functionName = "float64_eq";
    time_ab_float64_z_flag( float64_eq );
    functionName = "float64_le";
    time_ab_float64_z_flag( float64_le );
    functionName = "float64_lt";
    time_ab_float64_z_flag( float64_lt );
    functionName = "float64_eq_signaling";
    time_ab_float64_z_flag( float64_eq_signaling );
    functionName = "float64_le_quiet";
    time_ab_float64_z_flag( float64_le_quiet );
    functionName = "float64_lt_quiet";
    time_ab_float64_z_flag( float64_lt_quiet );
    */

    return EXIT_SUCCESS;
}
 
