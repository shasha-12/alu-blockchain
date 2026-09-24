#include "hblk_crypto.h"

/**
 * ec_to_pub - Extracts public key from EC_KEY structure
 * @key: EC_KEY structure containing key pair
 * @pub: Buffer to store extracted public key
 * Return: Pointer to pub buffer or NULL on error
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	const EC_POINT *point;
	const EC_GROUP *group;
	size_t len;

	if (!key || !pub)
	{
		return (NULL);
	}

	point = EC_KEY_get0_public_key(key);
	if (!point)
	{
		return (NULL);
	}

	group = EC_KEY_get0_group(key);
	if (!group)
	{
		return (NULL);
	}

	len = EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED,
				 pub, EC_PUB_LEN, NULL);
	if (len != EC_PUB_LEN)
	{
		return (NULL);
	}

	return (pub);
}
