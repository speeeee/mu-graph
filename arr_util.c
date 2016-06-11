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
typedef struct { char *name; int *dcs; } Fun;
typedef struct { int dc; int *dims; double *data; } NArr;
typedef struct { int dc; int *dims; Fun *data; } FArr;

typedef struct { Fun f; Num n; char op; char type; } Item;

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

//NArr read_data(FILE *d) { Item l;
//  while((l=tok(in)).type!=END;
