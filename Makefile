include config.mk

SRC = jules.c
OBJ = ${SRC:.c=.o}

all: jules

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

jules: ${OBJ}
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
