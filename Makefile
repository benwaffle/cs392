run: fclean all
	$(MAKE) -C test/ check

all:
	$(MAKE) -C src/my/ all
	$(MAKE) -C test/ all

clean:
	$(MAKE) -C src/my/ clean

fclean:
	$(MAKE) -C src/my/ fclean
	$(MAKE) -C test/ fclean

re: clean fclean all

.PHONY: run all clean fclean re
