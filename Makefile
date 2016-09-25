run: fclean all
	make -C test/ check

all:
	make -C src/my/ all
	make -C test/ all

clean:
	make -C src/my/ clean

fclean:
	make -C src/my/ fclean
	make -C test/ fclean

re: clean fclean all
