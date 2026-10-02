#include "cli.h"

static command_t const commands[] = {
	{"wallet_load", 1, "wallet_load <path>", "Load wallet (EC key pair)",
	 cmd_wallet_load},
	{"wallet_save", 1, "wallet_save <path>", "Save wallet (EC key pair)",
	 cmd_wallet_save},
	{"send", 2, "send <amount> <address>", "Send coins", cmd_send},
	{"mine", 0, "mine", "Mine a block", cmd_mine},
	{"info", 0, "info", "Display information about the Blockchain",
	 cmd_info},
	{"load", 1, "load <path>", "Load a Blockchain from a file", cmd_load},
	{"save", 1, "save <path>", "Save the local Blockchain into a file",
	 cmd_save},
	{"help", 0, "help", "Display this help", cmd_help},
	{"exit", 0, "exit", "Quit the CLI", NULL},
	{NULL, 0, NULL, NULL, NULL}
};

/**
 * cmd_help - lists the available commands
 * @state: CLI state (unused)
 * @argv: command arguments (unused)
 * Return: 0
 */
int cmd_help(state_t *state, char **argv)
{
	int i;

	printf("Available commands:\n");
	for (i = 0; commands[i].name; i++)
		printf("  %-26s %s\n", commands[i].usage, commands[i].desc);
	return (0);
	(void)state;
	(void)argv;
}

/**
 * run_line - splits a line into words and runs the matching command
 * @state: CLI state
 * @line: line typed by the user
 * Return: 1 if the CLI must exit, 0 otherwise
 */
static int run_line(state_t *state, char *line)
{
	char *argv[CLI_MAX_ARGS + 1] = {NULL};
	int argc = 0, i;

	argv[0] = strtok(line, " \t\r\n");
	if (!argv[0])
		return (0);
	while (argc < CLI_MAX_ARGS && argv[argc])
		argv[++argc] = strtok(NULL, " \t\r\n");
	for (i = 0; commands[i].name; i++)
	{
		if (strcmp(argv[0], commands[i].name))
			continue;
		if (!commands[i].run)
			return (1);
		if (argc - 1 != commands[i].argc)
		{
			fprintf(stderr, "Usage: %s\n", commands[i].usage);
			return (0);
		}
		commands[i].run(state, argv + 1);
		return (0);
	}
	fprintf(stderr, "%s: unknown command (type 'help')\n", argv[0]);
	return (0);
}

/**
 * state_init - loads or creates the wallet, and creates the Blockchain
 * @state: CLI state to initialize
 * @wallet_path: folder to load the wallet from, or NULL to create one
 * Return: 0 on success, -1 on failure
 */
static int state_init(state_t *state, char const *wallet_path)
{
	if (wallet_path)
	{
		state->wallet = ec_load(wallet_path);
		if (state->wallet)
			printf("Wallet loaded from %s\n", wallet_path);
		else
			fprintf(stderr, "Could not load wallet from %s\n",
				wallet_path);
	}
	if (!state->wallet)
	{
		state->wallet = ec_create();
		if (state->wallet)
			printf("New wallet created\n");
	}
	state->blockchain = blockchain_create();
	state->tx_pool = llist_create(MT_SUPPORT_FALSE);
	if (!state->wallet || !state->blockchain || !state->tx_pool)
		return (-1);
	return (0);
}

/**
 * state_free - frees the CLI state
 * @state: CLI state
 */
static void state_free(state_t *state)
{
	EC_KEY_free(state->wallet);
	blockchain_destroy(state->blockchain);
	llist_destroy(state->tx_pool, 1, (node_dtor_t)transaction_destroy);
}

/**
 * main - entry point of the Blockchain CLI
 * @argc: number of arguments
 * @argv: arguments, argv[1] is an optional wallet folder to load
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
	state_t state = {NULL, NULL, NULL};
	char *line = NULL;
	size_t size = 0;
	int quit = 0;

	if (state_init(&state, argc > 1 ? argv[1] : NULL))
	{
		fprintf(stderr, "Failed to initialize the CLI\n");
		state_free(&state);
		return (EXIT_FAILURE);
	}
	printf("Type 'help' to list the available commands\n");
	while (!quit)
	{
		printf(CLI_PROMPT);
		fflush(stdout);
		if (getline(&line, &size, stdin) == -1)
		{
			printf("\n");
			break;
		}
		quit = run_line(&state, line);
	}
	free(line);
	state_free(&state);
	return (EXIT_SUCCESS);
}
