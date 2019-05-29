#include"calc.h"


stack *init(){
  stack *head =(stack *)malloc(sizeof(stack));
  //  head->type = NULL;
  //head->data_type = NULL;
  //head->token = NULL;
  head->next = NULL;
  return head;
  
}

void push(char *type,char *data_type,char *token,stack *head){
  stack *new = (stack *)malloc(sizeof(stack));
  strcpy(new->type,type);
  if(data_type != NULL){
    strcpy(new->data_type , data_type);
  }
  strcpy(new->token , token);
  new->next = NULL;
  add_list(new,head);  
}
stack *pop(stack *head){
  stack *tmp = head->next;
  head->next = tmp->next;
  return tmp;
}
void reverse(stack *head){
  stack *tmp,*ima;
  ima = head->next;
  while(ima->next != NULL){
    tmp = ima->next;
    ima->next = tmp->next;
    tmp->next = head->next;
    head->next = tmp;
  }
}
void add_list(stack *new,stack *head){
  if(head->next == NULL){
    head->next = new;
  }else{
    new->next = head->next;
    head->next = new;
  }
}
void free_stack(stack *head){
  stack *tmp;
  tmp = head->next;
  while(tmp != NULL){
    head->next = tmp->next;
    free(tmp);
    tmp = head->next;
  }
  free(head);
}
void show(stack *head){
  stack *tmp=head->next;
  while(tmp != NULL){
    printf("%s\n",tmp->token);
    tmp = tmp->next;
  }
}
struct op *check_oper(char *ch){
  int size = sizeof(oper)/sizeof(struct op);
  int i=0;
  while(i<size){
    if(strcmp(oper[i].ch,ch) == 0){
      return &oper[i];
    }
    i++;
  }
  return NULL;
}
stack *exprs;
stack *output;
stack *result;
int main(int argc,char **argv){
  exprs = init();
  pass1("(1+2)*3");
  show(exprs);
}

void pass1(char *expr){
  char ch;
  printf("读取到%s\n",expr);
  char token[20] = {0};
  int len = strlen(expr);
  int pos=0;
  int num=0;
  int stat = s_idle;
  while(pos<len){
    ch = expr[pos];
    printf("ch = [%c] token = [%s]  num = %d  pos = %d\n",ch,token,num,pos);
    switch(stat){
    case s_idle:  
      if(ch == ' ' || ch == '\n'){
	pos++;
      }else if(isalpha(ch)){
	token[num]=ch;
	pos++;
	num++;
	stat = s_name;
      }else if(isdigit(ch)){
	token[num]=ch;
	pos++;
	num++;
	stat = s_number_int;
      }else if(ch == '['){
	pos++;
	stat = s_array;
      }else if(memchr(c_operator_prefix,ch,strlen(c_operator_prefix))){
	printf("[%c]\n",ch);
	if(ch == '('){
	  push("left_brace",NULL,&ch,exprs);
	}else if(ch == ')'){
	  push("right_brace",NULL,&ch,exprs);
	}else{
	  char ch2[2] = {expr[pos],expr[pos+1]};
	  printf("------%s\n",ch2);
	  if(check_oper(&ch)){
	    printf("%d\n",__LINE__);
	    push("operator",NULL,&ch,exprs);
	  }else if(check_oper(ch2)){
	    printf("%d\n",__LINE__);
	    push("operator",NULL,ch2,exprs);
	    pos++;
	  }
     	}
      }else{
	fprintf(stderr,"undefind char\n");
	return;
      }
      pos++;
      break;
    case s_number_int:
      if(ch == '.'){
	token[num] = ch;
	pos++;
	num++;
	stat = s_number_float;
      }else if(isdigit(ch)){
	token[num] = ch;
	pos++;
	num++;
      }else{
	push("value","int",token,exprs);
	num=0;
	memset(token,0,20);
	stat = s_idle;
      }
      break;
    case s_number_float:
      if(isdigit(ch)){
	token[num]=ch;
	num++;
	pos++;
      }else{
	push("value","float",token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }
      break;
    case s_name:
      if(isalpha(ch)){
	token[num] = ch;
	num++;
	pos++;
      }else if(ch == '.'){
	push("value","name",token,exprs);
	push("operator",NULL,&ch,exprs);
	num=0;
	pos++;
	stat = s_idle;
	memset(token,0,20);
      }else if(strstr("true false",token)){
	push("value","bool",token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }else if(strstr("in",token)){
	push("operator",NULL,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }else{
	push("value","name",token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }
      break;

    }
  }

}
