#include "cli.h"

/**
 * parse_amount - parses a strictly positive 32-bit amount of coins
 * @s: string to parse
 * @amount: where to store the parsed amount
 * Return: 0 on success, -1 if @s is not a valid amount
 */
int parse_amount(char const *s, uint32_t *amount)
{
	unsigned long value = 0;
	size_t i;

	if (!*s || strlen(s) > 10)
		return (-1);
	for (i = 0; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		value = value * 10 + (s[i] - '0');
	}
	if (!value || value > 0xFFFFFFFFUL)
		return (-1);
	*amount = (uint32_t)value;
	return (0);
}

/**
 * parse_pub - parses a hexadecimal EC public key
 * @s: string to parse (EC_PUB_LEN * 2 hexadecimal digits)
 * @pub: where to store the public key
 * Return: 0 on success, -1 if @s is not a valid public key
 */
int parse_pub(char const *s, uint8_t pub[EC_PUB_LEN])
{
	unsigned int byte;
	size_t i;

	if (strlen(s) != EC_PUB_LEN * 2)
		return (-1);
	for (i = 0; i < EC_PUB_LEN * 2; i++)
		if (!strchr("0123456789abcdefABCDEF", s[i]))
			return (-1);
	for (i = 0; i < EC_PUB_LEN; i++)
	{
		if (sscanf(s + i * 2, "%2x", &byte) != 1)
			return (-1);
		pub[i] = (uint8_t)byte;
	}
	return (0);
}

/**
 * same_output - checks if a transaction input refers to an unspent output
 * @in: transaction input
 * @utxo: unspent transaction output
 * Return: 1 if @in refers to @utxo, 0 otherwise
 */
int same_output(tx_in_t const *in, unspent_tx_out_t const *utxo)
{
	return (!memcmp(in->block_hash, utxo->block_hash, SHA256_DIGEST_LENGTH) &&
		!memcmp(in->tx_id, utxo->tx_id, SHA256_DIGEST_LENGTH) &&
		!memcmp(in->tx_out_hash, utxo->out.hash, SHA256_DIGEST_LENGTH));
}

/**
 * utxo_in_pool - checks if a pending transaction already spends an output
 * @tx_pool: list of pending transactions
 * @utxo: unspent transaction output
 * Return: 1 if @utxo is already spent in @tx_pool, 0 otherwise
 */
int utxo_in_pool(llist_t *tx_pool, unspent_tx_out_t const *utxo)
{
	transaction_t *tx;
	int i, j;

	for (i = 0; i < llist_size(tx_pool); i++)
	{
		tx = llist_get_node_at(tx_pool, i);
		for (j = 0; j < llist_size(tx->inputs); j++)
			if (same_output(llist_get_node_at(tx->inputs, j), utxo))
				return (1);
	}
	return (0);
}

/**
 * inputs_conflict - checks if a transaction spends an output already spent
 * by one of the transactions of a list
 * @transactions: list of transactions
 * @tx: transaction to check
 * Return: 1 if there is a conflict, 0 otherwise
 */
int inputs_conflict(llist_t *transactions, transaction_t const *tx)
{
	unspent_tx_out_t ref;
	tx_in_t *in;
	int i;

	memset(&ref, 0, sizeof(ref));
	for (i = 0; i < llist_size(tx->inputs); i++)
	{
		in = llist_get_node_at(tx->inputs, i);
		memcpy(ref.block_hash, in->block_hash, SHA256_DIGEST_LENGTH);
		memcpy(ref.tx_id, in->tx_id, SHA256_DIGEST_LENGTH);
		memcpy(ref.out.hash, in->tx_out_hash, SHA256_DIGEST_LENGTH);
		if (utxo_in_pool(transactions, &ref))
			return (1);
	}
	return (0);
}
