package main

import "chip8/chip8"

func main() {

	romFilePath := "./IBM Logo.ch8"
	emulator, err := chip8.NewEmulator(romFilePath)
	if err != nil {
		panic(err)
	}
	emulator.Start()

	//https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
}
