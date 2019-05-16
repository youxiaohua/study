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
    this.error = false;
    this.c_operators = {
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
	var tokens = pass1(this.expr,this.vars);//解析算式
	this.rp = pass2(tokens);//调度场算法
    }catch(error){
	console.log(error);
	
    }
}
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
	    }
	}
    }
};
