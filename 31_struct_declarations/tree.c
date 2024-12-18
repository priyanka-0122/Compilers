#include "defs.h"
#include "data.h"
#include "decl.h"

// Build and return a generic AST node
struct ASTnode *mkastnode(int op, int type,
			struct ASTnode *left,
			struct ASTnode *mid,
			struct ASTnode *right,
			struct symtable *sym, int intvalue) {
	struct ASTnode *n;
	
	//Calloc a new ASTnode
	n = (struct ASTnode *)calloc(1, sizeof(struct ASTnode));
	if (n == NULL) {
		fprintf(stderr, "Unable to calloc a node in mkastnode()");
		exit(1);
	}
	
	// Copy in the field values and return it
	n->op = op;
	n->type = type;
	n->left = left;
	n->mid = mid;
	n->right = right;
	n->sym = sym;
	n->intvalue = intvalue;
	return (n);
}

// Make an AST leaf node
struct ASTnode *mkastleaf(int op, int type, struct symtable *sym, int intvalue) {
	return (mkastnode(op, type, NULL, NULL, NULL, sym, intvalue));
}

// Make a unary AST node with only one child
struct ASTnode *mkastunary(int op, int type, struct ASTnode *left, struct symtable *sym, int intvalue) {
	return (mkastnode(op, type, left, NULL, NULL, sym, intvalue));
}

// Generate and return a new label number just for AST dumping purposes
static int gendumplabel(void) {
	static int id = 1;
	return (id++);
}

// Given an AST tree, print it out and follow the traversal of the tree that genAST() follows

void dumpAST(struct ASTnode *n, int label, int level) {
	int Lfalse, Lstart, Lend;

	switch (n->op) {
		case A_IF:
      			Lfalse = gendumplabel();
      			for (int i = 0; i < level; i++)
				      fprintf(stdout, " ");
      			fprintf(stdout, "A_IF");
      			if (n->right) {
				      Lend = gendumplabel();
        			fprintf(stdout, ", end L%d", Lend);
      			}
      			fprintf(stdout, "\n");

      			dumpAST(n->left, Lfalse, level + 2);
      			dumpAST(n->mid, NOLABEL, level + 2);
      			if (n->right)
				      dumpAST(n->right, NOLABEL, level + 2);
      			return;
    		case A_WHILE:
      			Lstart = gendumplabel();
      			for (int i = 0; i < level; i++)
				      fprintf(stdout, " ");
      			fprintf(stdout, "A_WHILE, start L%d\n", Lstart);
      			Lend = gendumplabel();
      			dumpAST(n->left, Lend, level + 2);
      			dumpAST(n->right, NOLABEL, level + 2);
      			return;
  	}

  	// Reset level to -2 for A_GLUE
  	if (n->op == A_GLUE)
		level = -2;

  	// General AST node handling
  	if (n->left)
		dumpAST(n->left, NOLABEL, level + 2);
  	if (n->right)
		dumpAST(n->right, NOLABEL, level + 2);

  	for (int i = 0; i < level; i++)
		fprintf(stdout, " ");
  	switch (n->op) {
    		case A_GLUE:
      			fprintf(stdout, "\n\n");
			return;
    		case A_FUNCTION:
      			fprintf(stdout, "A_FUNCTION %s\n", n->sym->name);
			return;
    		case A_ADD:
      			fprintf(stdout, "A_ADD\n");
			return;
    		case A_SUBTRACT:
      			fprintf(stdout, "A_SUBTRACT\n");
			return;
    		case A_MULTIPLY:
      			fprintf(stdout, "A_MULTIPLY\n");
			return;
    		case A_DIVIDE:
      			fprintf(stdout, "A_DIVIDE\n");
			return;
    		case A_EQ:
      			fprintf(stdout, "A_EQ\n");
			return;
    		case A_NE:
      			fprintf(stdout, "A_NE\n");
			return;
    		case A_LT:
      			fprintf(stdout, "A_LE\n");
			return;
    		case A_GT:
      			fprintf(stdout, "A_GT\n");
			return;
    		case A_LE:
      			fprintf(stdout, "A_LE\n");
			return;
    		case A_GE:
      			fprintf(stdout, "A_GE\n");
			return;
    		case A_INTLIT:
      			fprintf(stdout, "A_INTLIT %d\n", n->intvalue);
			return;
		case A_STRLIT:
			fprintf(stdout, "A_STRLIT rval label L%d\n", n->intvalue);
			return;
    		case A_IDENT:
      			if (n->rvalue)
        			fprintf(stdout, "A_IDENT rval %s\n", n->sym->name);
      			else
        			fprintf(stdout, "A_IDENT %s\n", n->sym->name);
      			return;
    		case A_ASSIGN:
      			fprintf(stdout, "A_ASSIGN\n");
			return;
    		case A_WIDEN:
      			fprintf(stdout, "A_WIDEN\n");
			return;
    		case A_RETURN:
      			fprintf(stdout, "A_RETURN\n");
			return;
    		case A_FUNCCALL:
      			fprintf(stdout, "A_FUNCCALL %s\n", n->sym->name);
			return;
    		case A_ADDR:
      			fprintf(stdout, "A_ADDR %s\n", n->sym->name);
			return;
    		case A_DEREF:
      			if (n->rvalue)
        			fprintf(stdout, "A_DEREF rval\n");
      			else
        			fprintf(stdout, "A_DEREF\n");
      			return;
    		case A_SCALE:
      			fprintf(stdout, "A_SCALE %d\n", n->size);
			return;
		case A_PREINC:
			fprintf(stdout, "A_PREINC %s\n", n->sym->name);
			return;
		case A_PREDEC:
			fprintf(stdout, "A_PREDEC %s\n", n->sym->name);
			return;
		case A_POSTINC:
			fprintf(stdout, "A_POSTINC\n");
			return;
		case A_POSTDEC:
			fprintf(stdout, "A_POSTDEC\n");
			return;
		case A_NEGATE:
			fprintf(stdout, "A_NEGATE\n");
			return;
    		default:
      			fatald("Unknown dumpAST operator", n->op);
  	}
}
