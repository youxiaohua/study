#ifndef _CALC_H_
#define _CALC_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<ctype.h>
#include<fcntl.h>


#define s_idle 1
#define s_array 2
#define s_number_int 3
#define s_number_float 4
#define s_name 5
#define s_string 6


#define LEFT_BRACE 0
#define RIGHT_BRACE 1
#define OPERATOR 2
#define VALUE 3

#define D_INT 10
#define D_FLOAT 11
#define D_NAME 12
#define D_BOOL 13
#define D_ARRAY 14
#define D_STRING 15
#define LTR 2
#define RTL 1

char c_operator_prefix[] =  "!%^&*+()|></=-";

typedef struct node{
  int type;
  int data_type;
  char token[30];
  struct node *next;
}stack;


stack *calc_plus(stack **arg);
stack *calc_power(stack **arg);
stack *calc_sub(stack **arg);
stack *calc_div(stack **arg);
stack *calc_div_int(stack **arg);
stack *calc_times(stack **arg);
stack *calc_gt(stack **arg);
stack *calc_lt(stack **arg);
stack *calc_array_in(stack **arg);
stack *calc_le(stack **arg);
stack *calc_ge(stack **arg);
stack *calc_eq(stack **arg);
stack *calc_ne(stack **arg);
stack *calc_and(stack **arg);
stack *calc_or(stack **arg);
struct op{
  char ch[3];
   int priority;
  int dir;
  int opers;
  stack *(*fun)(stack **);
}oper[17]={
  {"(", 1, 0, 0,NULL},
  {")", 1, 0, 0,NULL},
  {"*", 3, LTR, 2, calc_times},
  {"/", 3, LTR, 2, calc_div},
  {"%", 3, LTR, 2, calc_div_int},
  {"^", 3, RTL, 2, calc_power},
  {"+", 4, LTR, 2, calc_plus},
  {"-", 4, LTR, 2, calc_sub},
  {">", 6, LTR, 2, calc_gt},
  {"<", 6, LTR, 2, calc_lt},
  {"in",2, LTR, 2, calc_array_in},
  {">=",6, LTR, 2, calc_ge},
  {"<=",6, LTR, 2, calc_le},
  {"==",7, LTR, 2, calc_eq},
  {"!=",7, LTR, 2, calc_ne},
  {"&&",11, LTR, 2, calc_and},
  {"||",12, LTR, 2, calc_or}
};

stack *init();
void pass1(char *expr);
void pass2(stack *output);
void calc(stack *output);
void show(stack *head);
void free_stack(stack *head);
void add_list(stack *new,stack *head);
void reverse(stack *head);
stack *pop(stack *head);
void push(int type,int data_type,char *token,stack *head);
void push_stack(stack *p,stack *head);
void go_wrong(char *err,stack *p);
struct op *check_oper(char *ch);
int stack_size(stack *head);





#endif
