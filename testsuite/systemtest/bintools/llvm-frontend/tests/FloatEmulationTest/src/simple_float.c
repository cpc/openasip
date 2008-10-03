#include <stdio.h>
/* #include <stdint.h> */

volatile static unsigned a = 4100000000u;
volatile static int b = 41000;

int main(int argc,char **argv,char **envp) { 

    float aFloat, bFloat;
    float resMul, resDiv, resMod, resAdd, resSub, resNeg;
    
    aFloat = a;
    bFloat = b;
    
    resAdd = aFloat + bFloat;
    resSub = aFloat - bFloat;
    resNeg = -aFloat;

    resMul = aFloat * bFloat;
    resDiv = aFloat / bFloat;
//    resMod = aFloat % bFloat;
    

    printf("%x+%x=%x\n", *((int*)&aFloat), *((int*)&bFloat), *((int*)&resAdd));  
    printf("%x-%x=%x\n", *((int*)&aFloat), *((int*)&bFloat), *((int*)&resSub));  
    printf("%x*%x=%x\n", *((int*)&aFloat), *((int*)&bFloat), *((int*)&resMul));  
    printf("%x/%x=%x\n", *((int*)&aFloat), *((int*)&bFloat), *((int*)&resDiv));  
    printf("-%x=%x\n", *((int*)&aFloat), *((int*)&resNeg));  

    unsigned srcVal = a;
    float tempFloat;
    unsigned dstVal1, dstVal2;
    
    tempFloat = srcVal;
    dstVal1 = tempFloat;
    tempFloat = -tempFloat;
    dstVal2 = tempFloat;

    printf("%x to fp to uint %x\n", srcVal, dstVal1);  
    printf("%x to fp to -fp to uint %x\n", srcVal, dstVal2);  

    printf("%f+%f=%f\n", aFloat, bFloat, resAdd);
    printf("%f-%f=%f\n", aFloat, bFloat, resSub);
    printf("%f*%f=%f\n", aFloat, bFloat, resMul);
    printf("%f/%f=%f\n", aFloat, bFloat, resDiv);
    printf("-%f=%f\n", aFloat, resNeg);

    if (resDiv > resMul) return 1;
    if (resMod >= resAdd) return 2;
    if (resMod < resDiv) return 3;
    if (resAdd <= resNeg) return 4;

    if (resMod != resNeg &&         
        resMod == resSub) return 6;        

    return 0;
}
