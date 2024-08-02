#include "defs.h"
#include "data.h"
#include "decl.h"

static struct ASTnode *print_statement(void) {
  	struct ASTnode *tree;
  	int reg;

  	// Match a 'print' as the first token
  	match(T_PRINT, "print");

  	// Parse the following expression and generate the assembly code
  	tree = binexpr(0);

 	// Make an print AST tree
	tree = mkastunary(A_PRINT, tree, 0);

	// Match the following semicolon and return the AST
  	semi();
	return (tree);
}

static struct ASTnode *assignment_statement(void) {
  	struct ASTnode *left, *right, *tree;
  	int id;

  	// Ensure we have an identifier
  	ident();

  	// Check it's been defined then make a leaf node for it
  	if ((id = findglob(Text)) == -1) {
    		fatals("Undeclared variable", Text);
  	}
  	right = mkastleaf(A_LVIDENT, id);

  	// Ensure we have an equals sign
  	match(T_ASSIGN, "=");

  	// Parse the following expression
  	left = binexpr(0);

  	// Make an assignment AST tree
  	tree = mkastnode(A_ASSIGN, left, NULL, right, 0);

  	// Match the following semicolon and return the AST
  	semi();
	return (tree);
}

// Parse an If statement including any optional ELSE clause and return its AST
struct ASTnode *if_statement(void) {
	struct ASTnode *condAST, *trueAST, *falseAST = NULL;

	// Ensure we have 'if' '('
	match(T_IF, "if");
	lparen();

	// Parse the following expression and the ')' following. Ensure the
	// tree's operation is a comparison
	condAST = binexpr(0);

	if (condAST->op < A_EQ || condAST->op > A_GE)
		fatal("Bad comparison operator");
	rparen();

	// Get the AST for the compound statement
	trueAST = compound_statement();

	// If we have an 'else', skip it and get the AST for the compund statement
	if (Token.token == T_ELSE) {
		scan(&Token);
		falseAST = compound_statement();
	}

	// Build and return the AST for this statement
	return (mkastnode(A_IF, condAST, trueAST, falseAST, 0));
}

// Parse a WHILE statement and return its AST
struct ASTnode *while_statement(void) {
	struct ASTnode *condAST, *bodyAST;

	// Ensure we have 'while' '('
	match(T_WHILE, "while");
	lparen();
	
	// Parse the following expression and the ')' following. Ensure
	// the tree's operation is a comparison
	condAST = binexpr(0);
	if (condAST->op < A_EQ || condAST->op > A_GE)
		fatal("Bad comparison operator");
	rparen();

	// Get the AST for the compound statement
	bodyAST = compound_statement();
	
	// Build and return the AST for this statement
	return (mkastnode(A_WHILE, condAST, NULL, bodyAST,0));
}

// Parse a DO WHILE statement and return its AST
struct ASTnode *do_while_statement(void) {
	struct ASTnode *condAST, *bodyAST;
	
	// Ensure we have 'do'
	match(T_DO, "do");

	// Get the AST for the compound
	bodyAST = compound_statement();

	// Ensure we have 'while' '('
	match(T_WHILE, "while");
	lparen();

	// Parse the following expression and the ')' following. Ensure
	// the tree's operation is a ecomparison
	condAST = binexpr(0);
	if (condAST->op < A_EQ || condAST->op > A_GE)
		fatal("Bad comparison operator");
	rparen();
	semi();

	// Build and return the AST for this statement
	return (mkastnode(A_DO_WHILE, condAST, NULL, bodyAST, 0));
}

// Parse a compound statement and return its AST
struct ASTnode *compound_statement(void) {
	struct ASTnode *left = NULL;
	struct ASTnode *tree;

	//Require a left curly bracket
	lbrace();
	while (1) {
    		switch (Token.token) {
    			case T_PRINT:
      				tree = print_statement();
      				break;
    			case T_INT:
      				var_declaration();
				tree = NULL;	// No AST generated here      			
				break;
    			case T_IDENT:
      				tree = assignment_statement();
      				break;
    			case T_IF:
				tree = if_statement();
      				break;
			case T_WHILE:
				tree = while_statement();
				break;
			case T_DO:
				tree = do_while_statement();
				break;
			case T_RBRACE:
				// When we hit a right curly bracket, skip past it an return the AST
				rbrace();
				return (left);
    			default:
      				fatald("Syntax error, token", Token.token);
    		}
		
		// For each new tree, either save it in left if left is empty, or glue the left and the 
		// new tree together
		if (tree) {
			if (left == NULL)
				left = tree;
			else
				left = mkastnode(A_GLUE, left, NULL ,tree, 0);

		}
	}	
}
