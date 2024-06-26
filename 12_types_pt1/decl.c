#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse the current token and return a primitive type enum value
int parse_type(int t ) {
	if (t == T_CHAR)	return (P_CHAR);
	if (t == T_INT)		return (P_INT);
	if (t == T_VOID)	return (P_VOID);
	fatald("Illegal type,token", t);
}

// For now we have a vwery simplistic function declaration grammar
//Parse the declaration of a simplistic function
struct ASTnode *function_declaration(void) {
	struct ASTnode *tree;
	int nameslot;

	// Find the 'void', the identifier, and the '(' ')'.
	// For now, do nothing with them
	match(T_VOID, "void");
	ident();
	nameslot = addglob(Text, P_VOID, S_FUNCTION);
	lparen();
	rparen();

	// Get the AST tree for the compound statement
	tree = compound_statement();
	
	// Return an A_FUNCTION which has the function's nameslot and the compound statement sub-tree
	return(mkastunary(A_FUNCTION, P_VOID, tree, nameslot));
}

//Parse the declaration of a variable
void var_declaration(void) {
	int id, type;
	
	// Get the type of the variable, the the identifier
	type = parse_type(Token.token);
	scan(&Token);
	ident();
	
	// Text now has the identifier's name. Add it as a known identifier and
	// generate its space in assembly
	id = addglob(Text, type, S_VARIABLE);
	genglobsym(id);
	// Get the trailing semicolon
	semi();
}
