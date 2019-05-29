#ifndef _CALC_H_
#define _CALC_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<ctype.h>
#define s_idle 1
//#define s_string_double 2
//#define s_string_single 3
#define s_array 2
#define s_number_int 3
#define s_number_float 4
#define s_name 5

char c_operator_prefix[] =  "!%^&*+()|></=-";

typedef struct node{
  char type[20];
  char data_type[20];
  char token[30];
  struct node *next;
}stack;

struct op{
  char ch[2];
  int prority;
  char dir[10];
  int opers;
  // stack *(*fun)(stack **arg);
}oper[16]={
  {"(", 1, NULL, 0},
  {")", 1, NULL, 0},
  {".", 1, "ltr", 2},
  {"in",2, "ltr", 2},
  {"*", 3, "ltr", 2},
  {"/", 3, "ltr", 2},
  {"%", 3, "ltr", 2},
  {"^", 3, "rtl", 2},
  {"+", 4, "ltr", 2},
  {"-", 4, "ltr", 2},
  {">", 6, "ltr", 2},
  {"<", 6, "ltr", 2},
  {">=",6, "ltr", 2},
  {"<=",6, "ltr", 2},
  {"==",7, "ltr", 2},
  {"!=",7, "ltr", 2},
  {"&&",11, "ltr", 2},
  {"||",12, "ltr", 2}
};
void pass1(char *expr);
void show(stack *head);
void free_stack(stack *head);
void add_list(stack *new,stack *head);
void reverse(stack *head);
stack *pop(stack *head);
void push(char *type,char *data_type,char *token,stack *head);
stack *init();
#endif
