CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -I. -I../blockchain/v0.3 \
	 -I../blockchain/v0.3/transaction -I../blockchain/v0.3/provided \
	 -I../crypto
LDFLAGS = -L../blockchain/v0.3 -L../crypto
LDLIBS = -lhblk_blockchain -lhblk_crypto -lllist -lssl -lcrypto -pthread

# _endianness.c is provided with the Blockchain sources, not in the library
vpath _endianness.c ../blockchain/v0.3/provided

SRC = cli.c wallet.c send.c mine.c info.c storage.c utils.c _endianness.c
OBJ = $(SRC:.c=.o)
NAME = cli

.PHONY: all libs clean oclean fclean re

all: $(NAME)

$(NAME): libs $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $(NAME)

$(OBJ): cli.h ../blockchain/v0.3/blockchain.h

libs:
	$(MAKE) -C ../crypto libhblk_crypto.a
	$(MAKE) -C ../blockchain/v0.3 libhblk_blockchain.a

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean

re: fclean all
