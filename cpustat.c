#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <sys/time.h>

struct timespec nanosleep_ts = { 1, 0 };

struct cpu_stat {
	unsigned long user;
	unsigned long nice;
	unsigned long sys;
	unsigned long idle;
	unsigned long iowait;
	unsigned long irq;
	unsigned long softirq;
};

struct cpu_stat last_cpu_stat = {0};

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

void show_cpu_rate(struct cpu_stat *last, struct cpu_stat *current)
{
	unsigned long total;
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);
	printf("%04d/%02d/%02d %02d:%02d:%02d.%06d ",
		tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec, (int)tv.tv_usec);

	total = current->user - last->user +
		current->nice - last->nice +
		current->sys - last->sys +
		current->idle - last->idle +
		current->iowait - last->iowait +
		current->irq - last->irq +
		current->softirq - last->softirq;
	if( 0 == total )
	{
		printf("- - - -\n");
		return;
	}
	printf("%ld.%02ld%% %ld.%02ld%% %ld.%02ld%% %ld.%02ld%%\n",
		(((current->user - last->user)*10000)/total)/100,
		(((current->user - last->user)*10000)/total)%100,
		(((current->nice - last->nice)*10000)/total)/100,
		(((current->nice - last->nice)*10000)/total)%100,
		(((current->sys - last->sys)*10000)/total)/100,
		(((current->sys - last->sys)*10000)/total)%100,
		(((current->idle - last->idle)*10000)/total)/100,
		(((current->idle - last->idle)*10000)/total)%100);
	return;
}

void monitor(void)
{
	FILE *fp;
	struct cpu_stat cpu;
	int n;
	printf("date time user nice sys idle\n");
	for(;;)
	{
		fp = fopen("/proc/stat", "r" );
		if ( NULL == fp ) {
			perror("fopen");
			break;
		}
		n = fscanf(fp, "cpu  %ld %ld %ld %ld %ld %ld %ld",
			&cpu.user, &cpu.nice, &cpu.sys, &cpu.idle, &cpu.iowait, &cpu.irq, &cpu.softirq );
		if ( errno )
		{
			perror("fscanf");
			break;
		}
		if ( n != 7 )
		{
			fprintf(stderr, "fscanf match error (%d)\n",n);
			break;
		}
		fclose(fp);
		show_cpu_rate(&last_cpu_stat, &cpu);
		last_cpu_stat = cpu;
		nanosleep(&nanosleep_ts, NULL);
	}
}

int main(int argc, char * const argv[])
{
	parse_opt(argc, argv);
	printf("delay = %ld.%09ld sec\n", (long)nanosleep_ts.tv_sec, nanosleep_ts.tv_nsec);
	monitor();
	return 0;
}

