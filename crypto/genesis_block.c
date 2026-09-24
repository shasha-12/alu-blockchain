#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define DIFFICULTY 4 // Number of leading zeros required in hash
#define MAX_NONCE 100000000
#define COIN_BASE_REWARD 50.0 // Initial mining reward
#define GENESIS_MESSAGE "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks"

// Structure for transaction
typedef struct
{
        char from[64];
        char to[64];
        double amount;
        char signature[128];
} Transaction;

// Structure for block header
typedef struct
{
        unsigned int version;
        unsigned char previousHash[SHA256_DIGEST_LENGTH];
        unsigned char merkleRoot[SHA256_DIGEST_LENGTH];
        time_t timestamp;
        unsigned int difficulty;
        unsigned int nonce;
} BlockHeader;

// Structure for the complete block
typedef struct
{
        BlockHeader header;
        unsigned int transactionCount;
        Transaction *transactions;
        unsigned char hash[SHA256_DIGEST_LENGTH];
} Block;

// Function to calculate double SHA256 hash
void calculateDoubleHash(const void *input, size_t length, unsigned char *hash)
{
        unsigned char tempHash[SHA256_DIGEST_LENGTH];

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input, length);
        SHA256_Final(tempHash, &sha256);

        // Second round of SHA256
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, tempHash, SHA256_DIGEST_LENGTH);
        SHA256_Final(hash, &sha256);
}

// Function to calculate merkle root
void calculateMerkleRoot(Transaction *transactions, int count, unsigned char *merkleRoot)
{
        if (count == 0)
        {
                memset(merkleRoot, 0, SHA256_DIGEST_LENGTH);
                return;
        }

        // For genesis block, we typically only have the coinbase transaction
        calculateDoubleHash(transactions, sizeof(Transaction), merkleRoot);
}

// Function to check if hash meets difficulty requirement
int checkDifficulty(unsigned char *hash, unsigned int difficulty)
{
        for (unsigned int i = 0; i < difficulty / 2; i++)
        {
                if (hash[i] != 0)
                        return 0;
        }
        return 1;
}

// Function to create a coinbase transaction
Transaction createCoinbaseTransaction()
{
        Transaction coinbase;
        memset(&coinbase, 0, sizeof(Transaction));
        strcpy(coinbase.from, "0000000000000000000000000000000000000000");
        strcpy(coinbase.to, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"); // First Bitcoin address
        coinbase.amount = COIN_BASE_REWARD;
        strcpy(coinbase.signature, "GENESIS_COINBASE");
        return coinbase;
}

// Function to create the genesis block
Block *createGenesisBlock()
{
        Block *genesis = (Block *)malloc(sizeof(Block));
        if (!genesis)
                return NULL;

        // Initialize block
        memset(genesis, 0, sizeof(Block));

        // Set block version
        genesis->header.version = 1;

        // Set timestamp (using Bitcoin's genesis block timestamp)
        genesis->header.timestamp = 1231006505; // 2009-01-03 18:15:05 UTC

        // Set difficulty
        genesis->header.difficulty = DIFFICULTY;

        // Create and add coinbase transaction
        genesis->transactionCount = 1;
        genesis->transactions = (Transaction *)malloc(sizeof(Transaction));
        if (!genesis->transactions)
        {
                free(genesis);
                return NULL;
        }
        genesis->transactions[0] = createCoinbaseTransaction();

        // Calculate merkle root
        calculateMerkleRoot(genesis->transactions, 1, genesis->header.merkleRoot);

        // Mining process (finding nonce)
        unsigned char hash[SHA256_DIGEST_LENGTH];
        char blockHeader[sizeof(BlockHeader)];
        unsigned int nonce = 0;

        do
        {
                genesis->header.nonce = nonce++;
                memcpy(blockHeader, &genesis->header, sizeof(BlockHeader));
                calculateDoubleHash(blockHeader, sizeof(BlockHeader), hash);
        } while (!checkDifficulty(hash, DIFFICULTY) && nonce < MAX_NONCE);

        if (nonce >= MAX_NONCE)
        {
                free(genesis->transactions);
                free(genesis);
                return NULL;
        }

        // Store the final hash
        memcpy(genesis->hash, hash, SHA256_DIGEST_LENGTH);

        return genesis;
}

// Function to print block information in a formatted way
void printBlock(const Block *block)
{
        printf("\n=== GENESIS BLOCK ===\n");
        printf("Version: %u\n", block->header.version);
        printf("Timestamp: %s", ctime(&block->header.timestamp));
        printf("Difficulty: %u\n", block->header.difficulty);
        printf("Nonce: %u\n", block->header.nonce);

        printf("\nMerkle Root: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
                printf("%02x", block->header.merkleRoot[i]);
        }

        printf("\nBlock Hash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
                printf("%02x", block->hash[i]);
        }

        printf("\n\nCoinbase Transaction:");
        printf("\nFrom: %s", block->transactions[0].from);
        printf("\nTo: %s", block->transactions[0].to);
        printf("\nAmount: %.8f\n", block->transactions[0].amount);

        printf("\nGenesis Message: %s\n", GENESIS_MESSAGE);
}

int main()
{
        printf("Creating Genesis Block...\n");

        Block *genesisBlock = createGenesisBlock();
        if (!genesisBlock)
        {
                printf("Failed to create genesis block!\n");
                return 1;
        }

        printBlock(genesisBlock);

        // Clean up
        free(genesisBlock->transactions);
        free(genesisBlock);

        return 0;
}