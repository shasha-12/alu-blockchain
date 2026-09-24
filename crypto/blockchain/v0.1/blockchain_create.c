#include "blockchain.h"

/**
 * blockchain_create - Creates and initializes a Blockchain structure
 *
 * Return: Pointer to the created Blockchain structure, or NULL upon failure
 */
blockchain_t *blockchain_create(void)
{
        blockchain_t *blockchain;
        block_t *genesis;

        /* Allocate memory for blockchain structure */
        blockchain = calloc(1, sizeof(*blockchain));
        if (!blockchain)
                return (NULL);

        /* Create linked list to store blocks */
        blockchain->chain = llist_create(MT_SUPPORT_TRUE);
        if (!blockchain->chain)
        {
                free(blockchain);
                return (NULL);
        }

        /* Create genesis block */
        genesis = calloc(1, sizeof(*genesis));
        if (!genesis)
        {
                llist_destroy(blockchain->chain, 1, NULL);
                free(blockchain);
                return (NULL);
        }

        /* Initialize genesis block with predefined values */
        genesis->info.index = 0;
        genesis->info.difficulty = 0;
        genesis->info.timestamp = 1537578000;
        genesis->info.nonce = 0;
        memset(genesis->info.prev_hash, 0, sizeof(genesis->info.prev_hash));

        /* Set genesis block data */
        memcpy(genesis->data, "Holberton School", 16);

        /* Set genesis block hash */
        uint8_t genesis_hash[] = {
            0xc5, 0x2c, 0x26, 0xc8, 0xb5, 0x46, 0x16, 0x39,
            0x63, 0x5d, 0x8e, 0xdf, 0x2a, 0x97, 0xd4, 0x8d,
            0x0c, 0x8e, 0x00, 0x09, 0xc8, 0x17, 0xf2, 0xb1,
            0xd3, 0xd7, 0xff, 0x2f, 0x04, 0x51, 0x58, 0x03};
        memcpy(genesis->hash, genesis_hash, sizeof(genesis->hash));

        /* Add genesis block to chain */
        if (llist_add_node(blockchain->chain, genesis, ADD_NODE_FRONT) == -1)
        {
                free(genesis);
                llist_destroy(blockchain->chain, 1, NULL);
                free(blockchain);
                return (NULL);
        }

        return (blockchain);
}