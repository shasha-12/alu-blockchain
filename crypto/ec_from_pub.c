#include "hblk_crypto.h"

/**
 * ec_from_pub - Creates EC_KEY from public key
 * @pub: Buffer containing public key
 *
 * Return: Pointer to created EC_KEY structure, NULL on error
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN])
{
	EC_KEY *key = NULL;
	EC_POINT *point = NULL;
	const EC_GROUP *group = NULL;

	if (!pub)
		return (NULL);

	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	group = EC_KEY_get0_group(key);
	if (!group)
	{
		EC_KEY_free(key);
		return (NULL);
	}

	point = EC_POINT_new(group);
	if (!point)
	{
		EC_KEY_free(key);
		return (NULL);
	}
	if (!EC_POINT_oct2point(group, point, pub, EC_PUB_LEN, NULL))
	{
		EC_POINT_free(point);
		EC_KEY_free(key);
		return (NULL);
	}

	if (!EC_KEY_set_public_key(key, point))
	{
		EC_POINT_free(point);
		EC_KEY_free(key);
		return (NULL);
	}
	EC_POINT_free(point);
	return (key);
}
