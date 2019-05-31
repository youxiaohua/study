#include"calc.h"





stack *exprs = NULL; 
stack *output = NULL;
stack *result = NULL;
int main(int argc,char **argv){
  output = init(); 
  exprs = init();
  char expr[100];
  FILE *fp = fopen("./expr.txt","r");
  while(!feof(fp)){
    memset(expr,0,100);
    if(!fgets(expr,100,fp)){
      break;
    }
    pass1(expr);
    reverse(exprs);
    show(exprs);
    pass2(output);
    reverse(output);
    show(output);
    calc(output);
    printf("----------------------------\n\n");
  }
  // free_stack(exprs);
  // free_stack(output);
  
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
  if(head == NULL){
    return;
  }
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
    case LEFT_BRACE:printf("left_brace\t\t");break;
    case RIGHT_BRACE:printf("right_brace\t\t");break;
    case OPERATOR:printf("operator\t\t");break;
    case VALUE:printf("value\t");break;
    }
    switch(tmp->data_type){
    case D_INT:printf("\tint\t");break;
    case D_FLOAT:printf("\tfloat\t");break;
    case D_NAME:printf("\tname\t");break;
    case D_BOOL:printf("\tbool\t");break;
    case D_STRING:printf("\tstring\t");break;
    case D_ARRAY:printf("\tarray\t");break;
    }
    printf("%s \n",tmp->token);
    tmp = tmp->next;
  }
  printf("\n");
}
void go_wrong(char *err,stack *p){
  fprintf(stderr,err);
  if(result != NULL){free_stack(result);}
  if(output != NULL){free_stack(output);}
  if(exprs != NULL){free_stack(exprs);}
  free_stack(p);
  exit(0);
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
  char token[30] = {0};
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
      }else if(ch == '"'){
	pos++;
	stat = s_string;
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
	  if(check_oper(ch2)){
	    push(OPERATOR,0,ch2,exprs);
	    pos++;
	  }else if(check_oper(ch1)){
	    push(OPERATOR,0,ch1,exprs);
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
	memset(token,0,30);//对token里面的数据清零
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
	memset(token,0,30);
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
	memset(token,0,30);
      }else if(strcmp("true",token)==0 || strcmp("false",token)==0){
	push(VALUE,D_BOOL,token,exprs);
   	num=0;
	stat = s_idle;
	memset(token,0,30);
      }else if(strstr("in",token)){
	push(OPERATOR,0,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,30);
      }else{
	push(VALUE,D_NAME,token,exprs);
	num=0;
	stat = s_idle;
	memset(token,0,30);
      }
      break;
    case s_array:
      if(ch == ']'){
	push(VALUE,D_ARRAY,token,exprs);
	num=0;
	pos++;
	stat = s_idle;
	memset(token,0,30);
      }else{
	token[num]=ch;
	num++;
	pos++;
      }
      break;
    case s_string:
      if(ch == '"'){
	push(VALUE,D_STRING,token,exprs);
	num=0;
	pos++;
	stat = s_idle;
	memset(token,0,30);
      }else{
	token[num]=ch;
	num++;
	pos++;
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
	_op = check_oper(p->token);
	p1 = _op->priority;
	dir = _op->dir;
	_op = check_oper(o2->token);
	p2 = _op->priority;
      }
      while( o2!=NULL && o2->type == OPERATOR && ((p1 >= p2 && dir==LTR) || (p1 > p2 && dir==RTL)) ){
	push_stack(pop(oper),output);
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
      go_wrong("left_brace mismatch",oper);
    }
    push_stack(pop(oper),output);
  }
  free_stack(oper);
}
void calc(stack *output){
  result = init();
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
      if(num <= stack_size(result) ){
	for(int i=num-1;i>=0 ;i--){
	  arg[i] = pop(result);
	}
	p = check_oper(p->token)->fun(arg);
	push_stack(p,result);
      }else{
	fprintf(stderr,"not enough parameters");
	exit(0);
      }
      break;
    }
  }
  if(stack_size(result)>1){
    go_wrong("expr error\n",NULL);
  }
  show(result);
  free_stack(result);
}

stack *calc_plus(stack **arg){
  if(arg[0]->data_type == D_NAME){  }
  if(arg[1]->data_type == D_NAME){  }
				       
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){

    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    double v = a + b;
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
  }else if(arg[0]->data_type == D_STRING || arg[1]->data_type == D_STRING){
    arg[0]->data_type = D_STRING;
    memset(arg[0]->token,0,30);
    strcat(arg[0]->token,arg[1]->token);
  }else{
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
}
stack *calc_sub(stack **arg){
  if(arg[0]->data_type == D_NAME){  }
  if(arg[1]->data_type == D_NAME){  }

  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    double v = a - b;
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
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
}
stack *calc_div(stack **arg){
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}

  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){

    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    double v = a / b;
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
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
}
stack *calc_div_int(stack **arg){
  if(arg[0]->data_type == D_NAME){  }
  if(arg[1]->data_type == D_NAME){  }
  
  if( arg[0]->data_type == D_INT && arg[1]->data_type == D_INT){
    int a = atoi(arg[0]->token);
    int b = atoi(arg[1]->token);
    int v = a % b;
    arg[0]->data_type = D_INT;
    memset(arg[0]->token,0,30);
    sprintf(arg[0]->token,"%d",v);
    
  }else{
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
  
}
stack *calc_times(stack **arg){
  if(arg[0]->data_type == D_NAME){  }
  if(arg[1]->data_type == D_NAME){  }
  
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
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
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
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
}
stack *calc_gt(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){
    
  }
  if(arg[1]->data_type == D_NAME){
    
  }
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
                                    (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i > 0){
     arg[0]->data_type = D_BOOL;
     memset(arg[0]->token,0,30);   
     strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_lt(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
                                    (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i < 0){
     arg[0]->data_type = D_BOOL;
     memset(arg[0]->token,0,30);   
     strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_array_in(stack **arg){
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  if(arg[1]->data_type == D_ARRAY){
    char token[30]={0};
    int ofg = 0;//判断查找的状态
    arg[0]->data_type = D_BOOL;
    strcpy(token,arg[1]->token);
    char *p = strtok(token,",");
    while(p){
      if(strcmp(p,arg[0]->token)== 0){
	ofg = 1;
	break;
      }
      p = strtok(NULL,",");
    }
    memset(arg[0]->token,0,30);
    if(ofg){
      strcpy(arg[0]->token,"true");
    }else{
      strcpy(arg[0]->token,"false");
    }
  }else{
    go_wrong("data type error",NULL);
  }
  free(arg[1]);
  return arg[0];
}
stack *calc_le(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i <= 0){
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);   
    strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_ge(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i >= 0){
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);   
    strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_eq(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i == 0){
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);   
    strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_ne(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( (arg[0]->data_type == D_INT || arg[0]->data_type == D_FLOAT) &&
      (arg[1]->data_type == D_INT || arg[1]->data_type == D_FLOAT)){
    
    double a = atof(arg[0]->token);
    double b = atof(arg[1]->token);
    i = a-b;
  }else if(arg[0]->data_type == D_STRING && arg[1]->data_type == D_STRING){
    i = strcmp(arg[0]->token,arg[1]->token); 
  }else{
    go_wrong("data type error",NULL);
  }
  if(i != 0){
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);   
    strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_and(stack **arg){
  int i;
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( arg[0]->data_type == D_BOOL && arg[1]->data_type == D_BOOL ){
    int v1 = strcmp("true",arg[0]->token);
    int v2 = strcmp("true",arg[0]->token);
    i = v1 + v2;
  }else{
    go_wrong("data type error",NULL);
  }
  if(i == 0){
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);   
    strcpy(arg[0]->token,"true");
  }else{
    arg[0]->data_type = D_BOOL;
    memset(arg[0]->token,0,30);
    strcpy(arg[0]->token,"false");
  }
  free(arg[1]);
  return arg[0]; 
}
stack *calc_or(stack **arg){
  if(arg[0]->data_type == D_NAME){}
  if(arg[1]->data_type == D_NAME){}
  
  if( arg[0]->data_type == D_BOOL && arg[1]->data_type == D_BOOL ){
    int v1 = strcmp("true",arg[0]->token);
    int v2 = strcmp("true",arg[0]->token);
    if(v1 == 0 || v2 == 0){
      arg[0]->data_type = D_BOOL;
      memset(arg[0]->token,0,30);   
      strcpy(arg[0]->token,"true");
    }else{
      arg[0]->data_type = D_BOOL;
      memset(arg[0]->token,0,30);
      strcpy(arg[0]->token,"false");
    }
  }else{
    go_wrong("data type error",NULL);
  }
 
  free(arg[1]);
  return arg[0];
}

