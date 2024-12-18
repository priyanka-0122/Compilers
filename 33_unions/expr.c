#include "defs.h"
#include "data.h"
#include "decl.h"

// Parsing of expressions

// expression_list: <null>
//        | expression
//        | expression ',' expression_list
//        ;

// Parse a list of zero or more comma-separated expressions and
// return an AST composed of A_GLUE nodes with the left-hand child
// being the sub-tree of previous expressions (or NULL) and the right-hand
// child being the next expression. Each A_GLUE node will have size field
// set to the number of expressions in the tree at this point. If no
// expressions are parsed, NULL is returned
static struct ASTnode *expression_list(void) {
  struct ASTnode *tree = NULL;
  struct ASTnode *child = NULL;
  int exprcount = 0;

  // Loop until the final right parentheses
  while (Token.token != T_RPAREN) {

    // Parse the next expression and increment the expression count
    child = binexpr(0);
    exprcount++;

    // Build an A_GLUE AST node with the previous tree as the left child
    // and the new expression as the right child. Store the expression count.
    tree = mkastnode(A_GLUE, P_NONE, tree, NULL, child, NULL, exprcount);

    // Must have a ',' or ')' at this point
    switch (Token.token) {
      case T_COMMA:
        scan(&Token);
        break;
      case T_RPAREN:
        break;
      default:
        fatald("Unexpected token in expression list", Token.token);
    }
  }

  // Return the tree of expressions
  return (tree);
}

// Parse a function call and return its AST
static struct ASTnode *funccall(void) {
	struct ASTnode *tree;
	struct symtable *funcptr;

	// Check that the identifier has been defined, then make a leaf node for it.
	if ((funcptr = findsymbol(Text)) == NULL || funcptr->stype != S_FUNCTION) {
		fatals("Undeclared function", Text);
	}

	// Get the '('
	lparen();

	// Parse the following expression
	tree = expression_list();

	// XXX Check type of each argument against the function's prototype

	// Build the function call AST node. Store the function's return type
	// as this node's type. Also record the function's symbol-id
	tree = mkastunary(A_FUNCCALL, funcptr->type, tree, funcptr, 0);

	// Get the ')'
	rparen();
	return (tree);
}

// Parse the index into an array and return an AST tree for it
static struct ASTnode *array_access(void) {
	struct ASTnode *left, *right;
	struct symtable *aryptr;

	// Check that the identifier has been as an array then make a leaf node for it
	// that points at the base
	if ((aryptr = findsymbol(Text)) == NULL || aryptr->stype != S_ARRAY) {
		fatals("Undeclared array", Text);
	}
	left = mkastleaf(A_ADDR, aryptr->type, aryptr, 0);

  	// Get the '['
  	scan(&Token);

  	// Parse the following expression
  	right = binexpr(0);

	// Compare the size of array with the size to which the value is going to be assigned
	if (aryptr->size <= right->size)
		fatal("Size of the array is smaller than the size being accessed");

	// Get the ']'
	match(T_RBRACKET, "]");

  	// Ensure that this is of int type
  	if (!inttype(right->type))
    		fatal("Array index is not of integer type");

  	// Scale the index by the size of the element's type
  	right = modify_type(right, left->type, A_ADD);

  	// Return an AST tree where the array's base has the offset added to it, and dereference the element.
	// Still an lvalue at this point.
  	left = mkastnode(A_ADD, aryptr->type, left, NULL, right, NULL, 0);
  	left = mkastunary(A_DEREF, value_at(left->type), left, NULL, 0);
  	return (left);
}

// Parse the member reference of a struct (or union, soon) and return an AST tree for it.
// If withpointer is true, the access is through a pointer to the member.
static struct ASTnode *member_access(int withpointer) {
	struct ASTnode *left, *right;
	struct symtable *compvar;
	struct symtable *typeptr;
	struct symtable *m;

	// Check that the identifier has been declared as a struct (or a union, later),
	// or a struct/union pointer
	if ((compvar = findsymbol(Text)) == NULL)
		fatals("Undeclared variable", Text);
	if (withpointer && compvar->type != pointer_to(P_STRUCT)
	    && compvar->type != pointer_to(P_UNION))
		fatals("Undeclared variable", Text);
	if (!withpointer && compvar->type != P_STRUCT && compvar->type != P_UNION)
		fatals("Undeclared variable", Text);

	// If a pointer to a struct, get the pointer's value. Otherwise, make a leaf node that points at the base
	if (withpointer) {
		left = mkastleaf(A_IDENT, pointer_to(compvar->type), compvar, 0);
	} else
		left = mkastleaf(A_ADDR, compvar->type, compvar, 0);
	left->rvalue = 1;

	// Get the details of the composite type
	typeptr = compvar->ctype;

	// Skip the '.' or '->' token and get the member's name
	scan(&Token);
	ident();

	// Find the matching member's name in the type.  Die if we can't find it
	for (m = typeptr->member; m != NULL; m = m->next)
		if (!strcmp(m->name, Text))
			break;

	if (m == NULL)
		fatals("No member found in struct/union: ", Text);

	// Build an A_INTLIT node with the offset
	right = mkastleaf(A_INTLIT, P_INT, NULL, m->posn);

	// Add the member's offset to the base of the struct and dereference it. Still an lvalue at this point
	left = mkastnode(A_ADD, pointer_to(m->type), left, NULL, right, NULL, 0);
	left = mkastunary(A_DEREF, m->type, left, NULL, 0);
	return (left);
}

// Parse a postfix expression and return an AST node representing it. The identifier is already in Text
static struct ASTnode *postfix(void) {
	struct ASTnode *n;
	struct symtable *varptr;

	// Scan in the next token to see if we have a postfix expression
	scan(&Token);

	// Function call
	if (Token.token == T_LPAREN)
		return (funccall());

	// An array reference
	if (Token.token == T_LBRACKET)
		return (array_access());

	// Access into a struct or union
	if (Token.token == T_DOT)
		return (member_access(0));
	if (Token.token == T_ARROW)
		return (member_access(1));

	// A variable. Check that the variable exists.
	if ((varptr = findsymbol(Text)) == NULL || varptr->stype != S_VARIABLE)
		fatals("Unknown variable", Text);

	switch (Token.token) {
		// Post-increment: skip over the token
		case T_INC:
			scan(&Token);
			n = mkastleaf(A_POSTINC, varptr->type, varptr, 0);
			break;

		// Post-decrement: skip over the token
		case T_DEC:
			scan(&Token);
			n = mkastleaf(A_POSTDEC, varptr->type, varptr, 0);
			break;

		// Just a variable reference
		default:
			n = mkastleaf(A_IDENT, varptr->type, varptr, 0);
	}
	return (n);
}

// Parse a primary factor and return an AST node representing it.
static struct ASTnode *primary(void) {
  	struct ASTnode *n;
  	int id;
  
	switch (Token.token) {
  		case T_INTLIT:
			// For an INTLIT token, make a leaf AST node for it.
			// Make it a P_CHAR if it's within the P_CHAR range
			if ((Token.intvalue) >= 0 && (Token.intvalue < 256))
    				n = mkastleaf(A_INTLIT, P_CHAR, NULL, Token.intvalue);
			else
				n = mkastleaf(A_INTLIT, P_INT, NULL, Token.intvalue);
			break;
		
		case T_STRLIT:
			// For a STRLIT token, generate the assembly for it.
			// Then make a leaf AST node for it. id is the string's label.
			id = genglobstr(Text);
			n = mkastleaf(A_STRLIT, pointer_to(P_CHAR), NULL, id);
			break;

		case T_IDENT:
			return (postfix());
		
		case T_LPAREN:
			// Beginning of a parethesised expression, skip the '('. Scan in the expression
			// and the right parenthesis
			scan(&Token);
			n = binexpr(0);
			rparen();
			return (n);

		default:
    			fatald("Expecting a primary expression, got token", Token.token);
	}

	// Scan in the next token and return the leaf node
	scan(&Token);
	return (n);
}

// Convert a binary operator token into an AST operation. We rely on a 1:1 mapping from token to AST operation
static int binastop(int tokentype) {
	if (tokentype > T_EOF && tokentype <= T_SLASH)
		return (tokentype);
	fatald("Syntax error, token", tokentype);
	return (0);		// Keep -Wall happy
}

// Return true if a token is right-associative, false otherwise.
static int rightassoc(int tokentype) {
	if (tokentype == T_ASSIGN)
		return (1);
	return (0);
}

// Operator precedence for each token
// Must match up with the order of tokens in defs.h
static int OpPrec[] = { 0,		// T_EOF, 
			10, 20, 30,	// T_ASSIGN, T_LOGOR, T_LOGAND
			40, 50, 60,	// T_OR, T_XOR, T_AMPER
			70, 70,		// T_EQ, T_NE
			80, 80, 80, 80,	// T_LT, T_GT, T_LE, T_GE
			90, 90,		// T_LSHIFT, T_RSHIFT
			100, 100,	// T_PLUS, T_MINUS
			110, 110	// T_STAR, T_SLASH	
		      };

// Check that we have a binary operator and return its precedence.
static int op_precedence(int tokentype) {
	int prec;
	if (tokentype > T_SLASH)
		fatald("Token with no precedence in op_precedence:", tokentype);
	prec = OpPrec[tokentype];
  	if (prec == 0)
    		fatald("Syntax error, token", tokentype);
  	return (prec);
}

// Parse a prefix expression and return a sub-tree representing it
struct ASTnode *prefix(void) {
	struct ASTnode *tree;
	switch (Token.token) {
		case T_AMPER:
			// Get the next token and parse it recursively as a prefix expression
			scan(&Token);
			tree = prefix();
			
			// Ensure that it's an identifier
			if (tree->op != A_IDENT)
				fatal("& operator must be followed by an identifier");
	
			// Now change the operator to A_ADDR and the type to a pointer to the original type
			tree->op = A_ADDR;
			tree->type = pointer_to(tree->type);
			break;

		case T_STAR:
			// Get the next token and parse it recursively as a prefix expression
			scan(&Token);
			tree = prefix();
	
			// For now, ensure it's either another deref or an identifier
			if (tree->op != A_IDENT && tree->op != A_DEREF)
				fatal("* operator must be followed by an identifier or *");

			// Prepend an A_DEREF operation to the tree
			tree = mkastunary(A_DEREF, value_at(tree->type), tree, NULL, 0);
			break;

		case T_MINUS:
      			// Get the next token and parse it recursively as a prefix expression
			scan(&Token);
      			tree = prefix();

      			// Prepend a A_NEGATE operation to the tree and make the child an rvalue. Because chars are unsigned,
      			// also widen this to int so that it's signed
      			tree->rvalue = 1;
      			tree = modify_type(tree, P_INT, 0);
      			tree = mkastunary(A_NEGATE, tree->type, tree, NULL, 0);
      			break;

    		case T_INVERT:
      			// Get the next token and parse it recursively as a prefix expression
      			scan(&Token);
      			tree = prefix();

      			// Prepend a A_INVERT operation to the tree and make the child an rvalue.
      			tree->rvalue = 1;
      			tree = mkastunary(A_INVERT, tree->type, tree, NULL, 0);
      			break;

    		case T_LOGNOT:
      			// Get the next token and parse it recursively as a prefix expression
      			scan(&Token);
      			tree = prefix();

      			// Prepend a A_LOGNOT operation to the tree and	make the child an rvalue.
      			tree->rvalue = 1;
      			tree = mkastunary(A_LOGNOT, tree->type, tree, NULL, 0);
      			break;

		case T_INC:
      			// Get the next token and parse it recursively as a prefix expression
      			scan(&Token);
      			tree = prefix();

      			// For now, ensure it's an identifier
      			if (tree->op != A_IDENT)
				fatal("++ operator must be followed by an identifier");

      			// Prepend an A_PREINC operation to the tree
      			tree = mkastunary(A_PREINC, tree->type, tree, NULL, 0);
      			break;

		case T_DEC:
      			// Get the next token and parse it recursively as a prefix expression
      			scan(&Token);
      			tree = prefix();

      			// For now, ensure it's an identifier
      			if (tree->op != A_IDENT)
				fatal("-- operator must be followed by an identifier");

      			// Prepend an A_PREDEC operation to the tree
      			tree = mkastunary(A_PREDEC, tree->type, tree, NULL, 0);
      			break;

		default:
			tree = primary();
	}
	return (tree);
}

// Return an AST tree whose root is a binary operator. Parameter ptp is the previous token's precedence.
struct ASTnode *binexpr(int ptp) {
	struct ASTnode *left, *right;
	struct ASTnode *ltemp, *rtemp;
	int ASTop, tokentype;

	// Get the tree on the left. Fetch the next token at the same time.
	left = prefix();

	// If we hit one of several terminating tokens, return just the left node
  	tokentype = Token.token;
  	
	// If we hit a semicolon or ')', return just the left node
  	if (tokentype == T_SEMI || tokentype == T_RPAREN ||
	    tokentype == T_RBRACKET || tokentype == T_COMMA) {
    		left->rvalue = 1;
		return (left);
	}

  	// While the precedence of this token is more than that of the previous token precedence,
	// or it's right associative and equal to the previous token's precedence
  	while ((op_precedence(tokentype) > ptp) ||
	       (rightassoc(tokentype) && op_precedence(tokentype) == ptp)) {
    		// Fetch in the next integer literal
    		scan(&Token);

    		// Recursively call binexpr() with the precedence of our token to build a sub-tree
    		right = binexpr(OpPrec[tokentype]);

		// Determine the operation to be performed on the sub-trees
		ASTop = binastop(tokentype);

		if (ASTop == A_ASSIGN) {
			// Assignment
			// Make the right tree into an rvalue
			right->rvalue = 1;
	
			// Ensure the right's type matches the left
			right = modify_type(right, left->type, 0);
			if (right == NULL)
				fatal("Incompatible expression in assignment");
			
			// Make an assignment AST tree. However, switch left and right around, so that the right
			// expression's code will be generated before the left expression
			ltemp = left;
			left = right;
			right = ltemp;
		} else {
			// We are not doing an assignment, so both trees should be rvalues. Convert both trees
			// into rvalue if they are lvalue trees
			left->rvalue = 1;
			right->rvalue = 1;

    			// Ensure the two types are compatible by trying to modify each tree to match the other's type
			ltemp = modify_type(left, right->type, ASTop);
			rtemp = modify_type(right, left->type, ASTop);
			if (ltemp == NULL && rtemp == NULL)
				fatal("Incompatible types in binary expression");
			if (ltemp != NULL)
				left = ltemp;
			if (rtemp != NULL)
				right = rtemp;
		}

    		// Join that sub-tree with ours. Convert the token into an AST operation at the same time.
    		left = mkastnode(binastop(tokentype), left->type, left, NULL, right, NULL, 0);

    		// Update the details of the current token. If we hit a semicolon, ')', ']', return just the left node
    		tokentype = Token.token;
    		if (tokentype == T_SEMI || tokentype == T_RPAREN ||
		    tokentype == T_RBRACKET || tokentype == T_COMMA) {
			left->rvalue = 1;
      			return (left);
		}
  	}

  	// Return the tree we have when the precedence is the same or lower
  	left->rvalue = 1;
	return (left);
}
