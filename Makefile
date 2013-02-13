CC=gcc
CFLAGS=-O2 -Wall
OBJ=nss_devenv.o

%.o: %.c
	$(CC) -fPIC -c -o $@ $< $(CFLAGS)

libnss_devenv.so.2: $(OBJ)
	$(CC) -shared -o $@ $^ -Wl,-soname,libnss_devenv.so.2 $(CFLAGS)

clean:
	rm -f *.o *~ libnss_devenv.so.2

install:
	ln -sf $(CURDIR)/libnss_devenv.so.2 /lib

uninstall:
	rm /lib/libnss_devenv.so.2
