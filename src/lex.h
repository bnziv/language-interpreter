#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <iostream>
#include <map>
using namespace std;


//Definition of all the possible token types
enum Token {
	//Keywords or reserved words
	IF, ELSE, PRINT, INTEGER, REAL,
	CHARACTER, END, THEN, PROGRAM,
	TRUE, FALSE, LEN,
	//Identifiers
	IDENT, 
	//Constants
	ICONST, RCONST, SCONST, BCONST,
	//Operators
	PLUS, MINUS, MULT, DIV, ASSOP, EQ, POW,
	GTHAN, LTHAN, CAT,
	//Delimiters
	COMMA, LPAREN, RPAREN, DOT, DCOLON, DEF,
	//Error
	ERR,
	//On EOF
	DONE,
};


class LexItem {
	Token	token;
	string	lexeme;
	int	lnum;

public:
	LexItem() {
		token = ERR;
		lnum = -1;
	}
	LexItem(Token token, string lexeme, int line) {
		this->token = token;
		this->lexeme = lexeme;
		this->lnum = line;
	}

	bool operator==(const Token token) const { return this->token == token; }
	bool operator!=(const Token token) const { return this->token != token; }

	Token	GetToken() const { return token; }
	string	GetLexeme() const { return lexeme; }
	int	GetLinenum() const { return lnum; }
};



extern ostream& operator<<(ostream& out, const LexItem& tok);
extern LexItem id_or_kw(const string& lexeme, int linenum);
extern LexItem getNextToken(istream& in, int& linenum);


#endif
