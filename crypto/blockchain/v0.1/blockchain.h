#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <llist.h>
#include "../../crypto/hblk_crypto.h"

/**
 * struct block_info_s - Block info structure
 *
 * @index:      Index of the Block in the Blockchain
 * @difficulty: Difficulty of proof of work (hash leading zeros)
 * @timestamp:  Time the Block was created at (UNIX timestamp)
 * @nonce:      Salt value used to alter the Block hash
 * @prev_hash:  Hash of the previous Block in the Blockchain
 */
typedef struct block_info_s
{
        uint32_t index;
        uint32_t difficulty;
        uint64_t timestamp;
        uint64_t nonce;
        uint8_t prev_hash[32];
} block_info_t;

/**
 * struct block_s - Block structure
 *
 * @info: Block info
 * @data: Block data
 * @hash: 256-bit digest of the Block
 */
typedef struct block_s
{
        block_info_t info;
        int8_t data[32];
        uint8_t hash[32];
} block_t;

/**
 * struct blockchain_s - Blockchain structure
 *
 * @chain: Linked list of pointers to block_t
 */
typedef struct blockchain_s
{
        llist_t *chain;
} blockchain_t;

/* Function prototypes */
blockchain_t *blockchain_create(void);

#endif /* BLOCKCHAIN_H */