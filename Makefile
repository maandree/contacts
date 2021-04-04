.POSIX:

CONFIGFILE = config.mk
include $(CONFIGFILE)


BIN =\
	add-contact\
	find-contact-by-chat\
	find-contact-by-email\
	find-contact-by-name\
	find-contact-by-number\
	find-contact-by-organisation\
	find-contact-by-pgpkey\
	find-contact-by-photo\
	find-contact-by-site\
	get-contact-birthday\
	get-contact-chats\
	get-contact-emails\
	get-contact-file\
	get-contact-gender\
	get-contact-groups\
	get-contact-name\
	get-contact-notes\
	get-contact-numbers\
	get-contact-organisations\
	get-contact-pgpkeys\
	get-contact-photos\
	get-contact-sites\
	is-contact-ice\
	list-chat-contacts\
	list-contact-groups\
	list-contact-organisations\
	list-contacts\
	list-group-contacts\
	list-organisation-contacts\
	print-contact\
	remove-contact\
	set-contact-chats\
	set-contact-emails\
	set-contact-gender\
	set-contact-groups\
	set-contact-ice\
	set-contact-name\
	set-contact-notes\
	set-contact-numbers\
	set-contact-organisations\
	set-contact-pgpkeys\
	set-contact-photos\
	set-contact-sites

HDR =\
	common.h

OBJ = $(BIN:=.o)
BOBJ = $(BIN:=.bo)


all: $(BIN) contacts
$(OBJ): $(@:.o=.c) $(HDR)
$(BOBJ): $(@:.bo=.c) $(HDR)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.bo:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS) -Dmain="$$(printf '%s\n' $*_main | tr - _)" -DMULTICALL_BINARY

contacts: contacts.o $(BOBJ)
	$(CC) -o $@ $@.o $(BOBJ) $(LDFLAGS)

contacts.c: contacts.c.in Makefile
	printf '#define LIST_COMMANDS' > $@
	printf '\\\n\tX(%s)' $(BIN) | tr - _ >> $@
	printf '\n\n' >> $@
	cat contacts.c.in >> $@

add-contact: add-contact.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-chat: find-contact-by-chat.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-email: find-contact-by-email.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-name: find-contact-by-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-number: find-contact-by-number.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-organisation: find-contact-by-organisation.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-pgpkey: find-contact-by-pgpkey.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-photo: find-contact-by-photo.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-site: find-contact-by-site.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-birthday: get-contact-birthday.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-chats: get-contact-chats.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-emails: get-contact-emails.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-file: get-contact-file.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-gender: get-contact-gender.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-groups: get-contact-groups.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-name: get-contact-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-notes: get-contact-notes.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-numbers: get-contact-numbers.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-organisations: get-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-pgpkeys: get-contact-pgpkeys.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-photos: get-contact-photos.o
	$(CC) -o $@ $@.o $(LDFLAGS)

get-contact-sites: get-contact-sites.o
	$(CC) -o $@ $@.o $(LDFLAGS)

is-contact-ice: is-contact-ice.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-birthdays: list-birthdays.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-chat-contacts: list-chat-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-contact-groups: list-contact-groups.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-contact-organisations: list-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-contacts: list-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-group-contacts: list-group-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

list-organisation-contacts: list-organisation-contacts.o
	$(CC) -o $@ $@.o $(LDFLAGS)

print-contact: print-contact.o
	$(CC) -o $@ $@.o $(LDFLAGS)

remove-contact: remove-contact.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-birthday: set-contact-birthday.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-chats: set-contact-chats.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-emails: set-contact-emails.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-gender: set-contact-gender.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-groups: set-contact-groups.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-ice: set-contact-ice.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-name: set-contact-name.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-notes: set-contact-notes.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-numbers: set-contact-numbers.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-organisations: set-contact-organisations.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-pgpkeys: set-contact-pgpkeys.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-photos: set-contact-photos.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-sites: set-contact-sites.o
	$(CC) -o $@ $@.o $(LDFLAGS)

install: $(BIN)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	cp -- $(BIN) "$(DESTDIR)$(PREFIX)/bin/"

install-mcb: contacts
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	set -- $(BIN) &&\
		cp -- "$$1" "$(DESTDIR)$(PREFIX)/bin/$$1" &&\
		linkto="$$1" &&\
		shift 1 &&\
		cd -- "$(DESTDIR)$(PREFIX)/bin/" &&\
			for f; do\
				ln -- "$$linkto" "$$f" || exit 1;\
			done

install-mcb-symlinks: contacts
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	cp -- contacts "$(DESTDIR)$(PREFIX)/lib/"
	cd -- "$(DESTDIR)$(PREFIX)/bin/" &&\
		for f in $(BIN); do\
			ln -s -- ../lib/contacts "$$f" || exit 1;\
		done

uninstall:
	-cd -- "$(DESTDIR)$(PREFIX)/bin" && rm -f -- $(BIN)
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/contacts"

clean:
	-rm -f -- *.o *.a *.lo *.so *.bo *.su $(BIN) contacts contacts.c

.SUFFIXES:
.SUFFIXES: .c .o .bo

.PHONY: all install install-mcb install-mcb-symlinks uninstall clean
