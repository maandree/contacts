all: contacts

install: contacts
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	set -- $(BIN) &&\
		cp -- "$$1" "$(DESTDIR)$(PREFIX)/bin/$$1" &&\
		linkto="$$1" &&\
		shift 1 &&\
		cd -- "$(DESTDIR)$(PREFIX)/bin/" &&\
			for f; do\
				ln -- "$$linkto" "$$f" || exit 1;\
			done
