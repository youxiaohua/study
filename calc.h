#ifndef _CALC_H_
#define _CALC_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>
#include<ctype.h>


#define s_idle 1
#define s_array 2
#define s_number_int 3
#define s_number_float 4
#define s_name 5


#define LEFT_BRACE 0
#define RIGHT_BRACE 1
#define OPERATOR 2
#define VALUE 3

#define D_INT 10
#define D_FLOAT 11
#define D_NAME 12
#define D_BOOL 13

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
struct op{
  char ch[3];
  int priority;
  int dir;
  int opers;
  stack *(*fun)(stack **);
}oper[10]={
  {"(", 1, 0, 0,NULL},
  {")", 1, 0, 0,NULL},
  {"*", 3, LTR, 2, calc_times},
  {"/", 3, LTR, 2, calc_div},
  {"%", 3, LTR, 2, calc_div_int},
  {"^", 3, RTL, 2, calc_power},
  {"+", 4, LTR, 2, calc_plus},
  {"-", 4, LTR, 2, calc_sub},
  {">", 6, LTR, 2, calc_gt},
  {"<", 6, LTR, 2, calc_lt}
};
 /*
   {".", 1, LTR, 2},
   {"in",2, LTR, 2},
   {">=",6, LTR, 2},
   {"<=",6, LTR, 2},
   {"==",7, LTR, 2},
   {"!=",7, LTR, 2},
   {"&&",11, LTR, 2},
   {"||",12, LTR, 2}
 */

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
struct op *check_oper(char *ch);





#endif
