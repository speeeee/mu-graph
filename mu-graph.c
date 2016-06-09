#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <OpenGL/GL.h>
#include <GLFW/glfw3.h>

#define NUM 0
#define FUN 1
#define NIL 2

#define LINE 0
#define DUP  1
#define SAVE 2
#define LOAD 3
#define ADD  4
#define SUB  5
#define MUL  6
#define DIV  7
#define POW  8
#define LOG  9
#define CST  10 // constant reference

typedef struct { union { double n; int32_t c; } x; unsigned short type; } Lit;
typedef struct S { Lit x; struct S *adj; } S; //will be used as both stack and list.
S *stk;

void pop(void) { S *e = stk; stk = stk->adj; free(e); 
  if(!stk) { stk = malloc(sizeof(Stk)); stk->adj = NULL; stk->x.type = NIL; } }
void astk(Lit x) { if(stk->type==NIL) { stk->x = x; }
  else { S *e = malloc(sizeof(Stk)); e->adj = stk; stk = e; stk->n = x; } }

void error_callback(int error, const char* description) {
    fputs(description, stderr); }
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE); }
void rsz(GLFWwindow *win, int w, int h) {
  glViewport(0,0,w,h); float ratio = w/ (float) h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 2*ratio, 0, 2.f, 2.f, 0);
  glMatrixMode(GL_MODELVIEW); }

void setup(GLFWwindow *win) {
  float ratio;
  int width, height;
  glfwGetFramebufferSize(win, &width, &height);
  ratio = width / (float) height;
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 2*ratio, 0, 2.f, 2.f, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); }

Lit tok(FILE *in) { Lit l; int c = fgetc(in); /*printf("%i\n",c);*/ switch(c) {
  case FLT: fread(&l.x.n,sizeof(double),1,in); l.type = FLT; break;
  case FUN: fread(&l.x.c,sizeof(int64_t),1,in); l.type = FUN; break;
  case EOF: l.x.i = 0; l.type = END; } return l; }

/*void paint(GLFWwindow *win) { 
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glLoadIdentity();
  glClearColor(0.4,0.4,0.55,1.0); }*/
void parse_from_file(GLFWwindow *win, FILE *f) { Lit l;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glLoadIdentity();
  glClearColor(0,0,0,1); 
  while((l=tok(in)).type!=END&&!glfwWindowShouldClose(win)) {
    glSwapBuffers(win); glfwPollEvents(); }
  glfwDestroyWindow(win); glfwTerminate(); exit(EXIT_SUCCESS); }

int pressed(GLFWwindow *win,int x) { return glfwGetKey(win,x)!=GLFW_RELEASE; }
int mpressed(GLFWwindow *win, int x) { return glfwGetMouseButton(win,x); }
  
int main(void) { stk = malloc(sizeof(Stk)); stk->adj = NULL; stk->x.type = NIL;
  GLFWwindow* window; FILE *f = fopen("target.muo","rb");
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) exit(EXIT_FAILURE);
  window = glfwCreateWindow(800, 800, "µgraph", NULL, NULL);
  if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE); }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, key_callback); setup(window);
  glfwSetFramebufferSizeCallback(window, rsz); parse_from_file(window,f);
 /* while (!glfwWindowShouldClose(window)) { parse_from_file(window,f);
    glfwSwapBuffers(window); glfwPollEvents(); }*/
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS); return 0; }
