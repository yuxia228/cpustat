#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

struct timespec nanosleep_ts = { 1, 0 };

void usage(int argc, char * const argv[])
{
	fprintf(stderr, "Usage:\n  %s [OPTION]...\n", argv[0]);	
	exit(-1);
}

int parse_opt(int argc, char * const argv[])
{
	int c;
	long nsec;
	char *endptr;

	while (1) {
		int option_index = 0;
		static const struct option long_options[] = {
			{"delay",	required_argument, 0,  0 },
			{"help",	no_argument,	   0,  0 },
			{0,			0,				   0,  0 }
		};

		c = getopt_long(argc, argv, "d:h",
				 long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'd':
			nanosleep_ts.tv_sec = (time_t)strtoul(optarg,&endptr,10);
			if( endptr )
			{
				nsec = (long)(1000*1000*1000*strtod(endptr, NULL));
				if( nsec > 999999999 ) nsec = 999999999;
				nanosleep_ts.tv_nsec = nsec;
			}
			break;
		case 'h':
		case '?':
		default:
			usage(argc, argv);
		}
	}
	return 0;
}

int main(int argc, char * const argv[])
{
	parse_opt(argc, argv);
	printf("delay = %ld.%09ld sec\n", (long)nanosleep_ts.tv_sec, nanosleep_ts.tv_nsec);
	nanosleep(&nanosleep_ts, NULL);
	return 0;
}

