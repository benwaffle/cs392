CFLAGS += -I ./include -g -std=c99 -Wall -Werror -pedantic -D_POSIX_C_SOURCE=201112L

# libmy

MY_SRC := \
	  src/my/my_alpha.c \
	  src/my/my_atoi.c \
	  src/my/my_char.c \
	  src/my/my_digits.c \
	  src/my/my_int.c \
	  src/my/my_num_base.c \
	  src/my/my_revstr.c \
	  src/my/my_str.c \
	  src/my/my_str2vect.c \
	  src/my/my_strcat.c \
	  src/my/my_strcmp.c \
	  src/my/my_strconcat.c \
	  src/my/my_strcpy.c \
	  src/my/my_strncpy.c \
	  src/my/my_strdup.c \
	  src/my/my_strfind.c \
	  src/my/my_strindex.c \
	  src/my/my_strlen.c \
	  src/my/my_strncmp.c \
	  src/my/my_strnconcat.c \
	  src/my/my_strrfind.c \
	  src/my/my_strrindex.c \
	  src/my/my_vect2str.c

MY_OBJ := $(MY_SRC:.c=.o)

MY_LIB := lib/libmy.a

# liblist

LIST_SRC := \
	src/list/add_elem.c \
	src/list/add_node.c \
	src/list/add_node_at.c \
	src/list/append.c \
	src/list/count_s_nodes.c \
	src/list/debug_char.c \
	src/list/debug_int.c \
	src/list/debug_string.c \
	src/list/elem_at.c \
	src/list/empty_list.c \
	src/list/new_node.c \
	src/list/node_at.c \
	src/list/print_char.c \
	src/list/print_int.c \
	src/list/print_string.c \
	src/list/remove_last.c \
	src/list/remove_node.c \
	src/list/remove_node_at.c \
	src/list/traverse_char.c \
	src/list/traverse_int.c \
	src/list/traverse_string.c

LIST_OBJ := $(LIST_SRC:.c=.o)

LIST_LIB := lib/liblist.a



PIPES_BIN := src/pipes/pipes

SIGNALS_BIN := src/signals/signals

SOCKET_BIN := src/socket/socket

SOCKETTALK_BIN := \
	 src/sockettalk/server \
	 src/sockettalk/client

MINISHELL_BIN := src/minishell/minishell

MALLOC_BIN := src/malloc/malloc

MYSELECT_BIN := src/myselect/myselect

PTHREAD_BIN := src/pthread/lab6

NSMINISHELL_BIN := src/nsminishell/nsminishell

GTK_BIN := src/gtk/gtk

# tests

TESTS := \
	test/testmy \
	test/testlist \
	test/lab_one \
	test/lab_two

# rules

all: \
	$(MY_LIB) \
	$(LIST_LIB) \
	$(TESTS) \
	$(PIPES_BIN) \
	$(SIGNALS_BIN) \
	$(SOCKET_BIN) \
	$(SOCKETTALK_BIN) \
	$(MINISHELL_BIN) \
	$(MALLOC_BIN) \
	$(MYSELECT_BIN) \
	$(PTHREAD_BIN) \
	$(NSMINISHELL_BIN) \
	$(GTK_BIN)

clean:
	$(RM) $(MY_OBJ)
	$(RM) $(LIST_OBJ)

fclean: clean
	$(RM) $(MY_LIB)
	$(RM) $(LIST_LIB)
	$(RM) $(TESTS)
	$(RM) $(PIPES_BIN)
	$(RM) $(SIGNALS_BIN)
	$(RM) $(SOCKET_BIN)
	$(RM) $(SOCKETTALK_BIN)
	$(RM) $(MINISHELL_BIN)
	$(RM) $(MALLOC_BIN)
	$(RM) $(MYSELECT_BIN)
	$(RM) $(PTHREAD_BIN)
	$(RM) $(NSMINISHELL_BIN)
	$(RM) $(GTK_BIN)

re: fclean all

check: $(TESTS)
	test/testmy
	test/testlist

.PHONY: all clean fclean re check

test/testmy: $(MY_LIB)
test/testlist: $(MY_LIB) $(LIST_LIB)
test/lab_one: $(MY_LIB)
test/lab_two:

tests: $(TESTS)

$(MY_LIB): $(MY_OBJ)
$(LIST_LIB): $(LIST_OBJ)

$(PIPES_BIN): $(MY_LIB)
$(SOCKETTALK_BIN): $(MY_LIB) $(LIST_LIB)
$(MINISHELL_BIN): $(MY_LIB)
$(MYSELECT_BIN): LDLIBS += $(shell pkg-config --libs ncurses)
$(PTHREAD_BIN): LDFLAGS += -pthread

$(NSMINISHELL_BIN): \
	src/nsminishell/nsminishell.c \
	src/nsminishell/history.c \
	src/nsminishell/term.c \
	$(LIST_LIB) $(MY_LIB)
$(NSMINISHELL_BIN): LDLIBS += $(shell pkg-config --libs ncurses)

$(GTK_BIN): CFLAGS += $(shell pkg-config --cflags gtk+-3.0)
$(GTK_BIN): LDLIBS += $(shell pkg-config --libs gtk+-3.0)

# build archives
%.a:
	$(AR) rcs $@ $^
