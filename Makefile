# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = ashley.c
OBJ = ${SRC:.c=.o}

all: options ashley

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

ashley: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ashley ${OBJ}

dist:
	@echo "UNIMPLEMENTED"

install: all
	@echo "UNIMPLEMENTED"

uninstall:
	@echo "UNIMPLEMENTED"

.PHONY: all options clean dist install uninstall
