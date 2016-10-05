SRC := \
	  my_alpha.c \
	  my_atoi.c \
	  my_char.c \
	  my_digits.c \
	  my_int.c \
	  my_num_base.c \
	  my_revstr.c \
	  my_str.c \
	  my_strcat.c \
	  my_strcmp.c \
	  my_strconcat.c \
	  my_strcpy.c \
	  my_strncpy.c \
	  my_strdup.c \
	  my_strfind.c \
	  my_strindex.c \
	  my_strlen.c \
	  my_strncmp.c \
	  my_strnconcat.c \
	  my_strrfind.c \
	  my_strrindex.c

OBJ := $(SRC:.c=.o)
CFLAGS += -I../../include -g -std=c99 -Wall -Werror -pedantic

.PHONY: all clean fclean

all: $(OBJ)
	ar rcs ../../lib/libmy.a $^

clean:
	rm -f $(OBJ)

fclean:
	rm -f ../../lib/libmy.a

re: fclean all
