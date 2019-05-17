'use strict';

function myCalc(expr, v){
    this.s_idle = 1;
    this.s_string_double = 2;
    this.s_string_single = 3;
    this.s_number_int = 4;
    this.s_number_float = 5;
    this.s_name = 6;
    this.s_array = 7;
    this.s_comment = 8;
    this.c_prefix = '_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    this.c_digital = '0123456789';
    this.c_white_space = [' ','\r','\n'];
    this.c_comment = '#';
    this.c_operator_prefix = '!%^&*|<>()@+-=./';
    this.expr = expr;
    this.vars = v;
    this.debug = true;
    this.debug2 = true;
    this.error = false;
    this.operators = {
	'(' :{"priority":1,"dir":"ltr","opers":0},
	')' :{"priority":1,"dir":"ltr","opers":0},
	'.' :{"priority":1,"dir":"ltr","opers":2,"calc":this.calc_dot},
	'^' :{"priority":3,"dir":"rtl","opers":2,"calc":this.calc_power},
	'*' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_times},
	'/' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_div},
	'%' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_div_int},
	'+' :{"priority":4,"dir":"ltr","opers":2,"calc":this.calc_plus},
	'-' :{"priority":4,"dir":"ltr","opers":2,"calc":this.calc_sub},
	'-n':{"priority":2,"dir":"rtl","opers":2,"calc":this.calc_neg},
	'==':{"priority":7,"dir":"ltr","opers":2,"calc":this.calc_bool_eq},
	'!=':{"priority":7,"dir":"ltr","opers":2,"calc":this.calc_bool_ne},
	'>' :{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_bool_gt},
	'<' :{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_bool_lt},
	'>=':{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_bool_ge},
	'<=':{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_bool_le},
	'&&':{"priority":11,"dir":"ltr","opers":2,"calc":this.calc_bool_and},
	'||':{"priority":12,"dir":"ltr","opers":2,"calc":this.calc_bool_or},
	'in':{"priority":1,"dir":"ltr","opers":2,"calc":this.calc_array_in}
    };
    try{
	var tokens = this.pass1(this.expr,this.varsa);//解析算式
	this.rp = this.pass2(tokens);//调度场算法
    }catch(error){
	console.log(error);	
    }
}
myCalc.prototype.calc = function(v){
    if(!v){
	v = {};
    }
    this.vars = v;
    var result;
    try{
	if(this.debug2){console.log("\n",this.vars);}
	result = this.pass3(this.rp);
    }catch(e){
	console.log(e);
	result = {"data_type":"bool","token":"false"};
    }
    return result;
};
myCalc.prototype.get_var = function(token,obj){
    if(!obj){
	obj = this.vars;
    }
    var result;
    var data = obj[token.token];
    switch(typeof data){
    case "string":
	result = {"type":"value","data_type":"string","token":data};
	break;
    case "number":
	var re = new RegExp(/^(-|\+)?\d+$/);
	if(re.test(data.toString())){
	    result = {"type":"value","data_type":"int","token":data};
	}else{
	    result = {"type":"value","data_type":"float","token":data};
	}
	break;
    case "object":
	if(Array.isArray(data)){
	    result = {"type":"value","data_type":"array","token":data};
	}else{
	    result = {"type":"value","data_type":"object","token":data};
	}
	break;
    case "boolean":
	result = {"type":"value","data_type":"bool","token":data};
	break;
    default:
	result = {"type":"value","data_type":"undefine","token":"undefine"};
    }
    return result;
};
myCalc.prototype.calc_plus = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['float','int'].indexOf(t1.data_type) !== -1 && ['float','int'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 + v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else if(t1.data_type === 'string' || t2.data_type === 'string'){
	v1 = t1.token;
	v2 = t2.token;
	v = v1 + v2;
	result = {"type":"value","data_type":"string","token":v.toString()};
    }else{
	throw "data type error :" + t1.token + t2.token;
    }
    return result;
    
};
myCalc.prototype.calc_sub = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['float','int'].indexOf(t1.data_type) !== -1 && ['float','int'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 - v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else{
	throw "data type error :" + t1.token + t2.token;
    }
    return result;
    
};
myCalc.prototype.calc_times = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['float','int'].indexOf(t1.data_type) !== -1 && ['float','int'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 * v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else{
	throw "data type error :" + t1.token + t2.token;
    }
    return result;
    
};
myCalc.prototype.pass1 = function(expr){
    var stack = [];//存放解析出来的数据
    var token = '';
    var stat = this.s_idle;
    var pos = 0;
    while(pos<expr.length){
	var ch = expr[pos];
	switch(stat){
	case this.s_idle:
	    token = '';
	    if(this.c_white_space.indexOf(ch) !== -1){
		pos++;
	    }else if(this.c_digital.indexOf(ch) !== -1){
		token = ch;
		pos++;
		stat = this.s_number_int;
	    }else if(this.c_prefix.indexOf(ch) !== -1){
		token = ch;
		pos++;
		stat = this.s_name;
	    }else if(ch === '"'){
		pos++;
		stat = this.s_string_double;
	    }else if(ch === "'"){
		pos++;
		stat = this.s_string_single;
	    }else if(ch === '['){
		pos++;
		stat = this.s_array;
	    }else if(ch === '#'){
		pos++;
		stat = this.s_comment;
	    }else if(this.c_operator_prefix.indexOf(ch) !== -1){
		if(ch === '('){
		    stack.push({"type":"left_brace","token":ch});
		}else if(ch === ')'){
		    stack.push({"type":"right_brace","token":ch});
		}else{
		    var ch2 = expr[pos + 1];
		    if(this.operators[ch]){
			stack.push({"type":"operator","token":ch});
		    }else if(this.operators[ch+ch2]){
			stack.push({"type":"operator","token":ch+ch2});
			pos++;
		    }else{
			throw "undefind symbol" + ch;
		    }
		}
		if(ch === '-'){
		    var last_token = stack[stack.length - 2];
		    if(last_token.type === "operator"
		                 || last_token.type === "left_brace"){
			stack[stack.length - 1] = '-n';
		    }
		}
		pos++;
	    }else{
		throw "unexcept char"+ch;
	    }
	    break;
	case this.s_string_double:
	    if(ch === '"'){
		stack.push({"type":"value","data_type":"string","token":token});
		pos++;
		stat = this.s_idle;
	    }else{
		token = token + ch;
		pos++;
	    }
	    break;
	case this.s_string_single:
	    if(ch === "'"){
		stack.push({"type":"value","data_type":"string","token":token});
		pos++;
		stat = this.s_idle;
	    }else{
		token = token + ch;
		pos++;
	    }
	    break;
	case this.s_number_int:
	    if(ch === '.'){
		token = token + ch;
		pos++;
		stat = this.s_number_float;
	    }else if(this.c_digital.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else{
		stack.push({"type":"value","data_type":"int","token":token});
		stat = this.s_idle;
	    }
	    break;
	case this.s_number_float:
	    if(this.c_digital.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else{
		stack.push({"type":"value","data_type":"float","token":token});
		stat = this.s_idle;
	    }
	    break;
	case this.s_name:
	    if(this.c_prefix.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else if(this.c_digital.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else if(ch === '.'){
		stack.push({"type":"value","data_type":"name","token":token});
		stack.push({"type":"operator","token":ch});
		pos++;
		stat = this.s_idle;
	    }else if(['true','float'].indexOf(token) !== -1){
		stack.push({"type":"value","data_type":"bool","token":token});
		pos++;
		stat = this.idle;
	    }else if(token === 'in'){
		stack.push({"type":"operator","token":token});
		stat = this.s_idle;
	    }else{
		stack.push({"type":"value","data_type":"name","token":token});
		stat = this.s_idle;
	    }
	    break;
	case this.s_array:
	    if(ch === ']'){
		token = token.replace(/"/g,"");
		token = token.split(',');
		var i;
		for(i = 0;i<token.length; i++){
		    token[i] = token[i].trim();
		}
		stack.push({"type":"value","data_type":"array","token":token});
		pos++;
		stat = this.s_idle;
	    }else{
		token = token + ch;
		pos++;
	    }
	    break;
	case this.s_comment:
	    if(ch === '#'){
		stack.push({"type":"comment","token":token});
		pos++;
		stat = this.s_idle;
	    }else{
		token = token + ch;
		pos++;
	    }
	    break;
	}
    }
    if(this.debug){console.log(stack);}
    
    return stack; 
};
myCalc.prototype.pass2 = function(tokens){
    var token = '';
    var output = [];
    var stack = [];
    var pos = 0;
    while(pos<tokens.length){
	token = tokens[pos];
	switch(token.type){
	case "left_brace":
	    stack.push(token);
	    pos++;
	    break;
	case "right_brace":
	    token = stack.pop();
	    while( (stack.length > 0) && (token.type !== "left_brace") ){
		output.push(token);
		token = stack.pop();
	    }
	    if(token.type !== "left_brace"){
		throw this.expr+":  miss left barce "+ pos;
	    }
	    pos++;
	    break;
	case "value":
	    output.push(token);
	    pos++;
	    break;
	case "operator":
	    var o1 = token;
	    var o2 = null;
	    if(stack.length>0){
		o2 = stack[stack.length - 1];
	    }
	    if(o2){
		var p1 = this.operators[o1.token].priority;
		var p2 = this.operators[o2.token].priority;
		var o1_dir = this.operators[o1.token].dir;
	    }
	    while( (o2 && o2.type === 'operator') && ((o1_dir === 'ltr' && p1 >= p2) || (o1_dir === 'rtl' && p1 > p2)) ){
		output.push(stack.pop());
		if(stack.length > 0){
		    o2 = stack[stack.length - 1];
		    p2 = this.operators[o2.token].priority;
		}else{
		    o2 = null;
		}
	    }
	    stack.push(o1);
	    pos++;
	    break;
	}
    }
    while(stack.length > 0){
	 token = stack[stack.length - 1];
	 if(token.type ==='left_brace'){
	    throw "except error: unmatched barce" + this.expr; 
	}else{
	    output.push(stack.pop());
	}
    }
    
    return output.reverse();
    
};

myCalc.prototype.pass3 = function(expr){
    var result = [];
    var stack = [];
    var token = '';
    var pos = 0;
    while(expr.length>0){
	token = expr.pop();
	switch(token.type){
	case "value":
	    result.push(token);
	    break;
	case "operator":
	    if(result.length < this.operators[token.token].opers){
		throw "not enough argument" + token.token;
	    }else{
		var arg = [];
		var i;
		for(i=0;i<this.operators[token.token].opers;i++){
		    arg.push(result.pop());
		}
		if(this.debug2){console.log(arg,token.token);}
		var r = this.do_token_calc(token.token,arg);
		result.push(r);
	    }
	}
    }
    return result[0];
};
myCalc.prototype.do_token_calc = function(token,arg){
    arg = arg.reverse();
    return this.operators[token].calc.bind(this)(arg);
};


module.exports = myCalc;
