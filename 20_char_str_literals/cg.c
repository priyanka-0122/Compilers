#include "defs.h"
#include "data.h"
#include "decl.h"

// List of avalable registers and their names
static int freereg[4];
static char *reglist[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breglist[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *dreglist[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };

// Set all registers as available
void freeall_registers(void)
{
	freereg[0] = freereg[1] = freereg[2] = freereg[3] = 1;
}

// Allocate a free register. Return the number of the register. Die if no available registers.
static int alloc_register(void)
{
	for (int i = 0; i < 4; i++) {
		if (freereg[i]) {
			freereg[i] = 0;
			return (i);
		}
	}
	fatal("Out of registers!");
	return (NOREG);		// Keep -Wall happy
}

// Return a register to the list of available registers. Check to see if it's not already there
static void free_register(int reg)
{
	if (freereg[reg] != 0)
		fatald("Error trying to free register", reg);
	freereg[reg] = 1;
}

// Print out the assembly preamble
void cgpreamble()
{
	freeall_registers();
	fputs("\t.text\n", Outfile);
}

// Nothing to do
void cgpostamble() {
}

// Print out a function preamble
void cgfuncpreamble(int id) {
	char *name = Gsym[id].name;
	fprintf(Outfile,
		"\t.text\n"
		"\t.globl\t%s\n"
		"\t.type\t%s, @function\n"
		"%s:\n" "\tpushq\t%%rbp\n"
		"\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

// Print out a function postamble
void cgfuncpostamble(int id) {
	cglabel(Gsym[id].endlabel);
  	fputs("\tpopq %rbp\n" "\tret\n", Outfile);
}

// Load an integer literal value into a register. Return the number of the register
int cgloadint(int value, int type) {

	// Get a new register
	int r = alloc_register();

	// Print out the code to initialise it
	fprintf(Outfile, "\tmovq\t$%d, %s\n", value, reglist[r]);
	return(r);
}

// Load a value from a variable into a register. Return the number of the register
int cgloadglob(int id) {
	// Get a new register
	int r = alloc_register();

	// Print out the code to initialise it
	switch (Gsym[id].type) {
		case P_CHAR:
			fprintf(Outfile, "\tmovzbq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
			break;
		case P_INT:
			fprintf(Outfile, "\tmovl\t%s(\%%rip), %s\n", Gsym[id].name, dreglist[r]);
			break;
		case P_LONG:
		case P_CHARPTR:
		case P_INTPTR:
		case P_LONGPTR:
			fprintf(Outfile, "\tmovq\t%s(\%%rip), %s\n", Gsym[id].name, reglist[r]);
			break;
		default:
			fatald("Bad type in cgloadglob:", Gsym[id].type);
	}
	return (r);
}

// Given the label number of a global string, load its address into a new register
int cgloadglobstr(int id) {
	// Get a new register
	int r = alloc_register();
	fprintf(Outfile, "\tleaq\tL%d(\%%rip), %s\n", id, reglist[r]);
	return (r);
}

// Add two registers together and return the number of the register with the result
int cgadd(int r1, int r2) {
	fprintf(Outfile, "\taddq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return (r2);
}

// Subtract the second register from the first and return the number of the register with the result
int cgsub(int r1, int r2) {
	fprintf(Outfile, "\tsubq\t%s, %s\n", reglist[r2], reglist[r1]);
	free_register(r2);
	return (r1);
}

// Multiply two registers together and return the number of the register with the result
int cgmul(int r1, int r2) {
	fprintf(Outfile, "\timulq\t%s, %s\n", reglist[r1], reglist[r2]);
	free_register(r1);
	return (r2);
}

// Divide the first register by the second and return the number of the register with the result
int cgdiv(int r1, int r2) {
	fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[r1]);	//dividend is loaded to %rax
	fprintf(Outfile, "\tcqo\n");				//cqo is used to extend to eight bytes
	fprintf(Outfile, "\tdivq\t%s\n", reglist[r2]);		//idivq divides the content in %rax with 
								//the r2 and stores quotient in %rax
	fprintf(Outfile, "\tmovq\t%%rax,%s\n", reglist[r1]);
	free_register(r2);
	return (r1);
}

// Call printint() with the register
void cgprintint(int r) {
	fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]); 	//Linux x86-64 expects te first arg to
								// a function to be %rdi
	fprintf(Outfile, "\tcall\tprintint\n");
	free_register(r);
}

// Call a function with one argument from the given register. Return the register with the result
int cgcall(int r, int id) {

	// Get a new register
	int outr = alloc_register();
	fprintf(Outfile, "\tmovq\t%s, %%rdi\n", reglist[r]);
	fprintf(Outfile, "\tcall\t%s\n", Gsym[id].name);
	fprintf(Outfile, "\tmovq\t%%rax, %s\n", reglist[outr]);
	free_register(r);
	return (outr);
}

// Shift a register left by a constant
int cgshlconst(int r, int val) {
	fprintf(Outfile, "\tsalq\t$%d, %s\n", val, reglist[r]);
	return(r);
}

// Store a register's value into a variable
int cgstorglob(int r, int id) {
	switch (Gsym[id].type) {
		case P_CHAR:
			fprintf(Outfile, "\tmovb\t%s, %s(\%%rip)\n", breglist[r], Gsym[id].name);
			break;
		case P_INT:
			fprintf(Outfile, "\tmovl\t%s, %s(\%%rip)\n", dreglist[r], Gsym[id].name);
			break;
		case P_LONG:
		case P_CHARPTR:
		case P_INTPTR:
		case P_LONGPTR:
			fprintf(Outfile, "\tmovq\t%s, %s(\%%rip)\n", reglist[r], Gsym[id].name);
			break;
		default:
			fatald("Bad type in cgstorglob:", Gsym[id].type);
	}
	return (r);
}

// Array of type sizes in P_XXX order. 0 means no size.
//		  P_NONE, P_VOID, P_CHAR, P_INT, P_LONG, P_VOIDPTR, P_CHARPTR, P_INTPTR, P_LONGPTR
static int psize[] = { 0, 0, 1, 4, 8, 8, 8, 8, 8};

// Given a P_XXX type value, return the size of a primitive type in bytes.
int cgprimsize(int type) {
	// Check the type is valid
	if (type < P_NONE || type > P_LONGPTR)
		fatal("Bad type in cgprimsize()");
	return (psize[type]);
}

// Generate a global symbol
void cgglobsym(int id) {
	int typesize;
	// Get the size of the type
	typesize = cgprimsize(Gsym[id].type);
	
	// Generate the global identity and teh label
	fprintf(Outfile, "\t.data\n" "\t.globl\t%s\n", Gsym[id].name);
	fprintf(Outfile, "%s:", Gsym[id].name);

	// Generate the space
	for ( int i = 0; i < Gsym[id].size; i++) {
		switch(typesize) {
    			case 1:
				fprintf(Outfile, "\t.byte\t0\n");
				break;
    			case 4:
				fprintf(Outfile, "\t.long\t0\n");
				break;
			case 8:
				fprintf(Outfile, "\t.quad\t0\n");
				break;
    			default:
				fatald("Unknown typesize in cgglobsym: ", typesize);
		}
  	}
}

// Generate a global string and its start label
void cgglobstr(int l, char *strvalue) {
	char *cptr;
	cglabel(l);
	for (cptr = strvalue; *cptr; cptr++) {
		fprintf(Outfile, "\t.byte\t%d\n", *cptr);
	}
	fprintf(Outfile, "\t.byte\t0\n");
}

// List of comparisons instructions in AST order:
//			     A_EQ,  A_NE,    A_LT,   A_GT,    A_LE, 	A_GE
static char *cmplist[] = { "sete", "setne", "setl", "setg", "setle", "setge"};

// Compare two registers and set if true.
int cgcompare_and_set(int ASTop, int r1, int r2) {
	//Check two randge of the AST operation
	if (ASTop < A_EQ || ASTop > A_GE)
		fatal("Bad ASTop in cgcompare_and_set()");
	fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
	fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQ], breglist[r2]);
	fprintf(Outfile, "\tmovzbq\t%s, %s\n", breglist[r2], reglist[r2]);
	free_register(r1);
	return (r2);
}

// Generate a label
void cglabel(int l) {
	fprintf(Outfile, "L%d:\n", l);
}

// Generate a jump to a label
void cgjump(int l) {
	fprintf(Outfile, "\tjmp\tL%d\n", l);
}

// List of inverted jump instructions in AST order:
//			       A_EQ, A_NE,  A_LT,  A_GT, A_LE, A_GE
static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

// Compare two registers and jump if false.
int cgcompare_and_jump(int ASTop, int r1, int r2, int label) {
	// Check the range of the AST operation
	if (ASTop < A_EQ || ASTop > A_GE)
		fatal("Bad ASTop in cgcompare_and_jump()");

	fprintf(Outfile, "\tcmpq\t%s, %s\n", reglist[r2], reglist[r1]);
	fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - A_EQ], label);
	freeall_registers();
	return (NOREG);
}

// Widen the value in the register from the old to the new type, and return a register with this new value
int cgwiden (int r, int oldtype, int newtype) {
	// Nothing to do
	return r;
}

// Generate code to return a value from a function
void cgreturn(int reg, int id) {
	// Generate code depending on the function's type
	switch (Gsym[id].type) {
		case P_CHAR:
			fprintf(Outfile, "\tmovzbl\t%s, %%eax\n", breglist[reg]);
			break;
		case P_INT:
			fprintf(Outfile, "\tmovl\t%s, %%eax\n", dreglist[reg]);
			break;
		case P_LONG:
			fprintf(Outfile, "\tmovq\t%s, %%rax\n", reglist[reg]);
			break;
		default:
			fatald("Bad function type in cgreturn:", Gsym[id].type);
	}
	cgjump(Gsym[id].endlabel);
}

// Generate code to load the address of a global identifier into a variable. Return a new register
int cgaddress(int id) {
	int r = alloc_register();

	fprintf(Outfile, "\tleaq\t%s(%%rip), %s\n", Gsym[id].name, reglist[r]);	//leaq loads the address of the named identifier
	return (r);
}

// Dereference a pointer to get the value it is pointing at into the same register
int cgderef(int r, int type) {
	switch (type) {
		case P_CHARPTR:
			fprintf(Outfile, "\tmovzbq\t(%s), %s\n", reglist[r], reglist[r]);
			break;
		case P_INTPTR:
			fprintf(Outfile, "\tmovl\t(%s), %s\n", reglist[r], dreglist[r]);
			break;
		case P_LONGPTR:
			fprintf(Outfile, "\tmovq\t(%s), %s\n", reglist[r], reglist[r]);
			break;
	}
	return (r);
}

// Store through a dereferenced pointer
int cgstorderef(int r1, int r2, int type) {
  	switch (type) {
    		case P_CHAR:
      			fprintf(Outfile, "\tmovb\t%s, (%s)\n", breglist[r1], reglist[r2]);
      			break;
    		case P_INT:
      			fprintf(Outfile, "\tmovl\t%s, (%s)\n", dreglist[r1], reglist[r2]);
      			break;
    		case P_LONG:
      			fprintf(Outfile, "\tmovq\t%s, (%s)\n", reglist[r1], reglist[r2]);
      			break;
    		default:
      			fatald("Can't cgstoderef on type:", type);
  	}
  	return (r1);
}
