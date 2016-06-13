// Array processing utilities; loosely based off of languages like J.
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define INT 0
#define NUM 0
#define FLT 1
#define FUN 2
#define OPC 3

//list of opcodes
#define T 3 // parameters: {n : # dimensions} [d1 d2 .. dn : length of each dimension]
            //             (data ...) E
#define E 4 // end creation of an array

typedef struct { union { int64_t i; double f; } x; unsigned short type; } Num;
typedef struct { char *name; int *dcs; int *dcc; } Fun;
typedef struct { char *name; int x; int y; } F;
typedef struct { int dc; int *dims; Num *data; } NArr;
typedef struct { int dc; int *dims; Fun *data; } FArr;

typedef struct { Fun f; Num n; char op; char type; } Item;

F funs[4] = { { "add", 0, 0 }, { "sub", 0, 0 }, { "mul", 0, 0 }, { "div", 0, 0 } };

int64_t pi_mul(int *dims,int dc) { int64_t q = 1; for(int i=0;i<dc;i++) { q*=dims[i]; }
  return q; }

Item tok(FILE *in) { Item l; int c = fgetc(in); /*printf("%i\n",c);*/ switch(c) {
  case INT: { Num n; fread(&n.x.i,sizeof(int64_t),1,in); n.type = INT; l.n = n;
    l.type = NUM; break; }
  case FLT: { Num n; fread(&n.x.f,sizeof(double),1,in); n.type = FLT; l.n = n;
    l.type = NUM; break; }
  case FUN: { Fun f; int32_t q; fread(&q,sizeof(int32_t),1,in);
    f.name = malloc((q+1)*sizeof(char)); fread(f.name,sizeof(char),q,in);
    f.name[q] = '\0'; l.f = f; l.type = FUN; break; }
  case EOF: l.type = END; break;
  default: l.c = c; l.type = OPC; } return l; }

NArr read_data(FILE *d) { Item l; NArr n; int curr = 0;
  while((l=tok(in)).type!=END) { if(l.type == OPC&&l.c == T) { n.dc = tok(in).n.x.i;
    n.dims = malloc(n.dc*sizeof(int)); for(int i=0;i<n.dc;i++) { 
      n.dims[i] = tok(in).n.x.i; } n.data = malloc(pi_mul(n.dims,n.dc)); }
    if(l.type==NUM) { n.data[curr++] = l.n; } } }

double getf(Num q) { if(q.type==INT) { return (double)q.x.i; }
  else { return q.x.f; } }

NArr add_b(NArr a, NArr b) { // + 0 0
  NArr c; c.dc = 1; c.dims = a.dims; c.data = malloc(c.dims[0]*sizeof(Num));
  for(int i=0;i<c.dims[0];i++) { c.data[i] = Num { getf(a.data[i])+getf(b.data[i]), FLT }; }
  return c; }
NArr sub_b(NArr a, NArr b) { // - 0 0
  NArr c; c.dc = 1; c.dims = a.dims; c.data = malloc(c.dims[0]*sizeof(Num));
  for(int i=0;i<c.dims[0];i++) { c.data[i] = Num { getf(a.data[i])-getf(b.data[i]), FLT }; }
  return c; }
NArr mul_b(NArr a, NArr b) { // * 0 0
  NArr c; c.dc = 1; c.dims = a.dims; c.data = malloc(c.dims[0]*sizeof(Num));
  for(int i=0;i<c.dims[0];i++) { c.data[i] = Num { getf(a.data[i])*getf(b.data[i]), FLT }; }
  return c; }
NArr div_b(NArr a, NArr b) { // / 0 0
  NArr c; c.dc = 1; c.dims = a.dims; c.data = malloc(c.dims[0]*sizeof(Num));
  for(int i=0;i<c.dims[0];i++) { c.data[i] = Num { getf(a.data[i])/getf(b.data[i]), FLT }; }
  return c; }

// return size of nth dimension.
int sel(int a, Num n) { return n.dc-1-a; }

NArr appf(NArr *ar, Fun f) { NArr cu; NArr a; NArr b;
  // 6 5 (+"_ 0) 0 1 2 3 -> 6 5 + 0  6 5 + 1  6 5 + 2  6 5 + 3
  // +"0 1
  // allocate space for rank of array by allocating π(rank_a --> rank_b)
  a.dims = malloc(pi_mul(&ar[0].dims[sel(f.dcs[0],ar[0])],f.dcs[0]+1)*sizeof(int));
  b.dims = malloc(pi_mul(&ar[1].dims[sel(f.dcs[1],ar[1])],f.dcs[1]+1)*sizeof(int));
  for(int i=0;
