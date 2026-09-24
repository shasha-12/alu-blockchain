#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 4096

// Function prototypes
void print_usage(const char *program_name);
void print_hash(unsigned char *hash, size_t len);
int hash_file(const char *filename, const EVP_MD *md_type);

int main(int argc, char *argv[])
{
        if (argc != 3)
        {
                print_usage(argv[0]);
                return 1;
        }

        // Initialize OpenSSL
        OpenSSL_add_all_digests();

        const EVP_MD *md_type = EVP_get_digestbyname(argv[1]);
        if (!md_type)
        {
                fprintf(stderr, "Error: Unknown hash algorithm: %s\n", argv[1]);
                return 1;
        }

        int result = hash_file(argv[2], md_type);

        // Cleanup OpenSSL
        EVP_cleanup();

        return result;
}

void print_usage(const char *program_name)
{
        fprintf(stderr, "Usage: %s <hash_algorithm> <filename>\n", program_name);
        fprintf(stderr, "Supported hash algorithms: md5, sha1, sha256, sha512\n");
}

void print_hash(unsigned char *hash, size_t len)
{
        for (size_t i = 0; i < len; i++)
        {
                printf("%02x", hash[i]);
        }
        printf("\n");
}

int hash_file(const char *filename, const EVP_MD *md_type)
{
        FILE *file = fopen(filename, "rb");
        if (!file)
        {
                perror("Error opening file");
                return 1;
        }

        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        if (!mdctx)
        {
                fprintf(stderr, "Error creating hash context\n");
                fclose(file);
                return 1;
        }

        if (EVP_DigestInit_ex(mdctx, md_type, NULL) != 1)
        {
                fprintf(stderr, "Error initializing hash function\n");
                EVP_MD_CTX_free(mdctx);
                fclose(file);
                return 1;
        }

        unsigned char buffer[BUFFER_SIZE];
        size_t bytes;
        while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) != 0)
        {
                if (EVP_DigestUpdate(mdctx, buffer, bytes) != 1)
                {
                        fprintf(stderr, "Error updating hash\n");
                        EVP_MD_CTX_free(mdctx);
                        fclose(file);
                        return 1;
                }
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len;
        if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1)
        {
                fprintf(stderr, "Error finalizing hash\n");
                EVP_MD_CTX_free(mdctx);
                fclose(file);
                return 1;
        }

        print_hash(hash, hash_len);

        EVP_MD_CTX_free(mdctx);
        fclose(file);
        return 0;
}