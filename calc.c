#include"calc.h"


stack *init(){
  stack *head =(stack *)malloc(sizeof(stack));
  head->next = NULL;
  return head;
  
}

void push(int type,int data_type,char *token,stack *head){
  stack *new = (stack *)malloc(sizeof(stack));
  new->type = type;
  if(data_type != 0){
    new->data_type = data_type;
  }
  strcpy(new->token , token);
  new->next = NULL;
  add_list(new,head);  
}
void push_stack(stack *p,stack *head){
  stack *new = (stack *)malloc(sizeof(stack));
  new->type = p->type;
  if(p->data_type != 0){
    new->data_type = p->data_type;
  }
  strcpy(new->token , p->token);
  new->next = NULL;
  add_list(new,head);
}
stack *pop(stack *head){
  stack *tmp = head->next;
  if(tmp == NULL){
    return NULL;
  }
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
    printf("%s ",tmp->token);
    tmp = tmp->next;
  }
  printf("\n");
}
struct op *check_oper(char *ch){
  int size = sizeof(oper)/sizeof(struct op);
  int i=0;
  
  while(i<size){
    if(strcmp(oper[i].ch,ch) == 0){
      // printf("--[%s]   \n",oper[i].ch);
      return &oper[i];
    }
    i++;
  }
  return NULL;
}
stack *exprs;

stack *result;

int main(int argc,char **argv){
  stack *output = init();
  
  exprs = init();
  pass1("(2 + 1) - 3 / 2");
 
  reverse(exprs);
  show(exprs);
  pass2(output);
  reverse(output);
  show(output);

  
}

void pass1(char *ep){
  char ch;
  char ch1[2]={0};
  char token[20] = {0};
  int len = strlen(ep);
  int pos=0;
  int num=0;
  int stat = s_idle;
  while(pos<=len){
    ch = ep[pos];
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
	if(ch == '('){
	  ch1[0]=ch;
	  push(LEFT_BRACE,0,ch1,exprs);
	}else if(ch == ')'){
	  ch1[0]=ch;
	  push(RIGHT_BRACE,0,ch1,exprs);
	}else{
	  ch1[0]=ch;
	  char ch2[3];
	  ch2[0]=ch;
	  ch2[1]=ep[pos+1];
	  ch2[2]=0;
	  if(check_oper(ch1)){
	    push(OPERATOR,0,ch1,exprs);
	  }else if(check_oper(ch2)){
	    push(OPERATOR,0,ch2,exprs);
	    pos++;
	  }
     	}
	pos++;
      }else{
	fprintf(stderr,"undefind char\n");
	return;
      }
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
	push(VALUE,D_INT,token,exprs);
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
	push(VALUE,D_FLOAT,token,exprs);
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
	push(VALUE,D_NAME,token,exprs);
	push(OPERATOR,0,&ch,exprs);
	num=0;
	pos++;
	stat = s_idle;
	memset(token,0,20);
      }else if(strstr("true false",token)){
	push(VALUE,D_BOOL,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }else if(strstr("in",token)){
	push(OPERATOR,0,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }else{
	push(VALUE,D_NAME,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,20);
      }
      break;

    }
  }

}
void pass2(stack *output){
  stack *p;
  
  stack *oper = init();
  int p1,p2,dir;
  struct op *_op;
  while(exprs->next !=NULL){
    p = pop(exprs);
    if(p->type == LEFT_BRACE){
      push_stack(p,oper);
    }else if(p->type == RIGHT_BRACE){
      while(1){
	p = pop(oper);
	if(p->type == LEFT_BRACE){
	  break;
	}
	push_stack(p,output);
      }
    }else if(p->type == VALUE){
      push_stack(p,output);
    }else if(p->type == OPERATOR){
      stack *o2 = oper->next;
      if(o2 != NULL){
	//	printf("p->token = %s\n",p->token);
	_op = check_oper(p->token);
	//	printf("[%s]\n",_op->ch);
	p1 = _op->priority;
	//	printf("p1 = %d\n",p1);
	dir = _op->dir;
	//	printf("o2->token = %s\n",o2->token);
	_op = check_oper(o2->token);
	//	printf("[%s]\n",_op->ch);
	p2 = _op->priority;
	//	printf("p2 = %d\n",p2);
      }
      while( o2!=NULL && o2->type == OPERATOR && ((p1 >= p2 && dir==LTR) || (p1 > p2 && dir==RTL)) ){
	push_stack(pop(oper),oper);
	show(output);
	o2 = oper->next;
	if(o2 != NULL){
	  _op = check_oper(o2->token);
	  p2 = _op->priority;
	}	
      }
      push_stack(p,oper);
     
    }
  }
  while(oper->next != NULL){
    p = oper->next;
    if(p->type == LEFT_BRACE){
      fprintf(stderr,"left_brace mismatch");
      exit(0);
    }
    push_stack(pop(oper),output);
  }
  free_stack(oper);
}
void calc(stack *output){
  stack *result = init();
  stack *p;
  int num;
  stack *arg[2];
  while(output->next != NULL){
    p = pop(output);
    switch(p->type){
    case VALUE:
      push_stack(p,result);
      break;
    case OPERATOR:
      num = check_oper(p->token)->priority;
      if(num <= (sizeof(result)/sizeof(stack)) ){
	for(int i=0;i<num;i++){
	  arg[i] = pop(result);
	}
	check_oper(p->token)->fun(arg);
	
      }else{
	
      }
      
      break;
    }
  }
  
}

stack *calc_plus(stack **arg){
  printf("++++++++++++++++\n");
}
stack *calc_sub(stack **arg){
  
}
stack *calc_div(stack **arg){
  
}
stack *calc_div_int(stack **arg){
  
}
stack *calc_times(stack **arg){
  
}
stack *calc_power(stack **arg){
  
}
stack *calc_gt(stack **arg){
  
}
stack *calc_lt(stack **arg){
  
}

