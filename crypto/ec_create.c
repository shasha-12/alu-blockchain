#include "hblk_crypto.h"

/**
 * ec_create - Creates a new EC key pair
 *
 * Return: Pointer to EC_KEY structure containing private and public key
 *         NULL on failure
 */
EC_KEY *ec_create(void)
{
	EC_KEY *key;

	key = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (!key)
	{
		return (NULL);
	}

	if (!EC_KEY_generate_key(key))
	{
		EC_KEY_free(key);
		return (NULL);
	}

	return (key);
}
