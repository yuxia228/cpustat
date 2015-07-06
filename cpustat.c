#include <stdio.h>
#include <stdlib.h>


void usage(int argc, const char *argv[])
{
	fprintf(stderr, "Usage:\n  %s [OPTION]...\n", argv[0]);	
	exit(-1);
}

int main(int argc, const char *argv[])
{
	usage(argc, argv);
	return 0;
}

