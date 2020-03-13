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
	rm -f jules ${OBJ}

dist:
	@echo "UNIMPLEMENTED"

install: all
	cp jules $(DESTDIR)$(PREFIX)/bin/jules
	mkdir -p $(DESTDIR)$(PREFIX)/share/jules
	cp -r scripts $(DESTDIR)$(PREFIX)/share/jules/

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/jules $(DESTDIR)$(PREFIX)/share/jules
	@echo "UNIMPLEMENTED"

.PHONY: all options clean dist install uninstall
