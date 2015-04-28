struct w {
    int n;
    int v;
    int h;
};
struct a {
    int v;
    int s;
};
static void f(
    int q,
    const struct w *s,
    struct a *l)
{
    int f = ((s->n * q) +
	     (s->v * l->v) -
	     (s->h * l->s));
    l->v = q;
    l->s = f;
}
static const struct w o = {
    .v = 1
};
unsigned int p;
struct a c;
int h;
int d;
unsigned int u;
int k;
volatile unsigned int v, w;
int y;
int k;
void * l;
int x;
int main() {
    l = &&z;
    while (1) {
        p = w;
        f(p, &o, &c);
        int z = c.s;
        int q, a;
        goto *l; 
    z:
        if (--x != 0)
            l = &&x;
    x:
        continue;
    q:
        y += k;
        unsigned int  c = v;
        switch (c) {
        case 1:
            l = &&q;
        }
    }
}
