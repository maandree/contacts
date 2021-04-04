PREFIX    = /usr
MANPREFIX = $(PREFIX)/share/man

CC = cc

CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -I../libcontacts
CFLAGS   = -std=c99 -Wall -O2
LDFLAGS  = -L../libcontacts -lcontacts -lsimple
