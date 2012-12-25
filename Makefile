# syntog - toggle synaptics touchpad
# See LICENSE file for copyright and license details.

include config.mk

SRC = util.c syntog.c
OBJ = ${SRC:.c=.o}

all: options syntog

options:
	@echo syntog build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

syntog: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f syntog ${OBJ} syntog-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p syntog-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk \
		syntog.1 ${SRC} syntog-${VERSION}
	@tar -cf syntog-${VERSION}.tar syntog-${VERSION}
	@gzip syntog-${VERSION}.tar
	@rm -rf syntog-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f syntog ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/syntog

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/syntog

.PHONY: all options clean dist install uninstall
