#include "defs.h"
#include "data.h"
#include "decl.h"

// Determine if the symbol is in the global symbol table. Return its slot position or -1 if not found.
int findglob(char *s) {
	int i;

	for (i = 0; i < Globs; i++) {
		if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
			return (i);
	}
	return (-1);
}

// Get position of a new global symbol slot, or die if we've run out of positions.
static int newglob(void) {
	int p;
	
	if ((p = Globs++) >= NSYMBOLS)
		fatal("Too many global symbols");
	return (p);
}

// Add a global symbol to the symbol table. Return the slot number in the symbol table
int addglob(char *name, int type, int stype, int endlabel, int size) {
	int y;

	// If this is already in the symbol table, return the existing slot
	if ((y = findglob(name)) != -1)
		return (y);

	//Otherwise get a new slot, fill it in and return the slot number
	y = newglob();
	Gsym[y].name = strdup(name);
	Gsym[y].type = type;
	Gsym[y].stype = stype;
	Gsym[y].endlabel = endlabel;
	Gsym[y].size = size;
	return (y);
}
