# Blockchain CLI

A simple command line interface to interact with the Blockchain library
(`blockchain/v0.3`) and the crypto library (`crypto`).

## Build

```
make
```

## Run

```
./cli [wallet_folder]
```

At startup, the wallet (EC key pair) is loaded from `wallet_folder` if
given, otherwise a new wallet is created.

## Commands

| Command | Description |
| --- | --- |
| `wallet_load <path>` | Load wallet (EC key pair) from a folder |
| `wallet_save <path>` | Save wallet (EC key pair, PEM format) in a folder |
| `send <amount> <address>` | Create a transaction and add it to the local transaction pool |
| `mine` | Mine a Block with a coinbase and the pending transactions |
| `info` | Display the number of Blocks, unspent outputs and pending transactions |
| `load <path>` | Load a Blockchain from a file |
| `save <path>` | Save the local Blockchain into a file |
| `help` | List the commands |
| `exit` | Quit the CLI |
