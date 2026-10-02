#include "blockchain.h"

/**
 * hash_matches_difficulty - checks if hash matches difficulty
 * @hash: hash to check
 * @difficulty: minimum number of leading zero bits in the hash
 * Return: 1 if hash matches difficulty, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
			    uint32_t difficulty)
{
	uint32_t i, full_bytes, extra_bits;

	if (!hash || difficulty > SHA256_DIGEST_LENGTH * 8)
		return (0);
	full_bytes = difficulty / 8;
	extra_bits = difficulty % 8;
	for (i = 0; i < full_bytes; i++)
		if (hash[i])
			return (0);
	if (extra_bits && hash[full_bytes] >> (8 - extra_bits))
		return (0);
	return (1);
}
