NAME = woody_woodpacker
STUB = stub

CC = cc

CFLAGS = -Wall -Wextra -Werror

SRC = $(addprefix ./src/, $(SOURCES))
SOURCES = main.c chacha20.c

STUB_SRC = $(addprefix ./src/, $(STUB_SOURCES))
STUB_SOURCES = stub.c chacha20.c

OBJ = $(SRC:.c=.o)
STUB_OBJ = $(STUB_SRC:.c=.o)

all: $(NAME) $(STUB)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $(OBJ)
	@echo "woody_woodpacker built"

$(STUB): $(STUB_OBJ)
	@$(CC) $(CFLAGS) -o $@ $(STUB_OBJ)
	@echo "stub built"

woody: $(NAME)

stub: $(STUB)

clean:
	@rm -f $(OBJ) $(STUB_OBJ)

fclean: clean
	@rm -f $(NAME) $(STUB) woody stub

re: fclean all

.PHONY: all woody stub clean fclean re