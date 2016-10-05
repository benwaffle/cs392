BIN := testmy lab_one lab_two
CFLAGS += -g -Wall -Werror -pedantic -std=c99 -I ../include
LDLIBS += -L../lib -lmy

all: $(BIN)

check: testmy
	./testmy

fclean:
	rm -f $(BIN)

.PHONY: all check fclean
