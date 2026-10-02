#include "cli.h"

/**
 * cmd_wallet_load - loads the wallet (EC key pair) from a folder
 * @state: CLI state
 * @argv: argv[0] is the folder to load the key pair from
 * Return: 0 on success, -1 on failure
 */
int cmd_wallet_load(state_t *state, char **argv)
{
	EC_KEY *key;

	if (strlen(argv[0]) > CLI_MAX_PATH)
	{
		fprintf(stderr, "wallet_load: path too long\n");
		return (-1);
	}
	key = ec_load(argv[0]);
	if (!key)
	{
		fprintf(stderr, "wallet_load: failed to load wallet from %s\n",
			argv[0]);
		return (-1);
	}
	EC_KEY_free(state->wallet);
	state->wallet = key;
	printf("Wallet loaded from %s\n", argv[0]);
	return (0);
}

/**
 * cmd_wallet_save - saves the wallet (EC key pair) in PEM format
 * @state: CLI state
 * @argv: argv[0] is the folder to save the key pair in
 * Return: 0 on success, -1 on failure
 */
int cmd_wallet_save(state_t *state, char **argv)
{
	if (strlen(argv[0]) > CLI_MAX_PATH)
	{
		fprintf(stderr, "wallet_save: path too long\n");
		return (-1);
	}
	if (!ec_save(state->wallet, argv[0]))
	{
		fprintf(stderr, "wallet_save: failed to save wallet in %s\n",
			argv[0]);
		return (-1);
	}
	printf("Wallet saved in %s\n", argv[0]);
	return (0);
}
