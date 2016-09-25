BIN = test_my lab_one lab_two
CFLAGS += -g -Wall -Werror -pedantic -std=c99 -I ../include
LDLIBS += -L../lib -lmy

.PHONY: all check fclean

all: $(BIN)

check: test_my
	./test_my

fclean:
	rm -f $(BIN)
