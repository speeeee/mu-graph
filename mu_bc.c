#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define INT 0
#define FLT 1
#define STR 2
#define OPC 3

#define LOOP 4
#define CALL 5
#define SET  6
#define GET  7
#define IFC  8
#define NOT  9
#define LIBR 10
#define SYMR 11

#define ADD 0

// [file] : index of data file (-1 for base file)

typedef double (*F)(double *, double *)
typedef struct { union { int64_t i; double f; char x; char *s; } x; 
  unsigned int type; } Lit;

double add(double *a, double *b) { return a[0]+b[0]; }
double nothing(double *a, double *b) { return 0; }

double **dat;

Lit tok(FILE *in) { Lit l; int c = fgetc(in); /*printf("%i\n",c);*/ switch(c) {
  case INT: fread(&l.x.i,sizeof(int64_t),1,in); l.type = INT; break;
  case FLT: fread(&l.x.f,sizeof(double),1,in); l.type = FLT; break;
  case STR: { int64_t i; fread(&i,sizeof(int64_t),1,in); 
    l.x.s = malloc((i+1)*sizeof(char)); fread(&l.x.s,sizeof(char),i,in);
    l.x.s[i] = '\0'; l.type = STR; break; }
  case EOF: l.x.i = 0; l.type = END;
  default: l.x.x = c; l.type = OPC; } return l; }
void lexer(FILE *in, FILE **data) { Lit l; 
  for(int i=0;(l=tok(in)).type!=END;i++) {
    if(l.type == OPC) { op_code(l.x.c,in,data); }
void op_code(char op, FILE *in, FILE **data) { switch(op) {
  case LOOP: // LOOP: [file] [ind] [shp] [ele] [arg1] [arg2]
    double *q = malloc(6*sizeof(double));
    for(int i=0;i<6;i++) { q[i] = tok(in).x.i; } F f;
    if(q[0]==-1&&q[1]==ADD) { f = &add; } else { f = &nothing; }
    double *res = malloc(q[3]*sizeof(double));
    for(int i=0;i<q[2];i++) { double *a = malloc(q[3]*sizeof(double));
      double *b = malloc(q[3]*sizeof(double));
      for(int e=0;e<q[3];e++) { a[e] = dat[q[4]][i*q[3]+e];
        b[e] = dat[q[5]][i*q[3]+e]; }
      res[i] = (f)(a,b); } break;

int main(int argc, char **argv) {
