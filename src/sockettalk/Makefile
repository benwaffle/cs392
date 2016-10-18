CFLAGS = -std=c99 -g -Wall -Werror -pedantic -I../../include -D_POSIX_C_SOURCE=201112L
LDLIBS = -L../../lib -lmy -llist

sockettalk: server client

all: sockettalk

clean:
	$(RM) ./server ./client

.PHONY: all clean
