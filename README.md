# Wii Homebrew Practice

This repository starts with a tiny Hello World Wii homebrew app that can be run in Dolphin.

## Files

- `source/main.c`: The app code. It sets up the Wii video console, prints a few lines of text, and exits when HOME is pressed on Wii Remote 1.
- `Makefile`: The build script. It compiles `source/main.c` with devkitPro's PowerPC compiler and converts the result into a Dolphin-friendly `.dol` file.
- `.gitignore`: Keeps generated build output out of git.

## Requirements

Install devkitPro with the Wii development tools:

- devkitPPC
- libogc

On Windows, build from the devkitPro MSYS2 shell so commands like `make` and `mkdir -p` are available.

## Build

```sh
make
```

This creates:

- `hello-world.elf`: An intermediate executable.
- `hello-world.dol`: The file to open in Dolphin.

## Run In Dolphin

Open Dolphin, then choose:

```text
File -> Open -> hello-world.dol
```

You should see:

```text
Hello World from Wii homebrew!
Run this .dol in Dolphin.
Press HOME on a Wii Remote to exit.
```

## Clean

```sh
make clean
```
