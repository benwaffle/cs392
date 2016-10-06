BIN := testlist testmy lab_one lab_two
CFLAGS += -g -Wall -Werror -pedantic -std=c99 -I ../include
LDLIBS += -L../lib -llist -lmy

all: $(BIN)

check: testlist
	./testlist

fclean:
	rm -f $(BIN)

.PHONY: all check fclean
