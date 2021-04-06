all: $(BIN)

install: $(BIN)
	mkdir -p -- "$(DESTDIR)$(PREFIX)/bin"
	cp -- $(BIN) "$(DESTDIR)$(PREFIX)/bin/"
