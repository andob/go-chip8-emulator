package chip8

import (
	"bytes"
	"errors"
	"fmt"
	"github.com/inancgumus/screen"
	"io"
	"os"
	"sync"
	"time"
)

type System struct {
	RAM             [4 * 1024]uint8
	Registers       [16]uint8
	RegistersBackup [16]uint8
	ProgramCounter  uint16
	Index           uint16
	Stack           Stack
	DelayTimer      uint8
	SoundTimer      uint8
	Display         [64][32]bool
	Font            Font
	PressedKey      *uint8
	Lock            sync.RWMutex
}

func NewEmulator(romFilePath string) (*System, error) {
	chip8 := System{}
	chip8.Stack = Stack{}
	chip8.Lock = sync.RWMutex{}

	//load font into RAM
	chip8.Font = DefaultFont
	for _, character := range chip8.Font.chars {
		for index, value := range character.Data {
			chip8.RAM[int(character.Address)+index] = value
		}
	}

	//load ROM into RAM
	chip8.ProgramCounter = 0x200 //load ROM into RAM index 200
	if romFile, err := os.Open(romFilePath); err == nil {
		if romFileBytes, err := io.ReadAll(romFile); err == nil {
			for index, value := range romFileBytes {
				chip8.RAM[int(chip8.ProgramCounter)+index] = value
			}
		} else {
			return nil, errors.New("cannot read ROM file bytes")
		}
	} else {
		return nil, errors.New("cannot open ROM file")
	}

	return &chip8, nil
}

func (chip8 *System) Start() {
	go func() {
		for {
			chip8.render()
			time.Sleep(1000000000)
		}
	}()

	for {
		chip8.CPUTick()
	}
}

func (chip8 *System) render() {
	var buffer bytes.Buffer
	for y := 0; y < len(chip8.Display[0]); y++ {
		for x := 0; x < len(chip8.Display); x++ {
			if chip8.Display[x][y] {
				buffer.WriteString("▓▓▓")
			} else {
				buffer.WriteString("░░░")
			}
		}
		buffer.WriteString("\n")
	}

	screen.Clear()
	screen.MoveTopLeft()
	fmt.Println(buffer.String())
}
