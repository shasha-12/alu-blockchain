#include "cli.h"

/**
 * cmd_load - loads a Blockchain from a file, overriding the local one
 * @state: CLI state
 * @argv: argv[0] is the path of the file to load
 * Return: 0 on success, -1 on failure
 */
int cmd_load(state_t *state, char **argv)
{
	blockchain_t *blockchain = blockchain_deserialize(argv[0]);

	if (!blockchain)
	{
		fprintf(stderr, "load: failed to load a Blockchain from %s\n",
			argv[0]);
		return (-1);
	}
	blockchain_destroy(state->blockchain);
	state->blockchain = blockchain;
	printf("Blockchain loaded from %s (%d blocks)\n", argv[0],
	       llist_size(blockchain->chain));
	return (0);
}

/**
 * cmd_save - saves the local Blockchain into a file
 * @state: CLI state
 * @argv: argv[0] is the path of the file to save to
 * Return: 0 on success, -1 on failure
 */
int cmd_save(state_t *state, char **argv)
{
	if (blockchain_serialize(state->blockchain, argv[0]))
	{
		fprintf(stderr, "save: failed to save the Blockchain in %s\n",
			argv[0]);
		return (-1);
	}
	printf("Blockchain saved in %s\n", argv[0]);
	return (0);
}
