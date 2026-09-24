#include "blockchain.h"

#define CHECK_ENDIAN(x) (endianness ? SWAPENDIAN(x) : (void)0)

/**
 * read_tx - reads a transaction from a file
 * @fd: open file descriptor
 * @endianness: 1 if endianness needs switching
 * Return: pointer to the transaction or NULL
 */
static transaction_t *read_tx(int fd, uint8_t endianness)
{
	transaction_t *tx = calloc(1, sizeof(*tx));
	int32_t nb_in, nb_out, i;
	tx_in_t *in;
	tx_out_t *out;

	if (!tx || read(fd, tx->id, SHA256_DIGEST_LENGTH) != SHA256_DIGEST_LENGTH ||
	    read(fd, &nb_in, 4) != 4 || read(fd, &nb_out, 4) != 4)
		return (free(tx), NULL);
	CHECK_ENDIAN(nb_in);
	CHECK_ENDIAN(nb_out);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs)
		return (transaction_destroy(tx), NULL);
	for (i = 0; i < nb_in; i++)
	{
		in = calloc(1, sizeof(*in));
		if (!in || read(fd, in, TX_IN_SIZE) != TX_IN_SIZE ||
		    llist_add_node(tx->inputs, in, ADD_NODE_REAR))
			return (free(in), transaction_destroy(tx), NULL);
	}
	for (i = 0; i < nb_out; i++)
	{
		out = calloc(1, sizeof(*out));
		if (!out || read(fd, out, TX_OUT_SIZE) != TX_OUT_SIZE ||
		    llist_add_node(tx->outputs, out, ADD_NODE_REAR))
			return (free(out), transaction_destroy(tx), NULL);
		CHECK_ENDIAN(out->amount);
	}
	return (tx);
}

/**
 * read_block - reads a block and its transactions from a file
 * @fd: open file descriptor
 * @endianness: 1 if endianness needs switching
 * Return: pointer to the block or NULL
 */
static block_t *read_block(int fd, uint8_t endianness)
{
	block_t *block = calloc(1, sizeof(*block));
	transaction_t *tx;
	int32_t nb_tx, i;

	if (!block || read(fd, &block->info, sizeof(block->info)) !=
	    sizeof(block->info) || read(fd, &block->data.len, 4) != 4)
		return (free(block), NULL);
	CHECK_ENDIAN(block->info.index);
	CHECK_ENDIAN(block->info.difficulty);
	CHECK_ENDIAN(block->info.timestamp);
	CHECK_ENDIAN(block->info.nonce);
	CHECK_ENDIAN(block->data.len);
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
	    read(fd, block->data.buffer, block->data.len) !=
	    (ssize_t)block->data.len ||
	    read(fd, block->hash, SHA256_DIGEST_LENGTH) !=
	    SHA256_DIGEST_LENGTH || read(fd, &nb_tx, 4) != 4)
		return (free(block), NULL);
	CHECK_ENDIAN(nb_tx);
	if (nb_tx == -1)
		return (block);
	block->transactions = llist_create(MT_SUPPORT_FALSE);
	if (!block->transactions)
		return (free(block), NULL);
	for (i = 0; i < nb_tx; i++)
	{
		tx = read_tx(fd, endianness);
		if (!tx || llist_add_node(block->transactions, tx, ADD_NODE_REAR))
			return (transaction_destroy(tx), block_destroy(block), NULL);
	}
	return (block);
}

/**
 * deserialize_blocks - deserializes all the blocks in the file
 * @fd: open fd to save file
 * @size: number of blocks in the file
 * @endianness: if endianess needs switching
 * Return: pointer to list of blocks or NULL
 */
llist_t *deserialize_blocks(int fd, uint32_t size, uint8_t endianness)
{
	block_t *block;
	llist_t *list = llist_create(MT_SUPPORT_TRUE);
	uint32_t i;

	if (!list)
		return (NULL);
	for (i = 0; i < size; i++)
	{
		block = read_block(fd, endianness);
		if (!block || llist_add_node(list, block, ADD_NODE_REAR))
		{
			block_destroy(block);
			llist_destroy(list, 1, (node_dtor_t)block_destroy);
			return (NULL);
		}
	}
	return (list);
}

/**
 * read_unspent - reads the unspent transaction outputs from a file
 * @fd: open file descriptor
 * @size: number of unspent outputs in the file
 * @endianness: 1 if endianness needs switching
 * Return: pointer to list of unspent outputs or NULL
 */
static llist_t *read_unspent(int fd, uint32_t size, uint8_t endianness)
{
	unspent_tx_out_t *utxo;
	llist_t *list = llist_create(MT_SUPPORT_TRUE);
	uint32_t i;

	if (!list)
		return (NULL);
	for (i = 0; i < size; i++)
	{
		utxo = calloc(1, sizeof(*utxo));
		if (!utxo || read(fd, utxo, UNSPENT_SIZE) != UNSPENT_SIZE ||
		    llist_add_node(list, utxo, ADD_NODE_REAR))
			return (free(utxo), llist_destroy(list, 1, free), NULL);
		CHECK_ENDIAN(utxo->out.amount);
	}
	return (list);
}

/**
 * blockchain_deserialize - deserializes blockchain from file
 * @path: path to serialized blockchain file
 * Return: pointer to deserialized blockchain or null
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	int fd;
	blockchain_t *chain;
	uint8_t endianness;
	char buf[5] = {0};
	uint32_t nb_blocks, nb_unspent;

	if (!path)
		return (NULL);
	fd = open(path, O_RDONLY);
	if (fd == -1)
		return (NULL);
	if (read(fd, buf, 4) != 4 || strcmp(buf, HBLK_MAGIC))
		return (close(fd), NULL);
	memset(buf, 0, sizeof(buf));
	if (read(fd, buf, 3) != 3 || strcmp(buf, HBLK_VERSION) ||
	    read(fd, &endianness, 1) != 1 || read(fd, &nb_blocks, 4) != 4 ||
	    read(fd, &nb_unspent, 4) != 4)
		return (close(fd), NULL);
	endianness = endianness != _get_endianness();
	CHECK_ENDIAN(nb_blocks);
	CHECK_ENDIAN(nb_unspent);
	chain = calloc(1, sizeof(*chain));
	if (!chain)
		return (close(fd), NULL);
	chain->chain = deserialize_blocks(fd, nb_blocks, endianness);
	if (chain->chain)
		chain->unspent = read_unspent(fd, nb_unspent, endianness);
	if (!chain->chain || !chain->unspent)
		return (close(fd), blockchain_destroy(chain), NULL);
	return (close(fd), chain);
}
