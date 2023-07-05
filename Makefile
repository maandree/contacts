.POSIX:

CONFIGFILE = config.mk

CALLTYPE = multicall-hardlinks
# multicall-hardlinks = multiple hardlinks of the same multicall binary is installed
# multicall-symlinks  = multiple links to a multicall binary named $(PREFIX)/bin/contacts are installed
# singlecall          = separate binaries are install for each command (greatly wastes space when statically linked)


BIN =\
	add-contact\
	find-contact-by-address\
	find-contact-by-chat\
	find-contact-by-email\
	find-contact-by-name\
	find-contact-by-number\
	find-contact-by-organisation\
	find-contact-by-pgpkey\
	find-contact-by-photo\
	find-contact-by-site\
	get-contact-addresses\
	get-contact-birthday\
	get-contact-blocks\
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
	list-birthdays\
	list-chat-contacts\
	list-contact-groups\
	list-contact-organisations\
	list-contacts\
	list-group-contacts\
	list-organisation-contacts\
	print-contact\
	remove-contact\
	set-contact-addresses\
	set-contact-birthday\
	set-contact-blocks\
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
	set-contact-sites\
	unblock-contact

HDR =\
	common.h

OBJ = $(BIN:=.o) common-birthday.o common-address.o
BOBJ = $(OBJ:.o=.bo)


include $(CONFIGFILE)
include $(CALLTYPE).mk


$(OBJ): $(@:.o=.c) $(HDR)
$(BOBJ): $(@:.bo=.c) $(HDR)

.c.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS)

.c.bo:
	$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS) -Dmain="$$(printf '%s\n' main__$* | tr - _)" -DMULTICALL_BINARY

contacts: contacts.o $(BOBJ)
	$(CC) -o $@ $@.o $(BOBJ) $(LDFLAGS)

contacts.c: contacts.c.in Makefile
	printf '#define LIST_COMMANDS' > $@
	for bin in $(BIN); do\
		printf '\\\n\tX(%s, %s)' $$bin $$(printf '%s\n' $$bin | tr - _) || exit 1;\
	done >> $@
	printf '\n\n' >> $@
	cat contacts.c.in >> $@
# (printf '\\\n\tX(%s)' $(BIN); printf '\n\n') are run together
# because the input of sed must be a text file, and the first
# printf(1) do not generate a text file as text files are by
# definition LF terminated unless they are empty (also no line
# may exceed 2048 bytes including the LF)

add-contact: add-contact.o
	$(CC) -o $@ $@.o $(LDFLAGS)

find-contact-by-address: find-contact-by-address.o common-address.o
	$(CC) -o $@ $@.o common-address.o $(LDFLAGS)

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

get-contact-addresses: get-contact-addresses.o common-address.o
	$(CC) -o $@ $@.o common-address.o $(LDFLAGS)

get-contact-birthday: get-contact-birthday.o common-birthday.o
	$(CC) -o $@ $@.o common-birthday.o $(LDFLAGS)

get-contact-blocks: get-contact-blocks.o
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

list-birthdays: list-birthdays.o common-birthday.o
	$(CC) -o $@ $@.o common-birthday.o $(LDFLAGS)

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

set-contact-addresses: set-contact-addresses.o common-address.o
	$(CC) -o $@ $@.o common-address.o $(LDFLAGS)

set-contact-birthday: set-contact-birthday.o
	$(CC) -o $@ $@.o $(LDFLAGS)

set-contact-blocks: set-contact-blocks.o
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

unblock-contact: unblock-contact.o
	$(CC) -o $@ $@.o $(LDFLAGS)

uninstall:
	-cd -- "$(DESTDIR)$(PREFIX)/bin" && rm -f -- $(BIN)
	-rm -f -- "$(DESTDIR)$(PREFIX)/lib/contacts"

clean:
	-rm -f -- *.o *.a *.lo *.so *.bo *.su $(BIN) contacts contacts.c

.SUFFIXES:
.SUFFIXES: .c .o .bo

.PHONY: all install uninstall clean
