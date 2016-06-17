// Array processing utilities; loosely based off of languages like J.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define INT 0
#define NUM 0
#define FLT 1
#define FUN 2
#define OPC 3
#define END 4

//list of opcodes
#define T 3 // parameters: {n : # dimensions} [d1 d2 .. dn : length of each dimension]
            //             (data ...) E
#define E 4 // end creation of an array

typedef struct Dim { int d; struct Dim *prev; } Dim;

typedef struct { union { int64_t i; double f; } x; unsigned short type; } Num;
typedef struct { char *name; int x; int y; } F;
typedef struct { Dim *boxd; Dim *dims; Num *data; } NArr;
typedef NArr (*Dyad)(NArr,NArr);
typedef struct { char *name; Dyad f; Dim *dcs; } Fun;
typedef struct { Dim *boxd; Dim *dims; Fun *data; } FArr;

typedef struct { Fun f; Num n; char op; char type; } Item;

int64_t pi_mul(int *dims,int dc) { int64_t q = 1; for(int i=0;i<dc;i++) { q*=dims[i]; }
  return q; }
int64_t pi_mull(Dim *d) { int64_t q = 1; while(d) { q*=d->d; d = d->prev; }
  return q; }
int max(int a, int b) { return a>b?a:b; }

Dim *pd(Dim *q, int f) { if(!q) { q = malloc(sizeof(Dim)); q->d = f; q->prev = NULL; }
  else { Dim *e = malloc(sizeof(Dim)); e->d = f; e->prev = q; q = e; } return q; }

Item tok(FILE *in) { Item l; int c = fgetc(in); /*printf("%i\n",c);*/ switch(c) {
  case INT: { Num n; fread(&n.x.i,sizeof(int64_t),1,in); n.type = INT; l.n = n;
    l.type = NUM; break; }
  case FLT: { Num n; fread(&n.x.f,sizeof(double),1,in); n.type = FLT; l.n = n;
    l.type = NUM; break; }
  case FUN: { Fun f; int32_t q; fread(&q,sizeof(int32_t),1,in);
    f.name = malloc((q+1)*sizeof(char)); fread(f.name,sizeof(char),q,in);
    f.name[q] = '\0'; l.f = f; l.type = FUN; break; }
  case EOF: l.type = END; break;
  default: l.op = c; l.type = OPC; } return l; }

NArr read_data(FILE *in) { Item l; NArr n; n.boxd = NULL; n.dims = NULL; int curr = 0;
  while((l=tok(in)).type!=END) { if(l.type == OPC&&l.op == T) { int dc = tok(in).n.x.i;
    for(int i=0;i<dc;i++) { n.dims = pd(n.dims,tok(in).n.x.i); } 
    n.data = malloc(pi_mull(n.dims)*sizeof(Num)); }
    else if(l.type==NUM) { n.data[curr++] = l.n; } } return n; }

NArr nr(void) { NArr a; a.dims = NULL; a.boxd = NULL; return a; }

// returns the shape of an array of [dc]-cells.
int shp(NArr a) { if(a.dims) { return pi_mull(a.dims); } else { return 1; } }
// returns amount of elements each [dc]-cell. Amt of elements in boxed side.
int ele(NArr a) { if(a.boxd) { return pi_mull(a.boxd); } else { return 1; } }

double getf(Num q) { if(q.type==INT) { return (double)q.x.i; }
  else { return q.x.f; } }
Dim *fetch(Dim *x, int q) { Dim *e = x; 
  for(int i=0;i<q;i++) { e = e->prev; } return e; }
Dim *bget(Dim *boxd) { if(boxd) { return boxd; } else { Dim *x = malloc(sizeof(Dim));
  x->d = 1; return x; } }
// rank
int sz(Dim *x) { Dim *e = x; int i; for(i=0;e;++i) { e=e->prev; } return i; }
Dim *dcopy(Dim *a, Dim *b) { Dim *e = a; while(b) { Dim *f = malloc(sizeof(Dim));
  f->d = e->d; f->prev = a; a = f; b = b->prev; } return e; }
/*NArr pad(NArr a, NArr b) { int sa = shp(a); int sb = shp(b);
  if(sa!=sb) { NArr x; NArr n = max(sa,sb); NArr m = min(sa,sb); 
    x.dims = NULL; x.dims = dcopy(x.dims,m.dims); x.dims->d = n.dims->d;
    x.data = malloc(pi_mull(x));
    for(int i=0;i<pi_mull(x);i++) { x.data[i] = a.data[i%pi_mull(a.dims)]; }
    else { NArr x = b; } return x; }*/
Dim *dr(int x, int y) { Dim *a = malloc(sizeof(Dim)); a->d = y;
  Dim *b = malloc(sizeof(Dim)); b->d = x; a->prev = b; return a; }

NArr self(NArr a, NArr b) { return b; }
// always assumes the rank of the second argument. data is cyclic.
NArr add_b(NArr a, NArr b) { // + 0 0
  NArr c = nr(); c.dims = b.dims; c.data = malloc(c.dims->d*sizeof(Num));
  for(int i=0;i<c.dims->d;i++) { c.data[i] = (Num) { 
    getf(a.data[i%shp(a)])+getf(b.data[i%shp(b)]), FLT }; }
  return c; }
NArr sub_b(NArr a, NArr b) { // - 0 0
  NArr c = nr(); c.dims = b.dims; c.data = malloc(c.dims->d*sizeof(Num));
  for(int i=0;i<c.dims->d;i++) { c.data[i] = (Num) { 
    getf(a.data[i%shp(a)])-getf(b.data[i%shp(b)]), FLT }; }
  return c; }
NArr mul_b(NArr a, NArr b) { // * 0 0
  NArr c = nr(); c.dims = b.dims; c.data = malloc(c.dims->d*sizeof(Num));
  for(int i=0;i<c.dims->d;i++) { c.data[i] = (Num) { 
    getf(a.data[i%shp(a)])*getf(b.data[i%shp(b)]), FLT }; }
  return c; }
NArr div_b(NArr a, NArr b) { // / 0 0
  NArr c = nr(); c.dims = b.dims; c.data = malloc(c.dims->d*sizeof(Num));
  for(int i=0;i<c.dims->d;i++) { c.data[i] = (Num) {
    getf(a.data[i%shp(a)])/getf(b.data[i%shp(b)]), FLT }; }
  return c; }
// box items from b into [a]-cells.
NArr boxr(NArr a, NArr b) { if(a.data[0].x.i) {
  NArr c = nr(); c.dims = fetch(b.dims,a.data[0].x.i);
  c.boxd = b.dims; Dim *q = fetch(c.boxd,a.data[0].x.i-1); q->prev = NULL; 
  c.data = b.data; return c; } else { return b; } }

Fun funs[5] = { { "add", add_b, NULL }, { "sub", sub_b, NULL }, 
  { "mul", mul_b, NULL }, { "div", div_b, NULL }, { "self", self, NULL } };

// return size of nth dimension.
int sel(int a, NArr n) { return fetch(n.dims,sz(n.dims)-1-a)->d; }
// cyclic indexing.
NArr ind(NArr e, int x) { NArr r = nr(); r.dims = bget(e.boxd);
  r.data = malloc(ele(r)*sizeof(Num));
  for(int i=0;i<ele(r);i++) { r.data[i] = e.data[x*ele(r)+i]; }
  r.boxd = NULL; return r; }

NArr mk_narri(int x) { NArr a = nr(); a.data = malloc(sizeof(Num));
  a.data[0].x.i = x; a.data[0].type = INT; a.dims = malloc(sizeof(Dim));
  a.dims->d = 1; a.boxd = NULL; return a; }

void appf_nostore_print(NArr *ar, Fun f) { NArr a; NArr b;
  // 6 5 (+"_ 0) 0 1 2 3 -> 6 5 + 0  6 5 + 1  6 5 + 2  6 5 + 3
  // +"0 1
  // allocate space for rank of array by allocating π(rank_a --> rank_b)
  //a.dims = malloc(pi_mul(&ar[0].dims[sel(f.dcs[0],ar[0])],f.dcs[0]+1)*sizeof(int));
  //b.dims = malloc(pi_mul(&ar[1].dims[sel(f.dcs[1],ar[1])],f.dcs[1]+1)*sizeof(int));
  NArr lr = mk_narri(fetch(f.dcs,0)->d); NArr rr = mk_narri(fetch(f.dcs,1)->d);
  a = boxr(lr,ar[0]); b = boxr(rr,ar[1]); //NArr r; r.dims = b.dims;
  //r.data = malloc(shp(r));
  //int m = shp(ar[1]);
  for(int i=0;i<shp(ar[1]);i++) {
     // amount of calls is equal to the target shape of the smaller side.
     NArr s = (f.f)(ind(a,i),ind(b,i));
     for(int q=0;q<shp(s)*ele(s);q++) {
       printf("%lld ",s.data[q].x.i); } printf("\n");
       /* result: shape is the same as b, but each unit is as large as boxd */ } }

int main(int argc, char **argv) { for(int i=0;i<5;i++) { funs[i].dcs = dr(0,0); }
  FILE *f = fopen("out.muo","rb"); NArr a = read_data(f);
  NArr b = mk_narri(3);
  NArr n[2] = { b, a }; appf_nostore_print(n,funs[4]);
  return 0; }
