#!/usr/bin/make -f

CC=             gcc
CFLAGS_OPT?=    -O2 -g -pipe
CFLAGS_STD?=    -std=c11
CFLAGS_WARN?=   -Wall -W -Wextra -Wpedantic

CFLAGS?=        ${CFLAGS_OPT}
CFLAGS+=        ${CFLAGS_STD} ${CFLAGS_WARN}

CFLAGS+=        -Werror
CFLAGS+=        -Wbad-function-cast -Wcast-align -Wcast-qual -Wchar-subscripts \
                -Winline -Wnested-externs -Wpointer-arith \
                -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

CPPFLAGS_STD?=  -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700
CPPFLAGS+=      ${CPPFLAGS_STD}

RM?=            rm -f

# Programs and their source files
PROGS=          foo bar
SRCS_foo=       foo.c
SRCS_bar=       bar.c

# Object files
OBJS_foo=       $(SRCS_foo:.c=.o)
OBJS_bar=       $(SRCS_bar:.c=.o)

all:            ${PROGS}

foo:            ${OBJS_foo}
		${CC} ${LDFLAGS} -o $@ ${OBJS_foo}

bar:            ${OBJS_bar}
		${CC} ${LDFLAGS} -o $@ ${OBJS_bar}

%.o: %.c
		${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<

clean:
		${RM} -- ${OBJS_foo} ${OBJS_bar} ${PROGS}

.PHONY: all clean
