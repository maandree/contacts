.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)


BIN =\
	find-contact-by-email\
	find-contact-by-name\
	find-contact-by-pgpkey\
	find-contact-by-site\
	get-contact-emails\
	get-contact-file\
	get-contact-gender\
	get-contact-name\
	get-contact-notes\
	get-contact-organisations\
	get-contact-pgpkeys\
	get-contact-sites\
	is-contact-ice\
	list-contacts\
	set-contact-emails\
	set-contact-gender\
	set-contact-ice\
	set-contact-name\
	set-contact-notes\
	set-contact-organisations\
	set-contact-pgpkeys\
	set-contact-sites

HDR =\
	common.h

OBJ = $(BIN:=.o)


all: $(BIN)
$(OBJ): $(@:.o=.c) $(HDR)

libcontacts.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(AR) -s $@

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

find-contact-by-email: find-contact-by-email.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-name: find-contact-by-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-pgpkey: find-contact-by-pgpkey.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-site: find-contact-by-site.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-emails: get-contact-emails.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-file: get-contact-file.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-gender: get-contact-gender.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-name: get-contact-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-notes: get-contact-notes.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-organisations: get-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-pgpkeys: get-contact-pgpkeys.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-sites: get-contact-sites.o
	$(CC) -o $@ $@.o $(LDFLAGS)

is-contact-ice: is-contact-ice.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-contact-organisations: list-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-contacts: list-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-organisation-contacts: list-organisation-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-emails: set-contact-emails.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-gender: set-contact-gender.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-ice: set-contact-ice.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-name: set-contact-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-notes: set-contact-notes.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-organisations: set-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-pgpkeys: set-contact-pgpkeys.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-sites: set-contact-sites.o
	$(CC) -o $@ $@.o $(LDFLAGS)

install: $(BIN)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	cp -- $(BIN) "$(DESTDIR)$(PREFIX)/bin/"

uninstall:
	-cd -- "$(DESTDIR)$(PREFIX)/bin" && rm -f -- $(BIN)

clean:
	-rm -f -- *.o *.a *.lo *.so *.su $(BIN)

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY: all install uninstall clean
