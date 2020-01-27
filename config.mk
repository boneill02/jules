# paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
CFLAGS   = -g -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#CFLAGS   = -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
