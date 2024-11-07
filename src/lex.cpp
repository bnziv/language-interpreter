#include "lex.h"

LexItem getNextToken(istream& in, int& linenumber) {
    enum TokState {
        START, INID, INSTRING, ININT, INREAL, INMULT, INDIV, INDOT, INEQ, INCOMMENT, INDCOLON
    };
    TokState lexstate = START;
    string lexeme;
    char ch;

    while (in.get(ch)) {
        switch(lexstate) {
            case START: 
                if (isspace(ch)) {
                    if (ch == '\n') {
                        linenumber++;
                    }
                    continue;
                }
                lexeme = ch;
                if (isalpha(ch)) {
                    lexstate = INID;
                } else if (isdigit(ch)) {
                    lexstate = ININT;
                } else if (ch == '"' || ch == '\'') {
                    lexstate = INSTRING;
                } else if(ch == '*') { //Can be MULT, POW, or DEF
                    lexstate = INMULT;
                } else if (ch == '/') { //Can be DIV, or CONCAT
                    lexstate = INDIV;
                } else if (ch == '.') { //Can be DOT, or RCONST
                    lexstate = INDOT;
                } else if (ch == '=') { //Can be ASSOP, or EQ
                    lexstate = INEQ;
                } else if (ch == '!') {
                    lexstate = INCOMMENT;
                } else if (ch == ':') {
                    lexstate = INDCOLON;
                }
                else { //Single character tokens
                    Token token;
                    switch (ch) { 
                        case '+': 
                            token = PLUS;
                            break;
                        case '-': 
                            token = MINUS;
                            break;
                        case '<': 
                            token = LTHAN;
                            break;
                        case '>': 
                            token = GTHAN;
                            break;
                        case '(': 
                            token = LPAREN;
                            break;
                        case ')': 
                            token = RPAREN;
                            break;
                        case ',': 
                            token = COMMA;
                            break;
                        default:
                            linenumber++;
                            return LexItem(ERR, lexeme, linenumber);
                    }
                    return LexItem(token, lexeme, linenumber);
                }
                break;

            case INID:
                if (isalnum(ch) || ch == '_') {
                    lexeme += ch;
                } else {
                    in.putback(ch);
                    return id_or_kw(lexeme, linenumber);
                }
                break;
            case ININT:
                if (isdigit(ch)) {
                    lexeme += ch;
                } else if (ch == '.') {
                    if (!isdigit(in.peek())) { //in the case of 1.123.3
                        return LexItem(ERR, lexeme, linenumber);
                    }
                    lexeme += ch;
                    lexstate = INREAL;
                } else {
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenumber);
                }
                break;
            case INREAL:
                if (isdigit(ch)) {
                    lexeme += ch;
                } else if (ch == '.') {
                    lexeme += ch;
                    linenumber++;
                    return LexItem(ERR, lexeme, linenumber);
                } else {
                    in.putback(ch);
                    return LexItem(RCONST, lexeme, linenumber);
                }
                
                break;
            case INDOT:
                if (isdigit(ch)) {
                    lexeme += ch;
                    lexstate = INREAL;
                } else {
                    in.putback(ch);
                    return LexItem(DOT, lexeme, linenumber);
                }
                break;
            case INSTRING:
                if (ch == '\n') {
                    linenumber++;
                    return LexItem(ERR, lexeme, linenumber);
                } else if ((ch == '"' && lexeme[0] == '"') || (ch == '\'' && lexeme[0] == '\'')) {
                    lexeme = lexeme.substr(1,lexeme.length()-1); //strip quotes from the lexeme
                    return LexItem(SCONST, lexeme, linenumber);
                } else {
                    lexeme += ch;
                }
                break;
            case INEQ:
                if (ch == '=') {
                    lexeme += ch;
                    return LexItem(EQ, lexeme, linenumber);
                } else {
                    in.putback(ch);
                    return LexItem(ASSOP, lexeme, linenumber);
                }
                break;
            case INDIV:
                if (ch == '/') {
                    lexeme += ch;
                    return LexItem(CAT, lexeme, linenumber);
                } else {
                    in.putback(ch);
                    return LexItem(DIV, lexeme, linenumber);
                }
                break;
            case INMULT: 
                if (ch == '*') {
                    lexeme += ch;
                    return LexItem(POW, lexeme, linenumber);
                } else if (ch == ',') {
                    in.putback(ch);
                    return LexItem(DEF, lexeme, linenumber);
                } else {
                    in.putback(ch);
                    return LexItem(MULT, lexeme, linenumber);
                }
                break;
            case INCOMMENT:
                if (ch == '\n') {
                    linenumber++;
                    lexstate = START;
                }
                break;
            case INDCOLON:
                if (ch == ':') {
                    lexeme += ch;
                    return LexItem(DCOLON, lexeme, linenumber);
                } else {
                    lexeme += ch;
                    linenumber++;
                    return LexItem(ERR, lexeme, linenumber);
                }
                break;
            default:
                break;
        }
    }
    return LexItem(DONE, "", linenumber);
}

LexItem id_or_kw(const string& lexeme, int linenum) {
    std::map<string, Token> keywordMap = {
        {"if", IF},
        {"else", ELSE},
        {"print", PRINT},
        {"integer", INTEGER},
        {"real", REAL},
        {"character", CHARACTER},
        {"end", END},
        {"then", THEN},
        {"program", PROGRAM},
        {"len", LEN},
    };
    std::string lowerLexeme = lexeme;
    for (int i = 0; i < lowerLexeme.length(); i++) { //Convert to lower since reserved words are not case sensitive
        lowerLexeme[i] = tolower(lowerLexeme[i]);
    }
    auto it = keywordMap.find(lowerLexeme);
    if (it != keywordMap.end()) {
        return LexItem(it->second, lexeme, linenum);
    } else {
        return LexItem(IDENT, lexeme, linenum);
    }
 }

ostream& operator<<(ostream& out, const LexItem& tok) {
    if (tok.GetToken() == ICONST) {
        out << "ICONST: (" << tok.GetLexeme() << ")";
    } else if (tok.GetToken() == RCONST) {
        out << "RCONST: (" << tok.GetLexeme() << ")";
    } else if (tok.GetToken() == BCONST) {
        out << "BCONST: (" << tok.GetLexeme() << ")";
    } else if (tok.GetToken() == SCONST) {
        out << "SCONST: \"" << tok.GetLexeme() << "\"";
    } else if (tok.GetToken() == IDENT) {
        out << "IDENT: \'" << tok.GetLexeme() << "\'";
    } //Rest of tokens
    else if (tok.GetToken() == IF) {out << "IF";}
    else if (tok.GetToken() == ELSE) {out << "ELSE";}
    else if (tok.GetToken() == PRINT) {out << "PRINT";}
    else if (tok.GetToken() == INTEGER) {out << "INTEGER";}
    else if (tok.GetToken() == REAL) {out << "REAL";}
    else if (tok.GetToken() == CHARACTER) {out << "CHARACTER";}
    else if (tok.GetToken() == END) {out << "END";}
    else if (tok.GetToken() == THEN) {out << "THEN";}
    else if (tok.GetToken() == PROGRAM) {out << "PROGRAM";}
    else if (tok.GetToken() == LEN) {out << "LEN";}
    else if (tok.GetToken() == PLUS) {out << "PLUS";}
    else if (tok.GetToken() == MINUS) {out << "MINUS";}
    else if (tok.GetToken() == MULT) {out << "MULT";}
    else if (tok.GetToken() == DIV) {out << "DIV";}
    else if (tok.GetToken() == ASSOP) {out << "ASSOP";}
    else if (tok.GetToken() == EQ) {out << "EQ";}
    else if (tok.GetToken() == POW) {out << "POW";}
    else if (tok.GetToken() == GTHAN) {out << "GTHAN";}
    else if (tok.GetToken() == LTHAN) {out << "LTHAN";}
    else if (tok.GetToken() == CAT) {out << "CAT";}
    else if (tok.GetToken() == COMMA) {out << "COMMA";}
    else if (tok.GetToken() == LPAREN) {out << "LPAREN";}
    else if (tok.GetToken() == RPAREN) {out << "RPAREN";}
    else if (tok.GetToken() == DOT) {out << "DOT";}
    else if (tok.GetToken() == DCOLON) {out << "DCOLON";}
    else if (tok.GetToken() == DEF) {out << "DEF";}
    return out;
}