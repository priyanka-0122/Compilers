#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse the current token and return a primitive type enum value
int parse_type(void) {
	int type;
	switch (Token.token) {
		case T_VOID:
			type = P_VOID;
			break;
		case T_CHAR:
			type = P_CHAR;
			break;
		case T_INT:
			type = P_INT;
			break;
		case T_LONG:
			type = P_LONG;
			break;
		default:
			fatald("Illegal type,token", Token.token);
	}
	
	// Scan in one or more further '*' tokens and determine the correct pointer type
	while (1) {
		scan(&Token);
		if (Token.token != T_STAR)
			break;
		type = pointer_to(type);
	}

	// We leave with the next token already scanned
	return (type);
}

// For now we have a vwery simplistic function declaration grammar
// Parse the declaration of a simplistic function
struct ASTnode *function_declaration(void) {
	struct ASTnode *tree, *finalstmt;
	int nameslot, type, endlabel;

	// Get the type of the variable, then the identifier
	type = parse_type();
	ident();

	// Get a label id for the end label, add the function to the symbol table, and set the 
	// Functionid global to the function's symbol id
	endlabel = genlabel();
	nameslot = addglob(Text, type, S_FUNCTION, endlabel);
	Functionid = nameslot;

	// Scan in the parantheses
	lparen();
	rparen();

	// Get the AST tree for the compound statement
	tree = compound_statement();
	
	// If the function type isn't P_VOID, check that the last AST operation in the compond statement 
	// was a return statement
	if (type != P_VOID) {
		
		// Error if no statements in the function
		if (tree == NULL)
			fatal("No statements in function with non-void type");

		// Check that the last AST operantion in the compound statement was a return statement
		finalstmt = (tree->op == A_GLUE) ? tree->right : tree;
		if (finalstmt == NULL || finalstmt->op != A_RETURN)
			fatal("No return for function with non-void type");
	}

	// Return an A_FUNCTION which has the function's nameslot and the compound statement sub-tree
	return(mkastunary(A_FUNCTION, type, tree, nameslot));
}

//Parse the declaration of a variable
void var_declaration(void) {
	int id, type;
	
	// Get the type of the variable, which also scans in the identifier
	type = parse_type();
	ident();
	
	// Text now has the identifier's name. Add it as a known identifier and
	// generate its space in assembly
	id = addglob(Text, type, S_VARIABLE, 0);
	genglobsym(id);
	// Get the trailing semicolon
	semi();
}
