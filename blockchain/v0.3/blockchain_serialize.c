#include "blockchain.h"

/**
 * write_tx - writes a transaction to a file
 * @fd: open file descriptor
 * @tx: transaction to write
 * Return: 0 on success else -1 on failure
 */
static int write_tx(int fd, transaction_t const *tx)
{
	int32_t nb_in = llist_size(tx->inputs), nb_out = llist_size(tx->outputs);
	int32_t i;

	if (write(fd, tx->id, SHA256_DIGEST_LENGTH) != SHA256_DIGEST_LENGTH ||
	    write(fd, &nb_in, 4) != 4 || write(fd, &nb_out, 4) != 4)
		return (-1);
	for (i = 0; i < nb_in; i++)
		if (write(fd, llist_get_node_at(tx->inputs, i), TX_IN_SIZE) !=
		    TX_IN_SIZE)
			return (-1);
	for (i = 0; i < nb_out; i++)
		if (write(fd, llist_get_node_at(tx->outputs, i), TX_OUT_SIZE) !=
		    TX_OUT_SIZE)
			return (-1);
	return (0);
}

/**
 * write_block - writes a block and its transactions to a file
 * @fd: open file descriptor
 * @block: block to write
 * Return: 0 on success else -1 on failure
 */
static int write_block(int fd, block_t const *block)
{
	int32_t nb_tx = llist_size(block->transactions), i;

	if (write(fd, &block->info, sizeof(block->info)) !=
	    sizeof(block->info) ||
	    write(fd, &block->data.len, 4) != 4 ||
	    write(fd, block->data.buffer, block->data.len) !=
	    (ssize_t)block->data.len ||
	    write(fd, block->hash, SHA256_DIGEST_LENGTH) !=
	    SHA256_DIGEST_LENGTH ||
	    write(fd, &nb_tx, 4) != 4)
		return (-1);
	for (i = 0; i < nb_tx; i++)
		if (write_tx(fd, llist_get_node_at(block->transactions, i)))
			return (-1);
	return (0);
}

/**
 * blockchain_serialize - serializes blockchain to file
 * @blockchain: pointer to blockchain to serialize
 * @path: path to save file
 * Return: 0 on success else -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	int fd;
	int32_t i, nb_blocks, nb_unspent;
	uint8_t endianness = _get_endianness();

	if (!blockchain || !blockchain->chain || !path)
		return (-1);
	nb_blocks = llist_size(blockchain->chain);
	nb_unspent = llist_size(blockchain->unspent);
	if (nb_unspent < 0)
		nb_unspent = 0;
	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
		return (-1);
	if (write(fd, HBLK_MAGIC, 4) != 4 || write(fd, HBLK_VERSION, 3) != 3 ||
	    write(fd, &endianness, 1) != 1 || write(fd, &nb_blocks, 4) != 4 ||
	    write(fd, &nb_unspent, 4) != 4)
		return (close(fd), -1);
	for (i = 0; i < nb_blocks; i++)
		if (write_block(fd, llist_get_node_at(blockchain->chain, i)))
			return (close(fd), -1);
	for (i = 0; i < nb_unspent; i++)
		if (write(fd, llist_get_node_at(blockchain->unspent, i),
			  UNSPENT_SIZE) != UNSPENT_SIZE)
			return (close(fd), -1);
	return (close(fd), 0);
}
