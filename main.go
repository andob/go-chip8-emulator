package main

import (
	"chip8/chip8"
	"github.com/pkg/errors"
	"os"
)

func main() {

	if len(os.Args) <= 1 {
		panic("Syntax: chip8 <romfile>")
	}

	romFilePath := os.Args[1]
	if _, err := os.Stat(romFilePath); errors.Is(err, os.ErrNotExist) {
		panic("ROM file does not exist!")
	}

	emulator, err := chip8.NewEmulator(romFilePath)
	if err != nil {
		panic(err)
	}

	err = emulator.Start()
	if err != nil {
		panic(err)
	}
}
