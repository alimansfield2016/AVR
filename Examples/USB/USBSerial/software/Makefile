PRJNAME = usbserial

LIBUSB_CONFIG   = libusb-config
CFLAGS+=-g -Wall -pedantic `$(LIBUSB_CONFIG) --cflags`
LFLAGS+=`$(LIBUSB_CONFIG) --libs` -lusb

SRCDIR = src
SRCEXT = cpp
OBJDIR = obj
OBJEXT = o

SRC += ${shell find ${SRCDIR}/ -type f -name '*.${SRCEXT}'}
OBJ += ${SRC:${SRCDIR}/%.${SRCEXT}=${OBJDIR}/%.${OBJEXT}}

${OBJDIR}/%.${OBJEXT} : ${SRCDIR}/%.${SRCEXT}
	g++ -c ${CFLAGS} -o $@ $<

${PRJNAME} : ${OBJ}
	g++ -o ${LFLAGS} $@ $^