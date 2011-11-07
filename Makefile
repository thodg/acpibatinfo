
PROG = acpibatinfo
CFLAGS += -W -Wall -g
PREFIX := /usr/local

build: ${PROG}

OBJ = acpibatinfo.o

${PROG}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJ} -o $@

clean:
	rm -f ${PROG} ${OBJ}

install: ${PROG}
	install -d -o root -g wheel ${PREFIX}/bin
	install -c -o root -g wheel ${PROG} ${PREFIX}/bin
