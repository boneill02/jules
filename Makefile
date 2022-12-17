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
	tar -czf jules.tar.gz LICENSE README.md ${SRC} scripts/ Makefile config.mk


install: all
	cp jules $(DESTDIR)$(PREFIX)/bin/jules
	mkdir -p $(DESTDIR)$(PREFIX)/share/jules
	cp scripts/* $(DESTDIR)$(PREFIX)/share/jules/

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/jules $(DESTDIR)$(PREFIX)/share/jules

.PHONY: all clean dist install uninstall
