# Woody Woodpacker

A 64-bit ELF binary packer that encrypts executables using the **ChaCha20** stream cipher.

## Overview

`woody_woodpacker` takes a 64-bit ELF executable as input, encrypts its contents with ChaCha20, and produces a packed output file named `woody`. A runtime stub embedded in the output is responsible for decrypting and executing the original binary when the packed file is run.

This project is an educational implementation demonstrating:
- ELF binary parsing and validation
- ChaCha20 stream cipher (RFC 7539)
- Cryptographically secure key/nonce generation via the Linux `getrandom()` syscall
- Binary packing and stub injection techniques

## Features

- Validates that the input file is a valid 64-bit ELF binary
- Generates a fresh 256-bit key and 96-bit nonce on every invocation using kernel entropy
- Encrypts the binary in 64-byte streaming blocks
- Appends the key and nonce (44 bytes total) to the end of the packed output
- Reserves space for a runtime unpacker stub (`STUB_SIZE` = 16 KB)

## Requirements

- Linux x86-64
- GCC or Clang (`cc`)
- GNU Make

No external libraries are required beyond the standard C library.

## Building

```bash
make
```

| Target | Description |
|--------|-------------|
| `make` / `make all` | Compile and link → produces `./woody_woodpacker` and `./stub` |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and all built executables |
| `make re` | Full rebuild (`fclean` then `all`) |

The build uses `-Wall -Wextra -Werror` — all warnings are treated as errors.

## Usage

```bash
./woody_woodpacker <path-to-64-bit-elf>
```

**Arguments**

| Argument | Description |
|----------|-------------|
| `<path-to-64-bit-elf>` | Path to the 64-bit ELF binary to pack |

**Example**

```bash
# Pack the included sample binary
./woody_woodpacker resources/sample

# Pack a system binary
./woody_woodpacker /usr/bin/ls
```

On success, the packed binary is written to `./woody` in the current directory and the generated key and nonce are printed to standard output:

```
64-bit ELF
key: <64 hex characters>   # 32-byte encryption key
key: <24 hex characters>   # 12-byte nonce
```

Run the packed binary directly:

```bash
./woody
```

The stub decrypts the payload at runtime, prints `....WOODY....` to stdout, then transparently executes the original binary.

**Error messages**

| Message | Cause |
|---------|-------|
| `Wrong ammount of args` | Not exactly 1 argument provided |
| `Can't open file` | Input file could not be opened |
| `Wront type of file` | File is not a 64-bit ELF binary |
| `Error while creating output file` | Unable to write the `woody` output file |

## How It Works

### Packing (`woody_woodpacker`)

1. **Validation** – The tool reads the first bytes of the input file and checks for the ELF magic (`\x7fELF`) and the 64-bit class flag.
2. **Key generation** – A 32-byte key and 12-byte nonce are generated using `getrandom()`.
3. **Encryption** – The binary is read in 64-byte blocks. For each block, a ChaCha20 keystream is generated and XORed with the plaintext.
4. **Output** – The `stub` executable is written first, followed by the encrypted payload, followed by the 32-byte key and the 12-byte nonce. The resulting file is saved as `./woody`.

### Runtime unpacking (`woody`)

When `./woody` is executed, the embedded stub runs automatically:

1. **Self-read** – The stub opens `/proc/self/exe` and reads the key (bytes `[-44:-12]`) and nonce (last 12 bytes) from its own file.
2. **Decryption** – The encrypted payload (everything after `STUB_SIZE` bytes and before the key/nonce) is decrypted in 64-byte blocks using the same ChaCha20 keystream.
3. **In-memory execution** – The decrypted binary is written to an anonymous in-memory file via `memfd_create`, then executed with `execve` — the original binary never touches disk.

### ChaCha20 Implementation

The cipher follows RFC 7539:
- **State** – 16 × 32-bit words: 4 constants (`"expand 32-byte k"`), 8 key words, 1 counter, 3 nonce words.
- **Rounds** – 10 double-rounds (20 quarter-rounds total) mixing columns then diagonals.
- **Quarter-round** – Add, XOR, and rotate-left operations with rotations of 16, 12, 8, and 7 bits.

## Project Structure

```
woody_woodpacker/
├── Makefile
├── src/
│   ├── main.c              # Entry point – packing logic
│   ├── chacha20.c          # ChaCha20 cipher implementation
│   ├── woody_woodpacker.h  # Shared definitions and prototypes
│   └── stub.c              # Runtime decryption stub
└── resources/
    ├── sample.c            # Sample source for testing
    ├── sample              # Compiled sample ELF binary
    └── woody               # Example packed output
```

## Current Status

| Component | Status |
|-----------|--------|
| ELF validation | ✅ Complete |
| ChaCha20 encryption | ✅ Complete |
| Key/nonce generation | ✅ Complete |
| Packed output creation | ✅ Complete |
| Runtime unpacker stub | ✅ Complete |
