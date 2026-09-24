#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE SHA256_DIGEST_LENGTH

// Structure for a block in the blockchain
typedef struct Block
{
        unsigned int index;
        time_t timestamp;
        char data[MAX_DATA_SIZE];
        unsigned char previousHash[HASH_SIZE];
        unsigned char hash[HASH_SIZE];
        struct Block *next;
} Block;

// Function to calculate SHA256 hash
void calculateHash(Block *block, unsigned char hash[HASH_SIZE])
{
        char input[512];
        sprintf(input, "%d%ld%s", block->index, block->timestamp, block->data);

        if (block->index > 0)
        {
                char prevHashStr[HASH_SIZE * 2 + 1];
                for (int i = 0; i < HASH_SIZE; i++)
                {
                        sprintf(&prevHashStr[i * 2], "%02x", block->previousHash[i]);
                }
                strcat(input, prevHashStr);
        }

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input, strlen(input));
        SHA256_Final(hash, &sha256);
}

// Function to create genesis block
Block *createGenesisBlock()
{
        Block *genesis = (Block *)malloc(sizeof(Block));
        if (genesis == NULL)
        {
                return NULL;
        }

        genesis->index = 0;
        genesis->timestamp = time(NULL);
        strcpy(genesis->data, "Genesis Block");
        memset(genesis->previousHash, 0, HASH_SIZE);
        calculateHash(genesis, genesis->hash);
        genesis->next = NULL;

        return genesis;
}

// Function to create a new block
Block *createBlock(Block *previousBlock, const char *data)
{
        if (previousBlock == NULL || data == NULL)
        {
                return NULL;
        }

        Block *newBlock = (Block *)malloc(sizeof(Block));
        if (newBlock == NULL)
        {
                return NULL;
        }

        newBlock->index = previousBlock->index + 1;
        newBlock->timestamp = time(NULL);
        strncpy(newBlock->data, data, MAX_DATA_SIZE - 1);
        newBlock->data[MAX_DATA_SIZE - 1] = '\0';
        memcpy(newBlock->previousHash, previousBlock->hash, HASH_SIZE);
        calculateHash(newBlock, newBlock->hash);
        newBlock->next = NULL;

        previousBlock->next = newBlock;
        return newBlock;
}

// Function to verify block integrity
int verifyBlock(Block *block)
{
        if (block == NULL)
        {
                return 0;
        }

        unsigned char calculatedHash[HASH_SIZE];
        calculateHash(block, calculatedHash);

        return memcmp(calculatedHash, block->hash, HASH_SIZE) == 0;
}

// Function to print block information
void printBlock(Block *block)
{
        if (block == NULL)
        {
                printf("Invalid block\n");
                return;
        }

        printf("\nBlock #%u\n", block->index);
        printf("Timestamp: %ld\n", block->timestamp);
        printf("Data: %s\n", block->data);
        printf("Hash: ");
        for (int i = 0; i < HASH_SIZE; i++)
        {
                printf("%02x", block->hash[i]);
        }
        printf("\nPrevious Hash: ");
        for (int i = 0; i < HASH_SIZE; i++)
        {
                printf("%02x", block->previousHash[i]);
        }
        printf("\n");
}

// Example usage
int main()
{
        // Create genesis block
        Block *blockchain = createGenesisBlock();
        if (blockchain == NULL)
        {
                printf("Failed to create genesis block\n");
                return 1;
        }
        printBlock(blockchain);

        // Add blocks to the chain
        Block *block1 = createBlock(blockchain, "First Transaction Data");
        if (block1 != NULL)
        {
                printBlock(block1);
        }

        Block *block2 = createBlock(block1, "Second Transaction Data");
        if (block2 != NULL)
        {
                printBlock(block2);
        }

        sleep(2);

        Block *block3 = createBlock(block2, "Third Transaction Data");
        if (block3 != NULL)
        {
                printBlock(block3);
        }

        // Verify blockchain integrity
        Block *current = blockchain;
        while (current != NULL)
        {
                if (!verifyBlock(current))
                {
                        printf("Blockchain verification failed at block %u\n", current->index);
                        break;
                }
                current = current->next;
        }

        // Clean up
        current = blockchain;
        while (current != NULL)
        {
                Block *temp = current;
                current = current->next;
                free(temp);
        }

        return 0;
}