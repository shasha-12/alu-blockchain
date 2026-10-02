#include "cli.h"

/**
 * select_unspent - selects enough of the wallet's unspent outputs to send
 * an amount, skipping the ones already spent by pending transactions
 * @state: CLI state
 * @amount: amount to send
 * Return: list of selected unspent outputs, or NULL if funds are too low
 */
static llist_t *select_unspent(state_t *state, uint32_t amount)
{
	llist_t *selected = llist_create(MT_SUPPORT_FALSE);
	llist_t *all_unspent = state->blockchain->unspent;
	unspent_tx_out_t *utxo;
	uint8_t pub[EC_PUB_LEN];
	uint64_t total = 0;
	int i;

	if (!selected || !ec_to_pub(state->wallet, pub))
		return (llist_destroy(selected, 0, NULL), NULL);
	for (i = 0; i < llist_size(all_unspent) && total < amount; i++)
	{
		utxo = llist_get_node_at(all_unspent, i);
		if (memcmp(utxo->out.pub, pub, EC_PUB_LEN) ||
		    utxo_in_pool(state->tx_pool, utxo))
			continue;
		if (llist_add_node(selected, utxo, ADD_NODE_REAR))
			return (llist_destroy(selected, 0, NULL), NULL);
		total += utxo->out.amount;
	}
	if (total < amount)
		return (llist_destroy(selected, 0, NULL), NULL);
	return (selected);
}

/**
 * cmd_send - creates a transaction and adds it to the transaction pool
 * @state: CLI state
 * @argv: argv[0] is the amount, argv[1] the receiver's public key
 * Return: 0 on success, -1 on failure
 */
int cmd_send(state_t *state, char **argv)
{
	uint8_t pub[EC_PUB_LEN];
	uint32_t amount;
	EC_KEY *receiver;
	llist_t *selected;
	transaction_t *tx;

	if (parse_amount(argv[0], &amount))
		return (fprintf(stderr, "send: invalid amount\n"), -1);
	receiver = parse_pub(argv[1], pub) ? NULL : ec_from_pub(pub);
	if (!receiver)
		return (fprintf(stderr, "send: invalid address\n"), -1);
	selected = select_unspent(state, amount);
	if (!selected)
	{
		EC_KEY_free(receiver);
		return (fprintf(stderr, "send: not enough coins\n"), -1);
	}
	tx = transaction_create(state->wallet, receiver, amount, selected);
	llist_destroy(selected, 0, NULL);
	EC_KEY_free(receiver);
	if (!tx || !transaction_is_valid(tx, state->blockchain->unspent) ||
	    llist_add_node(state->tx_pool, tx, ADD_NODE_REAR))
	{
		transaction_destroy(tx);
		return (fprintf(stderr, "send: transaction failed\n"), -1);
	}
	printf("Transaction created and added to the pool: ");
	print_hex(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}
