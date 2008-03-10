/* int *sp,r8
   sp--;
   *sp = RA;
   r8 = sp[1];
   r7 = sp[2];
   rv = sum..
   RA = *sp;
   sp++
*/

int foo(int a, int b, int c, int d, int e, int f) {
    return a+b+c+d+e+f;
}

/* sp--;
   *sp = RA;
   RA = *sp;
   sp++;
*/
void bar() {};

/* sp--;
   *sp = RA;
   sp-=2;
   call
   *sp = 5;
   sp[1] = 6;
   r2=1,r3=2,r4=3,r5=4
   call
   sp+=2
   RA=*sp
   sp++
*/

int main() {
    
    int a=1,b=2,c=3,d=4,e=5,f=6;
    bar();
    foo(a,b,c,d,e,f);
}


/*

RA
out_param1
out_param2 <-- SP

---

in_param1
in_param2
RA <-- SP


=>

in_param1
in_param2 <-- add FP here
local_variables
RA
saved_regs
out_param1
out_param2 <-- SP



if ( SP_offset > number_of_out_params ) 
    SP_offset += local_variable_space

*/






