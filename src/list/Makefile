SRC := \
	add_elem.c \
	add_node.c \
	add_node_at.c \
	append.c \
	count_s_nodes.c \
	debug_char.c \
	debug_int.c \
	debug_string.c \
	elem_at.c \
	empty_list.c \
	new_node.c \
	node_at.c \
	print_char.c \
	print_int.c \
	print_string.c \
	remove_last.c \
	remove_node.c \
	remove_node_at.c \
	traverse_char.c \
	traverse_int.c \
	traverse_string.c

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
