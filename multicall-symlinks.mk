all: contacts

install: contacts
	mkdir -p -- "$(DESTDIR)$(PREFIX)/lib"
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	cp -- contacts "$(DESTDIR)$(PREFIX)/lib/"
	cd -- "$(DESTDIR)$(PREFIX)/bin/" &&\
		for f in $(BIN); do\
			ln -s -- ../lib/contacts "$$f" || exit 1;\
		done
