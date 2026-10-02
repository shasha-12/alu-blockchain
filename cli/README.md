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
| `address` | Display the wallet address (public key) |
| `balance` | Display the confirmed, pending and available balance |
| `history` | List the transactions that involve the wallet |
| `chain` | List the blocks of the Blockchain |
| `block <index>` | Display a block and its transactions |
| `tx <id>` | Find a transaction by id or id prefix (8+ digits) |
| `help` | List the commands |
| `exit` | Quit the CLI |

## Extension: wallet and block explorer

The last 6 commands of the table above turn the CLI into a wallet and a
block explorer:

- `address` prints the address to give to people who want to send you coins
- `balance` separates the coins that are confirmed in the Blockchain from
  the ones already used by transactions waiting in the pool
- `history` walks the whole Blockchain and shows every transaction where the
  wallet mined, received or sent coins, with the net amount
- `chain`, `block <index>` and `tx <id>` let you browse the Blockchain:
  timestamps, hashes, and the inputs and outputs of every transaction, with
  your own outputs marked `(you)`

### Example

```
blockchain> mine
blockchain> send 10 04d2335bcb8e3a1013ff18648b55a939dcec35ea6cb8a497089d73d5a79b069f74255d4442032b61cf328d0da7fcf8822380f7876b09ffc2002f5957a619ceb0c4
blockchain> balance
blockchain> mine
blockchain> history
blockchain> chain
blockchain> block 2
blockchain> tx <first 8 digits of an id from history>
```
