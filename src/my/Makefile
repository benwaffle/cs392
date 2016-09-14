SRC = my_char.c my_str.c my_int.c \
      my_num_base.c \
      my_alpha.c my_digits.c \
      my_revstr.c my_strindex.c my_strrindex.c my_strlen.c
OBJ = $(SRC:.c=.o)
CFLAGS += -I../../include -g -std=c99 -Wall -Werror -pedantic

all: $(OBJ)
	ar -rc ../../lib/libmy.a $^
	ranlib ../../lib/libmy.a

clean:
	rm -f $(OBJ)
	rm -f ../../lib/libmy.a
