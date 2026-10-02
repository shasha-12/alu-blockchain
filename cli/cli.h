#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "blockchain.h"

#define CLI_PROMPT "blockchain> "
#define CLI_MAX_ARGS 4
#define CLI_MAX_PATH 200
#define CLI_BLOCK_DATA "Holberton School"
#define CLI_SHORT_LEN 8

/**
 * struct state_s - Global state of the CLI
 *
 * @wallet:     EC key pair of the current user
 * @blockchain: Local Blockchain
 * @tx_pool:    Local list of pending transactions (transaction_t *)
 */
typedef struct state_s
{
	EC_KEY *wallet;
	blockchain_t *blockchain;
	llist_t *tx_pool;
} state_t;

/**
 * struct command_s - CLI command
 *
 * @name:  Name typed by the user
 * @argc:  Number of expected arguments (command name excluded)
 * @usage: Usage string
 * @desc:  Short description
 * @run:   Function executing the command
 */
typedef struct command_s
{
	char const *name;
	int argc;
	char const *usage;
	char const *desc;
	int (*run)(state_t *state, char **argv);
} command_t;

/* cli.c */
int cmd_help(state_t *state, char **argv);

/* wallet.c */
int cmd_wallet_load(state_t *state, char **argv);
int cmd_wallet_save(state_t *state, char **argv);

/* send.c */
int cmd_send(state_t *state, char **argv);

/* mine.c */
int cmd_mine(state_t *state, char **argv);

/* info.c */
int cmd_info(state_t *state, char **argv);
void print_hex(uint8_t const *buf, size_t len);

/* storage.c */
int cmd_load(state_t *state, char **argv);
int cmd_save(state_t *state, char **argv);

/* utils.c */
int parse_amount(char const *s, uint32_t *amount);
int parse_pub(char const *s, uint8_t pub[EC_PUB_LEN]);
int same_output(tx_in_t const *in, unspent_tx_out_t const *utxo);
int utxo_in_pool(llist_t *tx_pool, unspent_tx_out_t const *utxo);
int inputs_conflict(llist_t *transactions, transaction_t const *tx);

/* display.c */
void print_short(uint8_t const *buf, size_t len);
int tx_is_coinbase(transaction_t const *tx);
tx_out_t *find_output(blockchain_t *blockchain, tx_in_t const *in);
void print_output(tx_out_t const *out, uint8_t const *me);
void print_tx(blockchain_t *blockchain, transaction_t const *tx,
	      uint8_t const *me);

/* explorer.c */
int cmd_chain(state_t *state, char **argv);
int cmd_block(state_t *state, char **argv);
int cmd_tx(state_t *state, char **argv);

/* account.c */
int cmd_address(state_t *state, char **argv);
int cmd_balance(state_t *state, char **argv);
int cmd_history(state_t *state, char **argv);

#endif /* CLI_H */
