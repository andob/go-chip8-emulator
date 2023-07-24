package main

import (
	"chip8/chip8"
	"os"
)

func main() {

	if len(os.Args) <= 1 {
		panic("Syntax: chip8 <romfile>")
	}

	romFilePath := os.Args[1]
	emulator, err := chip8.NewEmulator(romFilePath)
	if err != nil {
		panic(err)
	}
	emulator.Start()

	//https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
}
