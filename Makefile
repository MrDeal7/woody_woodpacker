NAME = woody

CC = cc

CFLAGS = -Wall -Wextra -Werror

SRC = $(addprefix ./src/, $(SOURCES))
SOURCES = main.c chacha20.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ)
	@echo "woody_woodpacker built"

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re