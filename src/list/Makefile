SRC := \
	add.c \
	append.c \
	debug.c \
	new_node.c \
	print.c \
	remove.c \
	traverse.c \
	util.c

OBJ := $(SRC:.c=.o)
CFLAGS += -I../../include -g -std=c99 -Wall -Werror -pedantic

../../lib/liblist.a: $(OBJ)
	$(AR) rcs $@ $^

all: ../../lib/liblist.a

clean:
	$(RM) $(OBJ)

fclean:
	$(RM) ../../lib/liblist.a

re: fclean all

.PHONY: all clean fclean re
