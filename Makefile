# dunst-checkmail - Ultra-simple mail notification via dbus
# See LICENSE file for copyright and license details.

include config.mk

SRC = dunst-checkmail.c
OBJ = ${SRC:.c=.o}

all: doc options dunst-checkmail

options:
	@echo dunst-checkmail build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

dunst-checkmail: dunst-checkmail.o
	@echo CC -o $@
	@${CC} ${CFLAGS} -o $@ dunst-checkmail.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${OBJ}
	@rm -f dunst-checkmail

doc: dunst-checkmail.1
dunst-checkmail.1: README.pod
	pod2man --name=dunst-checkmail -c "Dunst Checkmail Reference" --section=1 --release=${VERSION} $< > $@

install: all
	@echo installing executables to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f dunst-checkmail ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dunst-checkmail
	@echo installing manual pages to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@cp -f dunst-checkmail.1 ${DESTDIR}${MANPREFIX}/man1/
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/dunst-checkmail.1

uninstall:
	@echo removing executables from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/dunst-checkmail
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/dunst-checkmail.1

.PHONY: all options clean dist install uninstall
