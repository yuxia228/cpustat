CFLAGS_OPT=-O2
CFLAGS_WARN=-Wall -Wextra -Wno-unused-parameter -Wformat=2 -Wstrict-aliasing=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum
CFLAGS=${CFLAGS_OPT} ${CFLAGS_WARN}

all:
	gcc ${CFLAGS} -o cpustat.exe cpustat.c

