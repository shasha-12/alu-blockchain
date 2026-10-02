#include "cli.h"

/**
 * print_block_header - prints the information of a block
 * @block: block to print
 */
static void print_block_header(block_t const *block)
{
	char date[32] = "unknown";
	time_t timestamp = (time_t)block->info.timestamp;
	struct tm *tm = gmtime(&timestamp);

	if (tm)
		strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S UTC", tm);
	printf("Block #%u\n", block->info.index);
	printf("  time      : %s\n", date);
	printf("  difficulty: %u\n", block->info.difficulty);
	printf("  nonce     : %lu\n", (unsigned long)block->info.nonce);
	printf("  data      : \"%.*s\"\n", (int)block->data.len,
	       (char const *)block->data.buffer);
	printf("  hash      : ");
	print_hex(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n  prev hash : ");
	print_hex(block->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf("\n  transactions: %d\n", llist_size(block->transactions) < 0 ?
	       0 : llist_size(block->transactions));
}

/**
 * cmd_chain - prints a one-line summary of every block of the Blockchain
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0
 */
int cmd_chain(state_t *state, char **argv)
{
	block_t *block;
	int i, nb_tx;

	printf("  #     difficulty  txs  hash\n");
	for (i = 0; i < llist_size(state->blockchain->chain); i++)
	{
		block = llist_get_node_at(state->blockchain->chain, i);
		nb_tx = llist_size(block->transactions);
		printf("  %-5u %-11u %-4d ", block->info.index,
		       block->info.difficulty, nb_tx < 0 ? 0 : nb_tx);
		print_hex(block->hash, SHA256_DIGEST_LENGTH);
		printf("\n");
	}
	return (0);
	(void)argv;
}

/**
 * cmd_block - prints a block and its transactions
 * @state: CLI state
 * @argv: argv[0] is the index of the block
 * Return: 0 on success, -1 on failure
 */
int cmd_block(state_t *state, char **argv)
{
	uint8_t me[EC_PUB_LEN];
	block_t *block = NULL;
	char *end;
	unsigned long index = strtoul(argv[0], &end, 10);
	int i;

	if (*argv[0] >= '0' && *argv[0] <= '9' && !*end &&
	    index < (unsigned long)llist_size(state->blockchain->chain))
		block = llist_get_node_at(state->blockchain->chain, index);
	if (!block || !ec_to_pub(state->wallet, me))
		return (fprintf(stderr, "block: no block #%s\n", argv[0]), -1);
	print_block_header(block);
	for (i = 0; i < llist_size(block->transactions); i++)
		print_tx(state->blockchain,
			 llist_get_node_at(block->transactions, i), me);
	return (0);
}

/**
 * find_tx - finds a transaction from the beginning of its id
 * @list: list of transactions to search
 * @prefix: lowercase hexadecimal prefix of the id
 * Return: the first matching transaction, or NULL
 */
static transaction_t *find_tx(llist_t *list, char const *prefix)
{
	char id[SHA256_DIGEST_LENGTH * 2 + 1];
	transaction_t *tx;
	int i, j;

	for (i = 0; i < llist_size(list); i++)
	{
		tx = llist_get_node_at(list, i);
		for (j = 0; j < SHA256_DIGEST_LENGTH; j++)
			sprintf(id + j * 2, "%02x", tx->id[j]);
		if (!strncmp(id, prefix, strlen(prefix)))
			return (tx);
	}
	return (NULL);
}

/**
 * cmd_tx - finds a transaction by id (or id prefix) and prints it
 * @state: CLI state
 * @argv: argv[0] is the id, or at least its first 8 hexadecimal digits
 * Return: 0 on success, -1 on failure
 */
int cmd_tx(state_t *state, char **argv)
{
	uint8_t me[EC_PUB_LEN];
	transaction_t *tx = NULL;
	block_t *block;
	char *s;
	int i;

	for (s = argv[0]; *s; s++)
		*s = (*s >= 'A' && *s <= 'F') ? *s - 'A' + 'a' : *s;
	if (strlen(argv[0]) < 8 || !ec_to_pub(state->wallet, me))
		return (fprintf(stderr, "tx: give at least 8 digits\n"), -1);
	for (i = 0; !tx && i < llist_size(state->blockchain->chain); i++)
	{
		block = llist_get_node_at(state->blockchain->chain, i);
		tx = find_tx(block->transactions, argv[0]);
	}
	if (tx)
		printf("In block #%u:\n", block->info.index);
	else
	{
		tx = find_tx(state->tx_pool, argv[0]);
		if (!tx)
			return (fprintf(stderr, "tx: %s not found\n", argv[0]), -1);
		printf("Pending in the transaction pool:\n");
	}
	print_tx(state->blockchain, tx, me);
	return (0);
}
