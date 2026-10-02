CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -I. -I../../crypto

SRC = blockchain_create.c blockchain_deserialize.c blockchain_destroy.c \
      blockchain_difficulty.c blockchain_serialize.c block_create.c \
      block_destroy.c block_hash.c block_is_valid.c block_mine.c \
      hash_matches_difficulty.c
OBJ = $(SRC:.c=.o)
NAME = libhblk_blockchain_v0.2.a

.PHONY: all clean oclean fclean re

all: $(NAME)

$(NAME): blockchain.h $(OBJ)
	ar rcs $(NAME) $(OBJ)

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean

re: fclean all
