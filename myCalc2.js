'use strict';


function myCalc(expr,v){
    //状态
    this.s_idle = 1;
    this.s_string_double = 2;
    this.s_string_single = 3;
    this.s_number_int = 4;
    this.s_number_float = 5;
    this.s_name = 6;
    this.s_array = 7;
    this.s_comment = 8;
    
    //判断
    this.c_comment = '#';
    this.c_digital = '0123456789';
    this.c_prefix = '_abcdefghijklmnopqrstuvwsyzABCDEFGHIJKLMNOPQRSTUVWSYZ';
    this.c_space = [' ','\r','\n'];
    this.c_operator_prefix = '!%^&*|/<>+-=()';
    this.operators = {
	'(' :{"priority":1,"dir":"ltr","opers":0},
	')' :{"priority":1,"dir":"ltr","opers":0},
	'.' :{"priority":1,"dir":"ltr","opers":2,"calc":this.calc_dot},
	'in':{"priority":2,"dir":"ltr","opers":2,"calc":this.calc_array_in},
	'-n':{"priority":2,"dir":"rtl","opers":1,"calc":this.calc_neg},
	'*' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_times},
	'/' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_div},
	'%' :{"priority":3,"dir":"ltr","opers":2,"calc":this.calc_div_int},
	'^' :{"priority":3,"dir":"rtl","opers":2,"calc":this.calc_power},
	'+' :{"priority":4,"dir":"ltr","opers":2,"calc":this.calc_plus},
	'-' :{"priority":4,"dir":"ltr","opers":2,"calc":this.calc_sub},
	'>' :{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_gt},
	'<' :{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_lt},
	'>=':{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_ge},
	'<=':{"priority":6,"dir":"ltr","opers":2,"calc":this.calc_le},
	'==':{"priority":7,"dir":"ltr","opers":2,"calc":this.calc_eq},
	'!=':{"priority":7,"dir":"ltr","opers":2,"calc":this.calc_ne},
	'&&':{"priority":11,"dir":"ltr","opers":2,"calc":this.calc_and},
	'||':{"priority":12,"dir":"ltr","opers":2,"calc":this.calc_or}
    };
    this.expr = expr;
    this.vars = v;
    this.debug = true;
    this.rp = null;
    try{
	var tokens = this.pass1(expr);
	this.expr = this.pass2(tokens);
    }catch(e){
	console.log(e);
    }
    
}
myCalc.prototype.calc = function(v){
    if(!v){v={};}
    this.vars = v;
    var result;
    try{
	result = this.pass3(this.expr);
    }catch(e){
	result = {"type":"error","token":e};
    }
    return result;
};
myCalc.prototype.calc_dot = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t1.data_type === 'object' && t2.data_type === 'name'){
	result = this.get_var(t2,t1.token);
    }else{
	throw "error data type  :" + t1.token + t2.token;
    }
    if(this.debug){console.log(t1.token,' . ',t2.token,' = ',result.token);}
    return result;
};
myCalc.prototype.calc_array_in = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['int','float','string'].indexOf(t1.data_type) !== -1 && t2.data_type === 'array'){
	v1 = t1.token;
	v2 = t2.token;
	v = v2.indexOf(v1);
	if(v < 0 ){
	    v1 = parseFloat(v1);
	    v = v2.indexOf(v1);
	}
	if(v < 0){
	    v1 = v1.toString();
	    v = v2.indexOf(v1);
	}
	v = v >= 0;
	result = {"type":"value","data_type":"bool","token":v.toString()};
    }else{
	throw "error data type  :" + t1.token + t2.token;
    }
    if(this.debug){console.log(v1,' in ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_neg = function(arg){
    var t1 = arg[0];
    var v,v1,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(['int','float'].indexOf(t1.data_type) !== -1 ){
	v1 = parseFloat(t1.token);
	v = v1 * -1;
	if(t1.data_type === 'int'){
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}
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
    if(['int','float'].indexOf(t1.data_type) !== -1 && ['int','float'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 * v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' * ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_div = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['int','float'].indexOf(t1.data_type) !== -1 && ['int','float'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 / v2;
	var re = new RegExp(/^(-|\+)?\d+$/);
	if(re.test(v.toString())){
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' / ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_div_int = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(t1.data_type === 'int' && t2.data_type === 'int'){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 % v2;
	result = {"type":"value","data_type":"int","token":v.toString()};
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' % ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_power = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(['int','float'].indexOf(t1.data_type) !== -1 && t2.data_type === 'int'){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = 1;
	var i;
	for(i = 0; i < v2;i++ ){
	    v = v1 * v;
	}
	if(t1.data_type === 'int'){
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' ^ ',v2,' = ',v );}
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
    if(['int','float'].indexOf(t1.data_type) !== -1 && ['int','float'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 + v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else if(t1.data_type === 'string' || t2.data_type === 'string'){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
	v = v1 + v2;
	result = {"type":"value","data_type":"string","token":v.toString()};
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' + ',v2,' = ',v );}
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
    if(['int','float'].indexOf(t1.data_type) !== -1 && ['int','float'].indexOf(t2.data_type) !== -1){
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
	v = v1 - v2;
	if(t1.data_type === 'float' || t2.data_type === 'float'){
	    result = {"type":"value","data_type":"float","token":v.toString()};
	}else{
	    result = {"type":"value","data_type":"int","token":v.toString()};
	}
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' - ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_gt = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 > v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' > ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_lt = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 < v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' < ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_ge = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 >= v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' >= ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_le = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 <= v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' <= ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_eq = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 == v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' == ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_ne = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    try{
	v1 = parseFloat(t1.token);
	v2 = parseFloat(t2.token);
    }catch(e){
	v1 = t1.token.toString();
	v2 = t2.token.toString();
    }
    v = v1 != v2;
    result = {"type":"value","data_type":"bool","token":v.toString()};
    if(this.debug){console.log(v1,' != ',v2,' = ',v );}
    return result;
};
myCalc.prototype.calc_and = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(t1.data_type === 'bool' && t2.data_type === 'bool'){
	v1 = t1.token.toString() === 'true';
	v2 = t2.token.toString() === 'true';
	v = v1 && v2;
	result = {"type":"value","data_type":"bool","token":v.toString()};
    }else{
	throw "error data type  "+t1+'  '+t2;
    }
    if(this.debug){console.log(v1,' && ',v2,' = ',v );}
    return result;
};

myCalc.prototype.calc_or = function(arg){
    var t1 = arg[0];
    var t2 = arg[1];
    var v,v1,v2,result;
    if(t1.data_type === 'name'){
	t1 = this.get_var(t1);
    }
    if(t2.data_type === 'name'){
	t2 = this.get_var(t2);
    }
    if(t1.data_type === 'bool' && t2.data_type === 'bool'){
	v1 = t1.token.toString() === 'true';
	v2 = t2.token.toString() === 'true';
	v = v1 || v2;
	result = {"type":"value","data_type":"bool","token":v.toString()};
    }else{
	throw "error data type  "+t1.token+t2.token;
    }
    if(this.debug){console.log(v1,' || ',v2,' = ',v );}
    return result;
};
myCalc.prototype.pass1 = function(expr){
    expr = expr + ' ';
    var token;
    var stack = [];
    var pos = 0;
    var stat = this.s_idle;
    while(pos<expr.length){
	var ch = expr[pos];
	switch(stat){
	case this.s_idle:
	    token = '';
	    if(this.c_space.indexOf(ch) !== -1){
		pos++;
	    }else if(ch === '"'){
		pos++;
		stat = this.s_string_double;
	    }else if(ch === "'"){
		pos++;
		stat = this.s_string_single;
	    }else if(ch === '['){
		pos++;
		stat = this.s_array;
	    }else if(this.c_digital.indexOf(ch) !== -1){
		token = ch;
		pos++;
		stat = this.s_number_int;
	    }else if(this.c_prefix.indexOf(ch) !== -1){
		token = ch;
		pos++;
		stat = this.s_name;
	    }else if(this.c_operator_prefix.indexOf(ch) !== -1){
		if(ch === '('){
		    stack.push({"type":"left_brace","token":ch});
		}else if(ch === ')'){
		    stack.push({"type":"right_brace","token":ch});
		}else{
		    var ch2 = expr[pos+1];
		    if(this.operators[ch]){
			stack.push({"type":"operator","token":ch});
		    }else if(this.operators[ch + ch2]){
			stack.push({"type":"operator","token":ch+ch2});
			pos++;
		    }
		}
		if(ch === '-'){
		    var last = stack[stack.length - 2];
		    if(last.type === 'left_brace' || last.type === 'operator'){
			stack[stack.length - 1] = '-n';
		    }
		}
		pos++;
	    }else{
		throw 'error char '+ pos + ch;
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
	case this.s_array:
	    if(ch === ']'){
		token = token.replace(/"/g,"");
		token = token.split(',');
		var i;
		for(i=0;i<token.length;i++){
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
		stat = this.s_idle;
	    }
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
	    if(this.c__digital.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else{
		stack.push({"type":"value","data_type":"float","token":token});
		stat = this.s_idle;
	    }
	    break;
	case this.s_name:
	    if(this.c_digital.indexOf(ch) !== -1 || this.c_prefix.indexOf(ch) !== -1){
		token = token + ch;
		pos++;
	    }else if(ch === '.'){
		stack.push({"type":"value","data_type":"name","token":token});
		stack.push({"type":"operator","token":ch});
		pos++;
		stat = this.s_idle;
	    }else if(['true','false'].indexOf(token) !== -1){
		stack.push({"type":"value","data_type":"bool","token":token});
		stat = this.s_idle;
	    }else if(token === 'in'){
		stack.push({"type":"operator","token":token});
		stat = this.s_idle;
	    }else{
		stack.push({"type":"value","data_type":"name","token":token});
		stat = this.s_idle;
	    }
	    break;
	}
    }
    //if(this.debug){console.log(stack);};
    return stack;
};
myCalc.prototype.pass2 = function(expr){
    var output = [];
    var stack = [];
    var token;
    var pos = 0;
    while(pos<expr.length){
	token = expr[pos];
	if(token.type === 'value'){
	    output.push(token);
	    pos++;
	}else if(token.type === 'left_brace'){
	    stack.push(token);
	    pos++;
	}else if(token.type === 'right_brace'){
	    token = stack.pop();
	    while(stack.length > 0 && token.type !== 'left_brace'){
		output.push(token);
		token = stack.pop();
	    }
	    if(token.type !== 'left_brace'){
		throw "missing left_brace";
	    }
	    pos++;
	}else if(token.type === 'operator'){
	    var o1 = token;
	    var o2 = null;
	    if(stack.length > 0){
		o2 = stack[stack.length - 1];
		var o1_dir = this.operators[o1.token].dir;
		var p1 = this.operators[o1.token].priority;
		var p2 = this.operators[o2.token].priority;
	    }
	    while(o2 && o2.type === 'operator' && ( (o1_dir === 'ltr' && p1 >= p2) || (o1_dir === 'rtl' && p1 > p2) )){
		output.push(stack.pop());
		o2 = null;
		if(stack.length > 0){
		    o2 = stack[stack.length - 1];
		    p2 = this.operators[o2.token].priority;
		}
	    }
	    stack.push(o1);
	    pos++;
	}
    }
    while(stack.length > 0){
	token = stack.pop();
	if(token.type === 'left_brace'){
	    throw " missing brace ";
	}
	output.push(token);
    }
    if(this.debug){console.log(output);}
    return output;
};
myCalc.prototype.pass3 = function(expr){
    var stack = [];
    var result = [];
    var token;
    var pos = 0;
    while(pos<expr.length){
	token = expr[pos];
	if(token.type === 'value'){
	    result.push(token);
	    pos++;
	}else if(token.type === 'operator'){
	    var num = this.operators[token.token].opers;
	    if(result.length >= num){
		var arg = [];
		var i;
		for(i=0;i<num;i++){
		    arg.push(result.pop());
		}
		var rs = this.do_calc_token(token.token,arg);
		result.push(rs);
	    }else{
		throw " not has enough arg "+token.token;
	    }
	    pos++;
	}
    }
    return result;
};
myCalc.prototype.do_calc_token = function(token,arg){
    arg = arg.reverse();
    return this.operators[token].calc.bind(this)(arg);
};
myCalc.prototype.get_var = function(token,obj){
    if(!obj){obj = this.vars;}
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
	result = {"type":"value","data_type":"undefind","token":"undefind"};
    }
    return result;

};
module.exports = myCalc;


