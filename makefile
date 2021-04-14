CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
STRIP=$(CROSS_COMPILE)strip
CFLAGS_OPT=-O2
CFLAGS_WARN=-Wall -Wextra -Wno-unused-parameter -Wformat=2 -Wstrict-aliasing=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum
CFLAGS=${CFLAGS_OPT} ${CFLAGS_WARN} $(EXTRA_CFLAGS)

all:
	${CC} ${CFLAGS} ${LFLAGS} -o cpustat.exe cpustat.c

