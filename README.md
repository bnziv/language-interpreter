# Language Interpreter

This is a language interpreter written in C++ for a simple programming language akin to Fortran95, called "SFort95". It supports syntax analysis, semantic checking, and execution of simple programs. It consists of a lexical analyzer and a recursive descent parser. 

## Features

* Parses and verifies the syntax of SFort95 programs based on EBNF grammar rules
* Performs type checking and detects runtime errors such as uninitialized variables, division by zero, and illegal operand types
* Evaluates expressions, assigns values, and executes control flow statments, and prints results
* Provides detailed error messages with line numbers for syntax and runtime errors

## Usage
Requires a C++ compiler

#### Compiling
To compile the interpreter, run the following command:
```
g++ src/*.cpp -o interpreter
```

#### Running
To run the interpreter on a program file, use the following command:
```
./interpreter <program_file>
```
Test programs and their expected outputs can be found in the `test` directory.

#### Examples
Running the interpreter on the following test files should produce the following output:

```
./interpreter test/test2
The output results are: 9, 4, -3
```

```
./interpreter test/test9
6: Runtime Error - Illegal Type for If-Statement Condition
6: Incorrect Statement in Program

Status: Unsuccessful Interpretation
Number of Errors 2
```

## Files
* `lex.cpp` and `lex.h`: Lexical analyzer
* `interpreter.cpp` and `interpreter.h`: Recursive descent parser with interpreter actions
* `val.cpp` and `val.h`: Implementation of the Value class for constants, variables, and expressions
* `program.cpp`: Main function for the interpreter

## Grammar Rules
The EBNF grammar rules for the language are as follows:
```
Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
Decl ::= Type :: VarList
Type ::= INTEGER | REAL | CHARACTER [(LEN = ICONST)]
VarList ::= Var [= Expr] {, Var [= Expr]}
Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
PrintStmt ::= PRINT *, ExprList
BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
SimpleIfStmt ::= IF (RelExpr) SimpleStmt
SimpleStmt ::= AssigStmt | PrintStmt
AssignStmt ::= Var = Expr
ExprList ::= Expr {, Expr}
RelExpr ::= Expr [ ( == | < | > ) Expr ]
Expr ::= MultExpr { ( + | - | // ) MultExpr }
MultExpr ::= TermExpr { ( * | / ) TermExpr }
TermExpr ::= SFactor { ** SFactor }
SFactor ::= [+ | -] Factor
Var ::= IDENT
Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
```

## License
This project is released under the [MIT License](https://opensource.org/licenses/MIT).