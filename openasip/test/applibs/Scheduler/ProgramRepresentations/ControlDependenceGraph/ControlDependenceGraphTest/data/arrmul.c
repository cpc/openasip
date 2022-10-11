#define ELEMENTS 20
int arrmul(int* a, int* b, int*c, int n ) {
    int i;
    for( i = 0; i < n; i++ ) {
        a[i] = b[i] * c[i];
    }
}
/*
  sp-=60
  sp--
  *sp = RA
  r2 = &sp[41]
  r3 = &sp[21]
  r4 = &sp[4]
  RA = *sp
  sp++
  sp+=60
*/

int main() {
    int a[ELEMENTS];
    int b[ELEMENTS];
    int c[ELEMENTS];
    arrmul(a,b,c, ELEMENTS);
}
