#include "defs.h"
#include "data.h"
#include "decl.h"

// Parse one or more statements
void statements(void) {
	struct ASTnode *tree;
	int reg;

	while (1) {
		// Match a 'print as the first token
		match(T_PRINT, "print");

		// Parse the following expression and generate the assembly code
		tree = binexpr(0);
		reg = genAST(tree);
		genprintint(reg);
		genfreeregs();

		// Match the followingsemicolon and stop if we are at EOF
		semi();
		if (Token.token == T_EOF)
			return;
	}
}
