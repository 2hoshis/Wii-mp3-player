# Wii MP3 Player

A simple Wii homebrew MP3 player prototype for Dolphin Emulator.

This project was created as a beginner Wii homebrew practice project using devkitPro, libogc, libfat, ASND, and MP3Player.

## Features

* Runs as Wii homebrew on Dolphin Emulator
* Reads `.mp3` files from `sd:/music`
* Displays a song list
* Wii Remote controls
* Play / stop MP3 files
* Next / previous song playback
* Pause / resume
* Reload song list from SD card
* Simple white and pink console-style UI

## Controls

| Button    | Action             |
| --------- | ------------------ |
| UP / DOWN | Select song        |
| A         | Play selected song |
| B         | Stop               |
| +         | Play next song     |
| -         | Play previous song |
| 1         | Reload `sd:/music` |
| 2         | Pause / resume     |
| HOME      | Exit               |

## Folder Structure

MP3 files should be placed in the Dolphin virtual SD card like this:

```text
sd:/music/song1.mp3
sd:/music/song2.mp3
```

On Windows, this can be done by mounting Dolphin's virtual SD card file with ImDisk.

Example Dolphin SD file location:

```text
C:\Users\testa\AppData\Roaming\Dolphin Emulator\Load\WiiSD.raw
```

After mounting the virtual SD card, create a `music` folder in the root of the mounted drive and put MP3 files inside it.

## Build Environment

* Windows
* devkitPro
* devkitPPC
* libogc
* libfat
* ASND
* MP3Player
* Dolphin Emulator

## Build

Open devkitPro MSYS2 and run:

```bash
cd /c/Users/testa/Projects/wii-homebrew-practice
make clean
make
```

The build generates a `.dol` file that can be launched with Dolphin Emulator.

## Notes

* Japanese filenames may not display correctly because the current UI uses the standard libogc console output.
* Short ASCII filenames are recommended for now.
* MP3 files are loaded fully into memory, so smaller files are recommended.
* This project is currently a prototype.

## Future Ideas

* Better graphical UI
* Japanese font rendering
* Shuffle playback
* Repeat mode
* Automatic next song playback
* Progress display
* Real Wii hardware testing
