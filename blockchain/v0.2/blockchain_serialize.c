#include "blockchain.h"
#include <fcntl.h>
#include <unistd.h>

/**
 * write_block - writes a block to a file
 * @fd: open file descriptor
 * @block: block to write
 * Return: 0 on success else -1 on failure
 */
static int write_block(int fd, block_t const *block)
{
	if (!block ||
	    write(fd, &block->info, sizeof(block->info)) !=
	    sizeof(block->info) ||
	    write(fd, &block->data.len, 4) != 4 ||
	    write(fd, block->data.buffer, block->data.len) !=
	    (ssize_t)block->data.len ||
	    write(fd, block->hash, SHA256_DIGEST_LENGTH) !=
	    SHA256_DIGEST_LENGTH)
		return (-1);
	return (0);
}

/**
 * blockchain_serialize - serializes a blockchain into a file
 *
 * @blockchain: pointer to the Blockchain structure to be serialized
 * @path: path to a file
 *
 * Return: 0 if successful, -1 if failed
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	int fd;
	int32_t i, nb_blocks;
	uint8_t endianness = _get_endianness();

	if (!blockchain || !path)
		return (-1);
	nb_blocks = llist_size(blockchain->chain);
	if (nb_blocks == -1)
		return (-1);
	fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
		return (-1);
	if (write(fd, HBLK_MAGIC, 4) != 4 || write(fd, HBLK_VERSION, 3) != 3 ||
	    write(fd, &endianness, 1) != 1 || write(fd, &nb_blocks, 4) != 4)
		return (close(fd), -1);
	for (i = 0; i < nb_blocks; i++)
		if (write_block(fd, llist_get_node_at(blockchain->chain, i)))
			return (close(fd), -1);
	return (close(fd), 0);
}
