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
        T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT, T_SEMI,T_EQUALS, T_IDENT,
	//Keywords
	T_PRINT, T_INT
};
	
// AST node types
enum {
	A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT, A_IDENT, A_LVIDENT, A_ASSIGN
};

// Abstract Sysntx Tree structure
struct ASTnode {
	int op;				// "Operation to be performed on tthis tree
	struct ASTnode *left;		// Left child tree
	struct ASTnode *right;		// Right child tree
	
	union {
		int intvalue;		// For A_INTLIT, the integer value
		int id;			// For A_IDENT, the symbol slot number
	} v;
};

//Symbol table structure
struct symtable {
	char *name;	//Name of a symbol
};
