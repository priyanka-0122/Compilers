int c;
int d;
int *e;
int f;

int main() {
  	c = 12;
	printint(c);
	d = 13;
	printint(d);
	e = &c;
	printint(e);
	e = &c + 1;
	printint(e);
  	return(0);
}
