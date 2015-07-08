#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <sys/time.h>

struct timespec nanosleep_ts = { 1, 0 };
unsigned long long cpumask = 0;
int flag_accumulation = 0;

struct cpu_stat {
	unsigned long user;
	unsigned long nice;
	unsigned long sys;
	unsigned long idle;
	unsigned long iowait;
	unsigned long irq;
	unsigned long softirq;
};

const struct cpu_stat INITIAL_CPU_STAT = {0};
struct cpu_stat last_cpu_stat = {0};
struct cpu_stat accum_cpu_stat = {0};
struct cpu_stat last_each_cpu_stat[64] = {{0}};

const char STAT_HEADER[] = "date       time            user% nice%  sys% idle%";

void usage(int argc, char * const argv[])
{
	fprintf(stderr, "Usage:\n  %s [OPTION]...\n\n", argv[0]);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -d, --delay=SEC      delay-time interval\n");
	fprintf(stderr, "  -c, --cpumask=MASK   specify cpu-mask\n");
	fprintf(stderr, "  -a, --accumulation   accumulate each cpus if cpu-mask is specified\n");
	fprintf(stderr, "  -h, --help           display this help\n");
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
			{"cpumask",	required_argument, 0,  0 },
			{"accumulation", no_argument,  0,  0 },
			{"help",	no_argument,	   0,  0 },
			{0,			0,				   0,  0 }
		};

		c = getopt_long(argc, argv, "c:d:ah",
				 long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'a':
			flag_accumulation = 1;
			break;
		case 'c':
			cpumask = strtoull(optarg, NULL, 16);
			if ( 0 == cpumask )
			{
				fprintf(stderr, "cpumask should not be zero.\n");
				usage(argc, argv);
			}
			break;
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

void accumulate_cpu_stat( struct cpu_stat * const accum, const struct cpu_stat * const current )
{
	accum->user += current->user;
	accum->nice += current->nice;
	accum->sys += current->sys;
	accum->idle += current->idle;
	accum->iowait += current->iowait;
	accum->irq += current->irq;
	accum->softirq += current->softirq;
	return;
}

void show_cpu_rate( const struct cpu_stat * const last, const struct cpu_stat * const current)
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

void monitor_each_cpu(void)
{
	FILE *fp;
	struct cpu_stat cpu;
	int n;
	unsigned int maxbit=0;
	unsigned long long b, cpuno;
	char buf[1024];

	for(b = cpumask ; b!=0 ; b>>=1, maxbit++);

	if ( !flag_accumulation ) printf("cpu# ");
	printf("%s\n", STAT_HEADER);
	for(;;)
	{
		fp = fopen("/proc/stat", "r" );
		if ( NULL == fp ) {
			perror("fopen");
			break;
		}

		/* skip first line */
		if ( NULL == fgets(buf, sizeof(buf), fp) )
		{
			perror("fgets");
			break;
		}
		for(b=0 ; b<maxbit ; b++)
		{
			if ( NULL == fgets(buf, sizeof(buf), fp) )
			{
				perror("fgets");
				break;
			}
			if( !(cpumask & (1ULL<<b)) ) continue;
			n = sscanf(buf, "cpu%llu  %ld %ld %ld %ld %ld %ld %ld",
				&cpuno, &cpu.user, &cpu.nice, &cpu.sys, &cpu.idle,
				&cpu.iowait, &cpu.irq, &cpu.softirq );
			if ( EOF == n )
			{
				perror("sscanf");
				break;
			}
			if ( n != 8 )
			{
				fprintf(stderr, "parse error (%d)\n",n);
				break;
			}
			if ( cpuno != b )
			{
				fprintf(stderr, "number error ? (line:%llu cpu%llu)\n", b+1, cpuno);
			}

			if ( flag_accumulation ) {
				accumulate_cpu_stat( &accum_cpu_stat, &cpu);
			} else {
				printf("cpu%llu ",b);
				show_cpu_rate(&last_each_cpu_stat[b], &cpu);
				last_each_cpu_stat[b] = cpu;
			}
		}
		fclose(fp);
		if ( flag_accumulation ) {
			show_cpu_rate(&last_cpu_stat, &accum_cpu_stat);
			last_cpu_stat = accum_cpu_stat;
			accum_cpu_stat = INITIAL_CPU_STAT;
		}
		fflush(stdout);
		nanosleep(&nanosleep_ts, NULL);
	}
}

void monitor(void)
{
	FILE *fp;
	struct cpu_stat cpu;
	int n;

	printf("%s\n", STAT_HEADER);
	for(;;)
	{
		fp = fopen("/proc/stat", "r" );
		if ( NULL == fp ) {
			perror("fopen");
			break;
		}
		n = fscanf(fp, "cpu  %ld %ld %ld %ld %ld %ld %ld",
			&cpu.user, &cpu.nice, &cpu.sys, &cpu.idle, &cpu.iowait, &cpu.irq, &cpu.softirq );
		if ( EOF == n )
		{
			perror("fscanf");
			break;
		}
		if ( n != 7 )
		{
			fprintf(stderr, "parse error (%d)\n",n);
			break;
		}
		fclose(fp);
		show_cpu_rate(&last_cpu_stat, &cpu);
		last_cpu_stat = cpu;
		fflush(stdout);
		nanosleep(&nanosleep_ts, NULL);
	}
}

int main(int argc, char * const argv[])
{
	parse_opt(argc, argv);
	if( cpumask ) printf("cpumask = %llx\n", cpumask);
	printf("delay = %ld.%09ld sec\n", (long)nanosleep_ts.tv_sec, nanosleep_ts.tv_nsec);
	if( cpumask ) {
		monitor_each_cpu();
	} else {
		monitor();
	}
	return 0;
}

