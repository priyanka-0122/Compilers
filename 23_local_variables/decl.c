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
struct ASTnode *function_declaration(int type) {
	struct ASTnode *tree, *finalstmt;
	int nameslot, endlabel;

	// Get a label id for the end label, add the function to the symbol table, and set the 
	// Functionid global to the function's symbol id
	endlabel = genlabel();
	nameslot = addglob(Text, type, S_FUNCTION, endlabel, 0);
	Functionid = nameslot;

	genresetlocals();	// Reset position of new locals

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

// Parse the declaration of a list of variables.
// The identifier has been scanned & we have the type
void var_declaration(int type, int islocal) {

	// Text now has the identifier's name. If the next token is a '['
	if (Token.token == T_LBRACKET) {
		// Skip past the '['
		scan(&Token);

		// Check we have an array size
		if (Token.token == T_INTLIT) {
			// Add this as a known array and generate its apace in assembly.
			// We treat the array as a pointer to its elements' type
			if (islocal) {
				addlocl(Text, pointer_to(type), S_ARRAY, 0, Token.intvalue);
			} else {
				addglob(Text, pointer_to(type), S_ARRAY, 0, Token.intvalue);
			}
		}
	
		// Ensure we have a following ']'
		scan(&Token);
		match(T_RBRACKET, "]");

	} else {
    		while(1) {
			// Add this as a known scalar and generate its space in assembly
			if (islocal) {
				addlocl(Text, type, S_VARIABLE, 0, 1);
			} else {
				addglob(Text, type, S_VARIABLE, 0, 1);
			}

			// If the next token is a semicolon, skip it and return.
			if (Token.token == T_SEMI) {
				scan(&Token);
				return;
			}
			// If the next token is a comma, skip it, get the identifier and loop back
    				if (Token.token == T_COMMA) {
      					scan(&Token);
      					ident();
      					continue;
    				}
    				fatal("Missing , or ; after identifier");
			}		
	}
	// Get the trailing semicolon
	semi();
}

// Parse one or more global declarations, either variable or functions
void global_declarations(void) {
	struct ASTnode *tree;
	int type;

	while(1) {
		
		// We have to read past the type and identifier to see either a '(' for a function declaration
		// or a ',' or ';' for a variable declaration. Text is filled in by the ident() call.
		type = parse_type();
		ident();
		if (Token.token == T_LPAREN) {
		
			// Parse the function declaration and generate the assembly code for it
			tree = function_declaration(type);
			if (O_dumpAST) {
				dumpAST(tree, NOLABEL, 0);
				fprintf(stdout, "\n\n");
			}

			genAST(tree, NOREG, 0);
		} else {

			// Parse the global variable declaration
			var_declaration(type, 0);
		}
	
		// Stop when we have reached EOF
		if (Token.token == T_EOF)
			break;
	}
}
