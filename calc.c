#include"calc.h"





stack *exprs; 
int main(int argc,char **argv){
  stack *output = init();
  
  exprs = init();
  pass1("(123+8)/2+3^3");
 
  reverse(exprs);
  show(exprs);
  pass2(output);
  reverse(output);
  show(output);
  calc(output);
  free_stack(exprs);
  free_stack(output);
  
}

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
    switch(tmp->type){
    case LEFT_BRACE:printf("left_brace\t");break;
    case RIGHT_BRACE:printf("right_brace\t");break;
    case OPERATOR:printf("operator\t");break;
    case VALUE:printf("value\t");break;
    }
    switch(tmp->data_type){
    case D_INT:printf("int\t");break;
    case D_FLOAT:printf("float\t");break;
    case D_NAME:printf("name\t");break;
    case D_BOOL:printf("bool\t");break;
    }
    printf("%s \n",tmp->token);
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
int stack_size(stack *head){
  int i=0;
  stack *p=head;
  while(p->next != NULL){
    i++;
    p = p->next;
  }
  return i;
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
	push_stack(pop(oper),output);
	//show(output);
	//	o2 = NULL;
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
      num = check_oper(p->token)->opers;
      printf("size = %d\n",stack_size(result));
      if(num <= stack_size(result) ){
	for(int i=num-1;i>=0 ;i--){
	  arg[i] = pop(result);
	  printf("[%d]%s\n",i,arg[i]->token);
	}
	p = check_oper(p->token)->fun(arg);
	push_stack(p,result);
		   
	
      }else{
	
      }
      break;
    }
  }
  printf("result\n");
  show(result);
  free_stack(result);
}

stack *calc_plus(stack **arg){
  if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  double a = atof(arg[0]->token);
  double b = atof(arg[1]->token);
  double v = a + b;
  int v1 = (int)v;
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    if(v1 < v){
      arg[0]->data_type = D_FLOAT;
      memset(arg[0]->token,0,30);   
      gcvt(v,4,arg[0]->token);
    }else{
      arg[0]->data_type = D_INT;
      memset(arg[0]->token,0,30);
      sprintf(arg[0]->token,"%d",v1);
    }
  }else{
    
  }
  return arg[0];
}
stack *calc_sub(stack **arg){
  if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  double a = atof(arg[0]->token);
  double b = atof(arg[1]->token);
  double v = a - b;
  int v1 = (int)v;
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    if(v1 < v){
      arg[0]->data_type = D_FLOAT;
      memset(arg[0]->token,0,30);   
      gcvt(v,4,arg[0]->token);
    }else{
      arg[0]->data_type = D_INT;
      memset(arg[0]->token,0,30);
      sprintf(arg[0]->token,"%d",v1);
    }
  }else{
    
  }
  return arg[0];
}
stack *calc_div(stack **arg){
  if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  double a = atof(arg[0]->token);
  double b = atof(arg[1]->token);
  double v = a / b;
  int v1 = (int)v;
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    if(v1 < v){
      arg[0]->data_type = D_FLOAT;
      memset(arg[0]->token,0,30);   
      gcvt(v,4,arg[0]->token);
    }else{
      arg[0]->data_type = D_INT;
      memset(arg[0]->token,0,30);
      sprintf(arg[0]->token,"%d",v1);
    }
  }else{
    
  }
  return arg[0];
}
stack *calc_div_int(stack **arg){
  
}
stack *calc_times(stack **arg){
  if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  double a = atof(arg[0]->token);
  double b = atof(arg[1]->token);
  double v = a * b;
  int v1 = (int)v;
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    if(v1 < v){
      arg[0]->data_type = D_FLOAT;
      memset(arg[0]->token,0,30);   
      gcvt(v,4,arg[0]->token);
    }else{
      arg[0]->data_type = D_INT;
      memset(arg[0]->token,0,30);
      sprintf(arg[0]->token,"%d",v1);
    }
  }else{
    
  }
  return arg[0]; 
}
stack *calc_power(stack **arg){
   if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  double v = 1;
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      arg[1]->data_type == D_INT ){
    double a = atof(arg[0]->token);
    int b = atoi(arg[1]->token);
    for(int i=0;i<b;i++){
      v = v * a;
    }
    int v1 = (int)v;
    if(v1 < v){
      arg[0]->data_type = D_FLOAT;
      memset(arg[0]->token,0,30);   
      gcvt(v,4,arg[0]->token);
    }else{
      arg[0]->data_type = D_INT;
      memset(arg[0]->token,0,30);
      sprintf(arg[0]->token,"%d",v1);
    }
  }else{
    
  }
  return arg[0];
}
stack *calc_gt(stack **arg){
  
}
stack *calc_lt(stack **arg){
  
}

