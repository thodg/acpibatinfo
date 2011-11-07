#
# Copyright (c) 2011 Thomas de Grivel <thomas@lowh.net>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

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
