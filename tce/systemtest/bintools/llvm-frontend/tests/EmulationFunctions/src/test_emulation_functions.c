#include <stdio.h> 
#include <stdint.h>

/* number of generated values nearby each value barrier 
   0, +max_i32, -max_i32 */
#define PARAM_SERIES 5

#define TEST_CASES 3*PARAM_SERIES
char buffer[500];

int64_t a,b, sum1, sub1, mul1, sum2, sub2, mul2;

int main(int argc,char **argv,char **envp) { 
    int64_t 
        params[TEST_CASES], 
        rmul[TEST_CASES*TEST_CASES], 
        radd[TEST_CASES*TEST_CASES], 
        rsub[TEST_CASES*TEST_CASES];

    uint64_t 
        uparams[TEST_CASES], 
        urmul[TEST_CASES*TEST_CASES], 
        uradd[TEST_CASES*TEST_CASES], 
        ursub[TEST_CASES*TEST_CASES];

    int i,j,k;
    uint64_t retVal = 0;


    printf("lshr pattern 0xAAAACCCC55553333 and print low word:\n");
    uint64_t ui64testVal = 0xAAAACCCC55553333ll;
    for (i = 0; i < 64; i++) {
        printf("Logical shift right %i %12x \n", i, (uint32_t)(ui64testVal >> i));        
    }

    printf("ashr pattern 0xAAAACCCC55553333 and print low word:\n");
    int64_t i64testVal = 0xAAAACCCC55553333ll;
    for (i = 0; i < 64; i++) {
        printf("Artihmetic shift right %i %12x \n", i, (uint32_t)(i64testVal >> i));        
    }

    /* Simple test case*/
    printf("Doing some simple add, sub and mul:\n");
    a = 0x21293834ll;
    b = 0xf4923871ll;
    sum1 = a + b;
    sum2 = b + a;

    sub1 = a - b;
    sub2 = b - a; 

    mul1 = a * b;
    mul2 = b * a;

    printf( "sums: %12x %12x %12x %12x\n",
            (uint32_t)(sum1 >> 32), (uint32_t)(sum1),
            (uint32_t)(sum2 >> 32), (uint32_t)(sum2));

    printf( "subs: %12x %12x %12x %12x\n",
            (uint32_t)(sub1 >> 32), (uint32_t)(sub1),
            (uint32_t)(sub2 >> 32), (uint32_t)(sub2));

    printf( "muls: %12x %12x %12x %12x\n",
            (uint32_t)(mul1 >> 32), (uint32_t)(mul1),
            (uint32_t)(mul2 >> 32), (uint32_t)(mul2));

    /* More complex case */
    printf("Calculating with some trickier values:\n");
    printf( "params             signed                  unsigned\n");
      
    for (i = 0; i < TEST_CASES/3; i++) {

        params [i*3] = (int64_t)(i)  - (TEST_CASES/6);
        uparams[i*3] = (uint64_t)(i) - (TEST_CASES/6);

        params [i*3+1] = (int64_t)(i)  - (TEST_CASES/6) + INT32_MAX;
        uparams[i*3+1] = (uint64_t)(i) - (TEST_CASES/6) + UINT32_MAX;

//        params [i*3+2] = (int64_t)(i)  - (TEST_CASES/6) - INT32_MAX;
//        uparams[i*3+2] = (uint64_t)(i) - (TEST_CASES/6) - UINT32_MAX;

/* newlib doesn't seem to work perfectly.. it should know how to print these
   printf(       "%25Ld %25Lu\n"
                 "%25Ld %25Lu\n"
                 "%25Ld %25Lu\n", 
                 params[i*3],   uparams[i*3], 
                 params[i*3+1], uparams[i*3+1],
                 params[i*3+2], uparams[i*3+2]);
*/


        printf( "%12d %12d %12u %12u\n"
                "%12d %12d %12u %12u\n"
                "%12d %12d %12u %12u\n",
                (int32_t) (params[i*3]    >> 32), (int32_t) (params[i*3]),
                (uint32_t)(uparams[i*3]   >> 32), (uint32_t)(uparams[i*3]),
                (int32_t) (params[i*3+1]  >> 32), (int32_t) (params[i*3+1]),
                (uint32_t)(uparams[i*3+1] >> 32), (uint32_t)(uparams[i*3+1]),
                (int32_t) (params[i*3+2]  >> 32), (int32_t) (params[i*3+2]),
                (uint32_t)(uparams[i*3+2] >> 32), (uint32_t)(uparams[i*3+2]));

    }

    for (j = 0; j < TEST_CASES; j++) {
        for (k = 0; k < TEST_CASES; k++) {
            rmul[j*TEST_CASES+k] = params[j] * params[k];
            radd[j*TEST_CASES+k] = params[j] + params[k];
            rsub[j*TEST_CASES+k] = params[j] - params[k];
 
            urmul[j*TEST_CASES+k] = uparams[j] * uparams[k];
            uradd[j*TEST_CASES+k] = uparams[j] + uparams[k];
            ursub[j*TEST_CASES+k] = uparams[j] - uparams[k];

            retVal += 
                rmul[j*TEST_CASES+k] + 
                radd[j*TEST_CASES+k] + 
                rsub[j*TEST_CASES+k] + 
                urmul[j*TEST_CASES+k] + 
                uradd[j*TEST_CASES+k] + 
                ursub[j*TEST_CASES+k];
        }
    }
    
    for (i = 0; i < TEST_CASES*TEST_CASES; i++) {
/*
        printf(buffer,
                "signed results: %25lli %25lli %25lli"
                "\tunsigned res: %25llu %25llu %25llu\n",
                 rmul[i],  radd[i],  rsub[i],
                 urmul[i], uradd[i], ursub[i]);
*/

        printf( "signed results: %12i %12i %12i %12i %12i %12i"
                "\tunsign results: %12u %12u %12u %12u %12u %12u\n",
                (int32_t)(rmul[i] >> 32),  (int32_t)rmul[i],
                (int32_t)(radd[i] >> 32),  (int32_t)radd[i],
                (int32_t)(rsub[i] >> 32),  (int32_t)rsub[i],
                (uint32_t)(urmul[i] >> 32),  (uint32_t)urmul[i],
                (uint32_t)(uradd[i] >> 32),  (uint32_t)uradd[i],
                (uint32_t)(ursub[i] >> 32),  (uint32_t)ursub[i]);
    }

    return retVal;
}
