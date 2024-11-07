#include "interpreter.h"

map<string, bool> defVar; //Map of declared variables
map<string, Token> SymTable;
map<string, bool> initVar; //Map of initialized variables

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQueue; //Declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if(pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}
	static void PushBackToken(LexItem & t) {
		if(pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}
}

static int error_count = 0;
int ErrCount(){
    return error_count;
}

void ParseError(int line, string msg){
	++error_count;
	cout << line << ": " << msg << endl;
}

//Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line) {
    LexItem token = Parser::GetNextToken(in, line);
    if (token != PROGRAM) {
        ParseError(line, "Missing Program");
        return false;
    }
    token = Parser::GetNextToken(in, line);
	if (token != IDENT) {
		ParseError(line, "Missing Program name");
		return false;
	}
	
	token = Parser::GetNextToken(in, line);
	while (token == REAL || token == INTEGER || token == CHARACTER) { //Iterating through declarations, ending when token isn't a Type
		Parser::PushBackToken(token);
		if (!Decl(in, line)) {
			ParseError(line, "Incorrect Declaration in Program");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		//cout << "Prog Decl " << token << endl;
	}

	while (token == IF || token == PRINT || token == IDENT) { //Iterating through statements, ending when token isn't a statement
		Parser::PushBackToken(token);
		if (!Stmt(in, line)) {
			ParseError(token.GetLinenum(), "Incorrect Statement in Program");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		//cout << "Prog Stmt " << token << endl;
	}
	
	if (token != END) {
		ParseError(line, "Missing END of Program");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != PROGRAM) {
		ParseError(line, "Missing Program");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != IDENT) {
		ParseError(line, "Missing Program name");
		return false;
	}
	return true;
}


//Decl ::= Type :: VarList
//Type ::= INTEGER | REAL | CHARACTER [(LEN = ICONST)]
bool Decl(istream& in, int& line) {
	string len;
	LexItem token = Parser::GetNextToken(in, line);
	if (token != INTEGER && token != REAL && token != CHARACTER) {
		ParseError(line, "Missing Type");
		return false;
	}
	LexItem type = token;
	token = Parser::GetNextToken(in, line);

	if (token == LPAREN) { //Type has a length
		token = Parser::GetNextToken(in, line);
		if (token != LEN) {
			ParseError(line, "Missing Length");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token != ASSOP) {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token != ICONST) {
			ParseError(line, "Incorrect Initialization of a String Length");
			return false;
		}
		len =  token.GetLexeme();
		token = Parser::GetNextToken(in, line);
		if (token != RPAREN) {
			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
	} else {
		Parser::PushBackToken(token);
	}

	token = Parser::GetNextToken(in, line);
	if (token != DCOLON) {
		ParseError(line, "Missing Double Colon");
		return false;
	}
	if (len == "") {
		if (!VarList(in, line, type)) {
			ParseError(line, "Missing Variable List");
			return false;
		}
	} else {
		if (!VarList(in, line, type, stoi(len))) {
			ParseError(line, "Missing Variable List");
			return false;
		}
	}
	return true;
}

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok, int strlen) {
	bool exprStatus = false;
	string identName;
	Value exprVal;

	if (idtok.GetToken() == CHARACTER) {
		exprVal.SetType(VSTRING);
		exprVal.SetstrLen(strlen);
		string initialStr(strlen, ' ');
		exprVal.SetString(initialStr);
	} else if (idtok.GetToken() == REAL) {
		exprVal.SetType(VREAL);
	} else if (idtok.GetToken() == INTEGER) {
		exprVal.SetType(VINT);
	}

	LexItem token = Parser::GetNextToken(in, line);
	if (token == IDENT) {
		identName = token.GetLexeme();
		if (!(defVar.find(identName)->second)) {
			defVar[identName] = true;
			SymTable[identName] = idtok.GetToken();
			if (idtok.GetToken() != CHARACTER) { //Initialize if it's a character variable
				initVar[identName] = false;
			} else {
				initVar[identName] = true;
			}
		} else {
			ParseError(line, "Variable Redefinition");
			return false;
		}
	} else {
		ParseError(line, "Missing Variable Name");
		return false;
	}
	TempsResults[identName] = exprVal;

	token = Parser::GetNextToken(in, line);
	if (token == ASSOP) {
		if (!Expr(in, line, exprVal)) {
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		if (exprVal.GetType() == VSTRING) {
			string initStr = exprVal.GetString();
			if (initStr.length() > strlen) { //Adjusting string to declared length
				initStr = initStr.substr(0, strlen);
			} else if (initStr.length() < strlen) {
				initStr.append(strlen - initStr.length(), ' ');
			}
			exprVal.SetString(initStr);
		}
		TempsResults[identName] = exprVal;
		initVar[identName] = true;

		token = Parser::GetNextToken(in, line);
		if (token == COMMA) {
			return VarList(in, line, idtok, strlen);
		} else {
			Parser::PushBackToken(token);
			return true;
		}
	} else if (token == COMMA) {
		return VarList(in, line, idtok, strlen);
	} else {
		Parser::PushBackToken(token);
		return true;
	}
	return true;
}

//Stmt ::= AssignStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	switch(token.GetToken()) {
		case IDENT: {
			Parser::PushBackToken(token);
			return SimpleStmt(in, line);
			break;
		}
		case PRINT: {
			Parser::PushBackToken(token);
			return PrintStmt(in, line);
			break;
		}
		case IF: {
			Parser::PushBackToken(token);
			return BlockIfStmt(in, line);
			break;
		}
		default:
			ParseError(line, "Missing Statement");
			return false;
	}
	return true;
}

//PrintStmt ::= PRINT *, ExprList
bool PrintStmt(istream& in, int& line) {
	LexItem token;
	ValQueue = new queue<Value>;

	token = Parser::GetNextToken(in, line);
	if (token != PRINT) {
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != DEF) {
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != COMMA) {
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	if (!ExprList(in, line)) {
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	while (!(*ValQueue).empty()) {
		Value nextVal = (*ValQueue).front();
		cout << nextVal;
		ValQueue->pop();
	}
	cout << endl;
	return true;
}

//SimpleIfStatement ::= IF (RelExpr) Stmt
//BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
bool BlockIfStmt(istream& in, int& line) {
	bool flag;
	LexItem token = Parser::GetNextToken(in, line);
	Value retVal;

	if (token != IF) {
		ParseError(line, "Missing IF");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != LPAREN) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	if (!RelExpr(in, line, retVal)) {
		ParseError(line, "Missing If-Statement Condition");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (retVal.GetType() != VBOOL) {
		ParseError(line, "Runtime Error - Illegal Type for If-Statement Condition");
		return false;
	}
	if (token != RPAREN) {
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	bool relExpr = retVal.GetBool();
	token = Parser::GetNextToken(in, line);
	
	//SimpleIfStmt
	if (token != THEN) { 
		if (relExpr) {
			Parser::PushBackToken(token);
			if (SimpleStmt(in, line)) {
				return true;
			} else {
				ParseError(line, "Missing Simple Statement");
				return false;
			}
		} else {
			return true;
		}
	}

	//BlockIfStmt
	if (relExpr) {
		while (true) {
			token = Parser::GetNextToken(in, line);
			if (token == ELSE || token == END) {
				break;
			} else {
				Parser::PushBackToken(token);
			}
			if (!Stmt(in, line)) {
				ParseError(line, "Missing Statement");
				return false;
			}
		}
	} else {
		while (token != ELSE && token != END) {
			token = Parser::GetNextToken(in, line);
		}
	}

	if (token == ELSE) {
		if (!relExpr) {
			while (true) {
				token = Parser::GetNextToken(in, line);
				if (token == END) {
					break;
				} else {
					Parser::PushBackToken(token);
				}
				if (!Stmt(in, line)) {
					ParseError(line, "Missing Statement");
					return false;
				}
			}
		} else {
			while (token != END) {
				token = Parser::GetNextToken(in, line);
			}
		}
	} 

	if (token == END) {
		token = Parser::GetNextToken(in, line);
		if (token != IF) {
			ParseError(line, "Missing IF at end of IF statement");
			return false;
		}
		return true;
	} else {
		ParseError(line, "Missing END");
		return false;
	}
	return flag;
}

//SimpleStmt ::= AssignStmt | PrintStmt
bool SimpleStmt(istream& in, int& line) {
	LexItem token = Parser::GetNextToken(in, line);
	switch(token.GetToken()) {
		case IDENT: {
			Parser::PushBackToken(token);
			return AssignStmt(in, line);
			break;
		}
		case PRINT: {
			Parser::PushBackToken(token);
			return PrintStmt(in, line);
			break;
		}
		default: {
			ParseError(line, "Missing Simple Statement");
			return false;
		}
	}
	return true;
}

//AssignStmt ::= Var = Expr
bool AssignStmt(istream& in, int& line) {
	Value retVal;
	LexItem token;
	if (!Var(in, line, token)) {
		ParseError(line, "Missing Variable");
		return false;
	}
	string varName = token.GetLexeme();
	token = Parser::GetNextToken(in, line);
	if (token != ASSOP) {
		ParseError(line, "Missing Assignment Operator");
		return false;
	}
	int originalStrlen;
	retVal = TempsResults[varName];
	if (retVal.GetType() == VSTRING) {
		originalStrlen = retVal.GetstrLen();
	}
	initVar[varName] = true;
	if (!Expr(in, line, retVal)) {
		ParseError(token.GetLinenum(), "Missing Expression in Assignment Statement");
		return false;
	}
	if (retVal.GetType() == VSTRING) {
		string resultStr = retVal.GetString();
		if (resultStr.length() < originalStrlen) {
			resultStr.append(originalStrlen - resultStr.length(), ' ');
		} else if (resultStr.length() > originalStrlen) {
			resultStr = resultStr.substr(0, originalStrlen);
		}
		retVal.SetString(resultStr);
		retVal.SetstrLen(originalStrlen);
	}
	if (SymTable[varName] == CHARACTER && retVal.GetType() != VSTRING) {
		ParseError(token.GetLinenum(), "Illegal mixed-mode assignment operation");
		return false;
	} else if (SymTable[varName] == INTEGER && retVal.GetType() == VSTRING) {
		ParseError(token.GetLinenum(), "Illegal mixed-mode assignment operation");
		return false;
	} else if (SymTable[varName] == REAL && retVal.GetType() == VSTRING) {
		ParseError(token.GetLinenum(), "Illegal mixed-mode assignment operation");
		return false;
	}
	TempsResults[varName] = retVal;
	return true;
}

//ExprList ::= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	Value retVal;

	if (!Expr(in, line, retVal)) {
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQueue->push(retVal);
	LexItem token = Parser::GetNextToken(in, line);
	if (token == COMMA) {
		if (!ExprList(in, line)) {
			return false;
		}
	} else if (token.GetToken() == ERR) {
		return false;
	} else {
		Parser::PushBackToken(token);
		return true;
	}
	return true;
}

//RelExpr ::= Expr [( == | < | > ) Expr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	Value comp1, comp2;
	if (!Expr(in, line, comp1)) {
		return false;
	}
	LexItem token = Parser::GetNextToken(in, line);
	if (token == EQ || token == LTHAN || token == GTHAN) {
		string op = token.GetLexeme();
		if (!Expr(in, line, comp2)) {
			return false;
		}
		if (op == "==") {
			retVal = comp1 == comp2;
		} else if (op == "<") {
			retVal = comp1 < comp2;
		} else if (op == ">") {
			retVal = comp1 > comp2;
		}
	} else {
		retVal = comp1;
		Parser::PushBackToken(token);
	}
	if (retVal.GetType() == VERR) {
		ParseError(line, "Illegal Operand Types for a Relational Operation");
		return false;
	}
	return true;
}

//Expr ::= MultExpr {(+ | - | //) MultExpr}
bool Expr(istream& in, int& line, Value & retVal) {
	if (!MultExpr(in, line, retVal)) {
		return false;
	}
	LexItem token = Parser::GetNextToken(in, line);

	while (token == PLUS || token == MINUS || token == CAT) {
		string op = token.GetLexeme();
		Value opVal;
		if (!MultExpr(in, line, opVal)) {
			ParseError(line, "Missing Operand After Operator");
			return false;
		}
		if (op == "+") {
			retVal = retVal + opVal;
		} else if (op == "-") {
			retVal = retVal - opVal;
		} else if (op == "//") {
			retVal = retVal.Catenate(opVal);
		}
		if (retVal.GetType() == VERR) {
			ParseError(token.GetLinenum(), "Illegal Operand Type for the Operation.");
			return false;
		}
		token = Parser::GetNextToken(in, line);
	}
	Parser::PushBackToken(token);
	return true;
}

//MultExpr ::= TermExpr {(* | / ) TermExpr}
bool MultExpr(istream& in, int& line, Value & retVal) {
	if (!TermExpr(in, line, retVal)) {
		return false;
	}

	LexItem token = Parser::GetNextToken(in, line);
	while (token == MULT || token == DIV) {
		string op = token.GetLexeme();
		Value opVal;
		if (!TermExpr(in, line, opVal)) {
			ParseError(line, "Missing Operand After Operator");
			return false;
		}
		if (op == "*") {
			retVal = retVal * opVal;
		} else if (op == "/") {
			if (opVal.GetType() == VREAL && opVal.GetInt() == 0) {
				ParseError(line, "Runtime Error - Division by Zero");
				return false;
			} else if (opVal.GetType() == VREAL && opVal.GetReal() == 0.0) {
				ParseError(line, "Runtime Error - Division by Zero");
				return false;
			}
			retVal = retVal / opVal;
		}
		if (retVal.GetType() == VERR) {
			ParseError(line, "Illegal operand types for the operation.");
			return false;
		}
		token = Parser::GetNextToken(in, line);
	}
	Parser::PushBackToken(token);
	return true;
}

//TermExpr ::= SFactor {** SFactor}
bool TermExpr(istream& in, int& line, Value& retVal) {
	if (!SFactor(in, line, retVal)) {
		return false;
	}
	LexItem token = Parser::GetNextToken(in, line);

	while (token == POW) {
		Value opVal;
		if (!TermExpr(in, line, opVal)) {
			ParseError(line, "Missing exponent operand");
		}
		retVal = retVal.Power(opVal);
		token = Parser::GetNextToken(in, line);
	}
	Parser::PushBackToken(token);
	return true;
}

//SFactor ::= [+ | -] Factor
bool SFactor(istream& in, int& line, Value& retVal) {
	int sign = 1;
	LexItem token = Parser::GetNextToken(in, line);

	if (token == MINUS) {
		sign = -1;
		if (retVal.GetType() == VSTRING) {
			ParseError(line, "Run-Time Error: Illegal Operand Type for Sign Operator");
			return false;
		}
	} else if (token == PLUS) {
		sign = 1;
		if (retVal.GetType() == VSTRING) {
			ParseError(line, "Run-Time Error: Illegal Operand Type for Sign Operator");
			return false;
		}
	} else {
		Parser::PushBackToken(token);
	}
	if (!Factor(in, line, sign, retVal)) {
		return false;
	}
	return true;
}

//Var ::= IDENT
bool Var(istream& in, int& line, LexItem & idtok) {
	LexItem token = Parser::GetNextToken(in, line);
	string varName;
	
	if (token == IDENT) {
		varName = token.GetLexeme();
		if (!(defVar.find(varName)->second)) {
			ParseError(line, "Undeclared Variable");
			return false;
		}
		idtok = token;
		return true;
	} else {
		ParseError(line, "Missing Variable Name");
		return false;
	}
	return false;
}

//Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {
	LexItem token = Parser::GetNextToken(in, line);
	if (token == IDENT) {
		string lexeme = token.GetLexeme();
		if (!(defVar.find(lexeme)->second)) {
			ParseError(line, "Undeclared Variable");
			return false;
		}
		if (!initVar[lexeme]) {
			ParseError(line, "Using Uninitialized Variable");
			return false;
		}
		retVal = TempsResults[lexeme];
		if (retVal.GetType() == VINT || retVal.GetType() == VREAL) {
			retVal = retVal * sign;
		}
		return true;
	} else if (token == ICONST) {
		if (retVal.GetType() == VINT) {
			retVal.SetInt(stoi(token.GetLexeme()) * sign);
		} else if (retVal.GetType() == VREAL) {
			retVal.SetReal(stod(token.GetLexeme()) * sign);
		} else {
			retVal.SetType(VINT);
			retVal.SetInt(stoi(token.GetLexeme()) * sign);
		}
		return true;
	} else if (token == RCONST) {
		retVal.SetType(VREAL);
		retVal.SetReal(stod(token.GetLexeme()) * sign);
		return true;
	} else if (token == SCONST) {
		retVal.SetType(VSTRING);
		string strLexeme = token.GetLexeme();
		if (retVal.GetstrLen() > 0 && strLexeme.length() < retVal.GetstrLen()) {
			strLexeme.append(retVal.GetstrLen() - strLexeme.length(), ' ');
		} else if (retVal.GetstrLen() > 0 && strLexeme.length() > retVal.GetstrLen()) {
			strLexeme = strLexeme.substr(0, retVal.GetstrLen());
		}
		retVal.SetString(strLexeme);
		return true;
	} else if (token == LPAREN) {
		if (!Expr(in, line, retVal)) {
			ParseError(line, "Missing Expression");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token != RPAREN) {
			ParseError(line, "Missing Right Parenthesis");
			return false;
		}
		return true;
	}
	return false;
}