#include "cli.h"

/**
 * fill_block - moves the valid transactions of the pool into a block
 * @state: CLI state
 * @block: block to fill, already containing the coinbase transaction
 * Return: number of transactions taken from the pool
 */
static int fill_block(state_t *state, block_t *block)
{
	transaction_t *tx;
	int added = 0;

	while (llist_size(state->tx_pool) > 0)
	{
		tx = llist_pop(state->tx_pool);
		if (!transaction_is_valid(tx, state->blockchain->unspent) ||
		    inputs_conflict(block->transactions, tx) ||
		    llist_add_node(block->transactions, tx, ADD_NODE_REAR))
		{
			fprintf(stderr, "mine: invalid transaction dropped\n");
			transaction_destroy(tx);
			continue;
		}
		added++;
	}
	return (added);
}

/**
 * cmd_mine - mines a block with the pending transactions and adds it to
 * the Blockchain
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0 on success, -1 on failure
 */
int cmd_mine(state_t *state, char **argv)
{
	blockchain_t *blockchain = state->blockchain;
	block_t *prev = llist_get_tail(blockchain->chain), *block;
	transaction_t *coinbase;
	int added;

	block = block_create(prev, (int8_t *)CLI_BLOCK_DATA,
			     strlen(CLI_BLOCK_DATA));
	if (!block)
		return (fprintf(stderr, "mine: failed to create block\n"), -1);
	block->info.difficulty = blockchain_difficulty(blockchain);
	coinbase = coinbase_create(state->wallet, block->info.index);
	if (!coinbase ||
	    llist_add_node(block->transactions, coinbase, ADD_NODE_REAR))
	{
		transaction_destroy(coinbase);
		block_destroy(block);
		return (fprintf(stderr, "mine: failed to create coinbase\n"), -1);
	}
	added = fill_block(state, block);
	block_mine(block);
	if (block_is_valid(block, prev, blockchain->unspent) ||
	    llist_add_node(blockchain->chain, block, ADD_NODE_REAR))
	{
		block_destroy(block);
		return (fprintf(stderr, "mine: invalid block\n"), -1);
	}
	blockchain->unspent = update_unspent(block->transactions, block->hash,
					     blockchain->unspent);
	printf("Block #%u mined (difficulty %u, coinbase + %d transaction%s): ",
	       block->info.index, block->info.difficulty, added,
	       added == 1 ? "" : "s");
	print_hex(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
	(void)argv;
}
