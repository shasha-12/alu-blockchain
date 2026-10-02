#include "cli.h"

/**
 * print_short - prints the first bytes of a buffer in hexadecimal
 * @buf: buffer to print
 * @len: total length of the buffer
 */
void print_short(uint8_t const *buf, size_t len)
{
	print_hex(buf, len < CLI_SHORT_LEN ? len : CLI_SHORT_LEN);
	if (len > CLI_SHORT_LEN)
		printf("...");
}

/**
 * tx_is_coinbase - checks if a transaction is a coinbase transaction
 * @tx: transaction to check
 * Return: 1 if @tx is a coinbase transaction, 0 otherwise
 */
int tx_is_coinbase(transaction_t const *tx)
{
	uint8_t zero[SHA256_DIGEST_LENGTH] = {0};
	tx_in_t *in;

	if (llist_size(tx->inputs) != 1 || llist_size(tx->outputs) != 1)
		return (0);
	in = llist_get_node_at(tx->inputs, 0);
	return (!memcmp(in->block_hash, zero, SHA256_DIGEST_LENGTH) &&
		!memcmp(in->tx_id, zero, SHA256_DIGEST_LENGTH));
}

/**
 * find_output - finds the transaction output referred to by an input
 * @blockchain: Blockchain to search
 * @in: transaction input
 * Return: pointer to the output, or NULL if not found
 */
tx_out_t *find_output(blockchain_t *blockchain, tx_in_t const *in)
{
	block_t *block;
	transaction_t *tx;
	tx_out_t *out;
	int i, j, k;

	for (i = 0; i < llist_size(blockchain->chain); i++)
	{
		block = llist_get_node_at(blockchain->chain, i);
		if (memcmp(block->hash, in->block_hash, SHA256_DIGEST_LENGTH))
			continue;
		for (j = 0; j < llist_size(block->transactions); j++)
		{
			tx = llist_get_node_at(block->transactions, j);
			if (memcmp(tx->id, in->tx_id, SHA256_DIGEST_LENGTH))
				continue;
			for (k = 0; k < llist_size(tx->outputs); k++)
			{
				out = llist_get_node_at(tx->outputs, k);
				if (!memcmp(out->hash, in->tx_out_hash,
					    SHA256_DIGEST_LENGTH))
					return (out);
			}
		}
	}
	return (NULL);
}

/**
 * print_output - prints an amount and the address it belongs to
 * @out: transaction output
 * @me: public key of the wallet, to highlight its outputs
 */
void print_output(tx_out_t const *out, uint8_t const *me)
{
	printf("%u coins  ", out->amount);
	print_short(out->pub, EC_PUB_LEN);
	if (!memcmp(out->pub, me, EC_PUB_LEN))
		printf(" (you)");
	printf("\n");
}

/**
 * print_tx - prints a transaction with its inputs and outputs
 * @blockchain: Blockchain used to resolve the inputs
 * @tx: transaction to print
 * @me: public key of the wallet, to highlight its inputs and outputs
 */
void print_tx(blockchain_t *blockchain, transaction_t const *tx,
	      uint8_t const *me)
{
	tx_out_t *out;
	int i, coinbase = tx_is_coinbase(tx);

	printf("    tx ");
	print_hex(tx->id, SHA256_DIGEST_LENGTH);
	printf("%s\n", coinbase ? "  [coinbase]" : "");
	if (coinbase)
		printf("      in : new coins (mining reward)\n");
	for (i = 0; !coinbase && i < llist_size(tx->inputs); i++)
	{
		out = find_output(blockchain, llist_get_node_at(tx->inputs, i));
		printf("      in : ");
		if (out)
			print_output(out, me);
		else
			printf("unknown output\n");
	}
	for (i = 0; i < llist_size(tx->outputs); i++)
	{
		printf("      out: ");
		print_output(llist_get_node_at(tx->outputs, i), me);
	}
}
