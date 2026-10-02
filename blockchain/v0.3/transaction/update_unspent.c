#include "transaction.h"
#include "../provided/provided.h"

/**
 * find_unspent - finds the unspent output referred to by an input
 * @node: current node, unspent_tx_out_t
 * @arg: tx_in_t referring to the unspent output
 * Return: 1 if the node matches, 0 otherwise
 */
int find_unspent(llist_node_t node, void *arg)
{
	unspent_tx_out_t *utxo = node;
	tx_in_t *txi = arg;

	return (!memcmp(txi->block_hash, utxo->block_hash, SHA256_DIGEST_LENGTH) &&
		!memcmp(txi->tx_id, utxo->tx_id, SHA256_DIGEST_LENGTH) &&
		!memcmp(txi->tx_out_hash, utxo->out.hash, SHA256_DIGEST_LENGTH));
}

/**
 * foreach_input - removes the output spent by an input from the unspent list
 * @node: current node, txi
 * @idx: index of node
 * @_args: arguments
 * Return: 0 if continue else 1
 */
int foreach_input(llist_node_t node, unsigned int idx, void *_args)
{
	void **args = _args;
	unspent_tx_out_t *utxo = llist_find_node(args[0], find_unspent, node);

	/* Unlink then free, the node is not always freed by llist_remove_node */
	if (utxo && !llist_remove_node(args[0], find_unspent, node, 0, NULL))
		free(utxo);
	return (0);
	(void)idx;
}

/**
 * foreach_output - maps output to input txs
 * @node: current node, txo
 * @idx: index of node
 * @_args: arguments
 * Return: 0 if continue else 1
 */
int foreach_output(llist_node_t node, unsigned int idx, void *_args)
{
	void **args = _args;
	tx_out_t *txo = node;
	unspent_tx_out_t *utxo;

	utxo = unspent_tx_out_create(args[1], args[2], txo);
	if (!utxo)
	{
		dprintf(2, "foreach_output: failed to create UTXO.\n");
		exit(1);
	}

	if (llist_add_node(args[0], utxo, ADD_NODE_REAR))
	{
		dprintf(2, "foreach_output: failed to add node.\n");
		exit(1);
	}
	return (0);
	(void)idx;
}

/**
 * foreach_transaction - maps output to input txs
 * @node: current node, tx
 * @idx: index of node
 * @__args: arguments
 * Return: 0 if continue else 1
 */
int foreach_transaction(llist_node_t node, unsigned int idx, void *__args)
{
	transaction_t *tx = node;
	void *args[3] = {0}, **_args = __args;

	args[0] = _args[0], args[1] = _args[1], args[2] = tx->id;
	llist_for_each(tx->inputs, foreach_input, args);
	llist_for_each(tx->outputs, foreach_output, args);
	return (0);
	(void)idx;
}
/**
 * update_unspent - updates list of UTXOs
 * @transactions: list of validate txs
 * @block_hash: hash of block containing txs
 * @all_unspent: list of all UTXOs
 * Return: new UTXO list
 */
llist_t *update_unspent(llist_t *transactions,
			uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent)
{
	void *args[2] = {0};

	args[0] = all_unspent, args[1] = block_hash;
	llist_for_each(transactions, foreach_transaction, args);
	(void)transactions;
	(void)block_hash;
	(void)all_unspent;
	return (all_unspent);
}
