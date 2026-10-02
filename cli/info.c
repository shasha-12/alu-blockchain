#include "cli.h"

/**
 * print_hex - prints a buffer in hexadecimal
 * @buf: buffer to print
 * @len: number of bytes to print
 */
void print_hex(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", buf[i]);
}

/**
 * wallet_balance - computes the coins owned by the wallet
 * @state: CLI state
 * @pub: public key of the wallet
 * Return: sum of the wallet's unspent outputs
 */
static uint64_t wallet_balance(state_t *state, uint8_t const *pub)
{
	llist_t *all_unspent = state->blockchain->unspent;
	unspent_tx_out_t *utxo;
	uint64_t balance = 0;
	int i;

	for (i = 0; i < llist_size(all_unspent); i++)
	{
		utxo = llist_get_node_at(all_unspent, i);
		if (!memcmp(utxo->out.pub, pub, EC_PUB_LEN))
			balance += utxo->out.amount;
	}
	return (balance);
}

/**
 * cmd_info - displays information about the Blockchain and the wallet
 * @state: CLI state
 * @argv: command arguments (unused)
 * Return: 0 on success, -1 on failure
 */
int cmd_info(state_t *state, char **argv)
{
	uint8_t pub[EC_PUB_LEN];
	transaction_t *tx;
	int i;

	if (!ec_to_pub(state->wallet, pub))
		return (fprintf(stderr, "info: invalid wallet\n"), -1);
	printf("Blocks in the Blockchain:      %d\n",
	       llist_size(state->blockchain->chain));
	printf("Unspent transaction outputs:   %d\n",
	       llist_size(state->blockchain->unspent));
	printf("Pending transactions in pool:  %d\n",
	       llist_size(state->tx_pool));
	for (i = 0; i < llist_size(state->tx_pool); i++)
	{
		tx = llist_get_node_at(state->tx_pool, i);
		printf("  - ");
		print_hex(tx->id, SHA256_DIGEST_LENGTH);
		printf("\n");
	}
	printf("Wallet address: ");
	print_hex(pub, EC_PUB_LEN);
	printf("\nWallet balance: %lu coins\n",
	       (unsigned long)wallet_balance(state, pub));
	return (0);
	(void)argv;
}
