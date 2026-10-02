#include "cli.h"

/**
 * cmd_address - prints the address (public key) of the wallet
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0 on success, -1 on failure
 */
int cmd_address(state_t *state, char **argv)
{
	uint8_t me[EC_PUB_LEN];

	if (!ec_to_pub(state->wallet, me))
		return (fprintf(stderr, "address: invalid wallet\n"), -1);
	print_hex(me, EC_PUB_LEN);
	printf("\n");
	return (0);
	(void)argv;
}

/**
 * cmd_balance - prints the confirmed, pending and available balance
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0 on success, -1 on failure
 */
int cmd_balance(state_t *state, char **argv)
{
	llist_t *all_unspent = state->blockchain->unspent;
	unspent_tx_out_t *utxo;
	uint8_t me[EC_PUB_LEN];
	unsigned long confirmed = 0, locked = 0;
	int i, count = 0;

	if (!ec_to_pub(state->wallet, me))
		return (fprintf(stderr, "balance: invalid wallet\n"), -1);
	for (i = 0; i < llist_size(all_unspent); i++)
	{
		utxo = llist_get_node_at(all_unspent, i);
		if (memcmp(utxo->out.pub, me, EC_PUB_LEN))
			continue;
		confirmed += utxo->out.amount;
		count++;
		if (utxo_in_pool(state->tx_pool, utxo))
			locked += utxo->out.amount;
	}
	printf("Confirmed : %lu coins (%d unspent output%s)\n", confirmed,
	       count, count == 1 ? "" : "s");
	printf("Pending   : %lu coins used by transactions in the pool\n",
	       locked);
	printf("Available : %lu coins\n", confirmed - locked);
	return (0);
	(void)argv;
}

/**
 * tx_net - computes what a transaction gives to and takes from the wallet
 * @blockchain: Blockchain used to resolve the inputs
 * @tx: transaction
 * @me: public key of the wallet
 * @received: where to store the coins received by the wallet
 * @spent: where to store the coins spent by the wallet
 */
static void tx_net(blockchain_t *blockchain, transaction_t const *tx,
		   uint8_t const *me, unsigned long *received,
		   unsigned long *spent)
{
	tx_out_t *out;
	int i;

	*received = 0;
	*spent = 0;
	for (i = 0; i < llist_size(tx->outputs); i++)
	{
		out = llist_get_node_at(tx->outputs, i);
		if (!memcmp(out->pub, me, EC_PUB_LEN))
			*received += out->amount;
	}
	for (i = 0; !tx_is_coinbase(tx) && i < llist_size(tx->inputs); i++)
	{
		out = find_output(blockchain, llist_get_node_at(tx->inputs, i));
		if (out && !memcmp(out->pub, me, EC_PUB_LEN))
			*spent += out->amount;
	}
}

/**
 * print_history_line - prints one line of the wallet history
 * @block: block containing the transaction
 * @tx: transaction
 * @received: coins received by the wallet in @tx
 * @spent: coins spent by the wallet in @tx
 */
static void print_history_line(block_t const *block, transaction_t const *tx,
			       unsigned long received, unsigned long spent)
{
	char const *kind = "received";

	if (tx_is_coinbase(tx))
		kind = "mined";
	else if (spent > received)
		kind = "sent";
	printf("  block %-5u %-8s %c%-8lu ", block->info.index, kind,
	       spent > received ? '-' : '+',
	       spent > received ? spent - received : received - spent);
	print_short(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
}

/**
 * cmd_history - prints the transactions of the Blockchain that involve
 * the wallet
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0 on success, -1 on failure
 */
int cmd_history(state_t *state, char **argv)
{
	uint8_t me[EC_PUB_LEN];
	unsigned long received, spent;
	block_t *block;
	transaction_t *tx;
	int i, j, count = 0;

	if (!ec_to_pub(state->wallet, me))
		return (fprintf(stderr, "history: invalid wallet\n"), -1);
	for (i = 0; i < llist_size(state->blockchain->chain); i++)
	{
		block = llist_get_node_at(state->blockchain->chain, i);
		for (j = 0; j < llist_size(block->transactions); j++)
		{
			tx = llist_get_node_at(block->transactions, j);
			tx_net(state->blockchain, tx, me, &received, &spent);
			if (!received && !spent)
				continue;
			print_history_line(block, tx, received, spent);
			count++;
		}
	}
	printf("%d transaction%s\n", count, count == 1 ? "" : "s");
	return (0);
	(void)argv;
}
