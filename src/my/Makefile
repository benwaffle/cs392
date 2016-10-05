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

../../lib/libmy.a: $(OBJ)
	$(AR) rcs $@ $^

all: ../../lib/libmy.a

clean:
	$(RM) $(OBJ)

fclean:
	$(RM) ../../lib/libmy.a

re: fclean all

.PHONY: all clean fclean re
