# cpustat
Linux CPU Statistics Tool

## Help

```bash
$ ./cpustat.exe --help
Usage:
  ./cpustat.exe [OPTION]...

Options:
  -d, --delay=SEC      delay-time interval
  -c, --cpumask=MASK   specify cpu-mask
  -a, --accumulation   accumulate each cpus if cpu-mask is specified
  -w, --wide           show cpu rate of iowait, irq and softirq
  -t, --tick           show tick instead of cpu rate
  -h, --help           display this help
```

## Example

```bash
$ ./cpustat.exe
delay = 1.000000000 sec
date       time            user% nice%  sys% idle%
2015/07/08 00:10:50.471471 0.51% 0.00% 0.79% 98.26%
2015/07/08 00:10:51.473531 0.00% 0.00% 0.00% 100.00%
2015/07/08 00:10:52.475964 0.00% 0.00% 0.00% 100.00%
2015/07/08 00:10:53.509681 0.00% 0.00% 0.00% 100.00%
2015/07/08 00:10:54.515131 0.00% 0.00% 0.24% 99.75%
( quit by ctrl + c )
```

```bash
$ ./cpustat.exe -d 0.2
delay = 0.200000000 sec
date       time            user% nice%  sys% idle%
2015/07/08 00:12:24.737498 0.48% 0.00% 0.76% 98.33%
2015/07/08 00:12:24.941880 0.00% 0.00% 1.21% 98.78%
2015/07/08 00:12:25.144380 0.00% 0.00% 0.00% 100.00%
2015/07/08 00:12:25.346361 0.00% 0.00% 0.00% 100.00%
```

```bash
$ ./cpustat.exe -c 0c
cpumask = c
delay = 1.000000000 sec
cpu# date       time            user% nice%  sys% idle%
cpu2 2015/07/08 00:14:56.836423 0.31% 0.00% 0.63% 98.78%
cpu3 2015/07/08 00:14:56.836807 0.41% 0.00% 0.68% 98.60%
cpu2 2015/07/08 00:14:57.838844 0.00% 0.00% 1.00% 99.00%
cpu3 2015/07/08 00:14:57.839269 0.00% 0.00% 0.00% 100.00%
cpu2 2015/07/08 00:14:58.841128 0.00% 0.00% 0.00% 100.00%
cpu3 2015/07/08 00:14:58.841340 0.00% 0.00% 0.00% 100.00%
```

```bash 
$ ./cpustat.exe -c 0c -a
cpumask = c
delay = 1.000000000 sec
date       time            user% nice%  sys% idle%
2015/07/08 00:16:04.464295 0.35% 0.00% 0.63% 98.72%
2015/07/08 00:16:05.466384 0.00% 0.00% 0.50% 99.50%
2015/07/08 00:16:06.472750 0.00% 0.00% 0.00% 100.00%
2015/07/08 00:16:07.475505 0.00% 0.00% 0.49% 99.50%
```

