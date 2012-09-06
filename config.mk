# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

VERSION= $(shell git describe)

# includes and libs
INCS = $(shell pkg-config --cflags dbus-1)
LIBS = $(shell pkg-config --libs dbus-1)

# flags
CPPFLAGS += -D_BSD_SOURCE -DVERSION=\"${VERSION}\"
CFLAGS   += -g --std=c99 -pedantic -Wall -Wno-overlength-strings -Os ${INCS} ${CPPFLAGS} ${EXTRACFLAGS}
LDFLAGS  += ${LIBS}
