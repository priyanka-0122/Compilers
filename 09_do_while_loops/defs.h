#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define TEXTLEN		512 //Length of symbols in input
#define NSYMBOLS	1024	//Number of symbol table entries

//Token structure
struct token {
        int token;
        int intvalue;
};

//Tokens
enum {
        T_EOF,
	//Binary Operator
	T_PLUS, T_MINUS, T_STAR, T_SLASH,
	//Comparison operator
	T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,

	T_INTLIT, T_SEMI, T_ASSIGN, T_IDENT,
	T_LBRACE, T_RBRACE, T_LPAREN, T_RPAREN,
	//Keywords
	T_PRINT, T_INT, T_IF, T_ELSE, T_WHILE, T_DO
};
	
// AST node types
enum {
	A_ADD=1, A_SUBTRACT, A_MULTIPLY, A_DIVIDE,
	A_EQ, A_NE, A_LT, A_GT, A_LE, A_GE,
	A_INTLIT,
	A_IDENT, A_LVIDENT, A_ASSIGN, A_PRINT, A_GLUE,
	A_IF, A_WHILE, A_DO_WHILE
};

// Abstract Sysntx Tree structure
struct ASTnode {
	int op;				// Operation to be performed on this tree
	struct ASTnode *left;		// Left child tree
	struct ASTnode *mid;		// Middle child tree
	struct ASTnode *right;		// Right child tree
	
	union {
		int intvalue;		// For A_INTLIT, the integer value
		int id;			// For A_IDENT, the symbol slot number
	} v;
};

#define NOREG	-1	// Use NOREG when the AST generation functions have no register to return

//Symbol table structure
struct symtable {
	char *name;	//Name of a symbol
};
