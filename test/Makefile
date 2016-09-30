BIN = testmy lab_one lab_two
CFLAGS += -g -Wall -Werror -pedantic -std=c99 -I ../include
LDLIBS += -L../lib -lmy

.PHONY: all check fclean

all: $(BIN)

check: testmy
	./testmy

fclean:
	rm -f $(BIN)
