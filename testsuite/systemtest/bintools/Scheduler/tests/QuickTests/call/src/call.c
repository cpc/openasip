#include <stdio.h>

typedef union {
   int a;
   short b[2];
} u1;

typedef struct {
   int* a;
   short* b;
   char* c;
   u1 u;
} s2;

typedef struct {
   char a;
   int b;
   short c;
   unsigned char d;
   unsigned int e;
   unsigned short f;
   s2 g;
} s1;

int a = 123456;
short b = -120;
char c = 100; 
unsigned count = 0;

int call_int(int a)  {
   printf("call_int(%d)", a);
   return a*a;
}

unsigned int call_uint(unsigned int a)  {
   printf("call_uint(%d)", a);
   return a+1;
}

int call_ints(int a, int b, int c, int d, int e, int f,
	      int g, int h, int i, int j, int k, int l,
	      int m, int n, int o, int p, int q, int r,
	      int s, int t, int u, int v, int w, unsigned int x
	      )  {
   
   printf("call_ints(%d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d)",
	  a,b,c,d,e,f,g,h,j,i,k,l,m,n,o,p,q,e,r,s,t,u,v,w,x);
   
   return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x;
}


short call_short(short a)  {
   printf("call_short(%d)", a);
   return a*a;
}

short call_ushort(unsigned short a)  {
   printf("call_ushort(%d)", a);
   return a+1;
}


short call_shorts(short a, short b, short c, short d, short e, short f,
	      short g, short h, short i, short j, short k, short l,
	      short m, short n, short o, short p, short q, short r,
	      short s, short t, short u, short v, short w, unsigned short x
	      )  {
   
   printf("call_shorts(%d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d)",
	  a,b,c,d,e,f,g,h,j,i,k,l,m,n,o,p,q,e,r,s,t,u,v,w,x);
   
   return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x;
}

char call_char(char a)  {
   printf("call_char(%d)", a);
   return a*a;
}

char call_uchar(unsigned char a)  {
   printf("call_uchar(%d)", a);
   return a+1;
}


char call_chars(char a, char b, char c, char d, char e, char f,
	      char g, char h, char i, char j, char k, char l,
	      char m, char n, char o, char p, char q, char r,
	      char s, char t, char u, char v, char w, unsigned char x
	      )  {
   
   printf("call_chars(%d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d)",
	  a,b,c,d,e,f,g,h,j,i,k,l,m,n,o,p,q,e,r,s,t,u,v,w,x);
   
   return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x;
}

int call_mixed(long a, char b, int c, unsigned int d, short e,
	       char f, long g, char h, unsigned long i, char j,
	       int k, int l, unsigned int m, unsigned char n, unsigned short o,
	       int p, unsigned short q, char r, int s, int t, unsigned char u,
	       int v, int w, char x, long y, long z
	      )  {
   
   printf("call_mixed(%d, %d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d,%d, %d, %d, %d, %d, %d, %d)",
	  a,b,c,d,e,f,g,h,j,i,k,l,m,n,o,p,q,e,r,s,t,u,v,w,x,y,z);
   
   return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z;
}

s1 recurse_struct(s1 s) {
   
   count++;
   s.a--;
   s.b--;
   s.c--;
   s.d += 2;
   s.e += 3;
   s.f += 4;   
   if (count < 10) {
      printf("recurse(");
      s1 r =  recurse_struct(s);
      printf(")");
      return r;
   } else {
      printf("recurse()");
      return s;
   }
}



int call_struct(s1 s) {
   printf("call_struct(s):\n");
   printf("    s.a: %d:\n", s.a);
   printf("    s.b: %d:\n", s.b);
   printf("    s.c: %d:\n", s.c);
   printf("    s.d: %d:\n", s.d);
   printf("    s.e: %d:\n", s.e);
   printf("    s.f: %d:\n", s.f);
   printf("    *s.g.a:%d\n", *(s.g.a));
   printf("    *s.g.b:%d\n", *(s.g.b));
   printf("    *s.g.c:%d\n", *(s.g.c));
   printf("    *s.g.u.a:%d\n", s.g.u.a);
   printf("    *s.g.u.b[0]:%d\n", s.g.u.b[0]);
   printf("    *s.g.u.b[1]:%d\n", s.g.u.b[1]);
   return s.a + *(s.g.a) + s.g.u.a;
}



int main() {
   printf(":%d\n", call_int(-123));
   printf(":%d\n", call_uint(3123123123u));
   printf(":%d\n",
	  call_ints(1,-2,3,-4,5, -6,
		   123456789, 987654321u, 0, -123, 123, 100,
		   1,2,3,4,5, 6,
		   -1, -10, -100, -10000, -1000000, 3123456789u));

   printf(":%d\n", call_short(-123));
   printf(":%d\n", call_ushort(65000u));
   printf(":%d\n",
	  call_shorts(1,-2,3,-4,5, -6,
		   12345, 24321, 0, -123, 123, 100,
		   1,2,3,4,5, 6,
		   -1, -10, -100, -1000, -10000, 65000u));

   printf(":%d\n", call_char(-123));
   printf(":%d\n", call_uchar(255u));
   printf(":%d\n",
	  call_chars(1,-2,3,-4,5, -6,
		   100, 120, 0, -123, 123, 89,
		   1,2,3,4,5, 6,
		   -1, -10, -100, -111, -12, 234u));

   printf(":%d\n",
	  call_mixed(1000000000, -12, -1234, 200, -20000,
		     123, -12345678, -1, 0, 0,
		     -89, 1234, 99999, 1, 12345,
		     123, 60000u, 12,9999999, -123, 0,
		     12359, -999, 0, 0, 1000000));

   volatile u1 bas; bas.a = 123456789;
   volatile s2 bar =  {&a, &b, &c, bas };
   volatile s1 foo =  { -123, 123456789, 10000, 255u, 3123456789u, 65000u,  bar };
   printf("------\n%d\n\n", call_struct(foo));
   s1 s = recurse_struct(foo);
   printf("\n------\n%d\n", call_struct(s));
}
