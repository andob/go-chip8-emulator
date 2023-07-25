package chip8

import (
	"github.com/pkg/errors"
	"io"
	"os"
	"sync"
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
			return nil, errors.Wrap(errors.New("cannot read ROM file bytes"), GetStackTraceAsString())
		}
	} else {
		return nil, errors.Wrap(errors.New("cannot open ROM file"), GetStackTraceAsString())
	}

	return &chip8, nil
}

func (chip8 *System) Start() error {

	go func() {
		for {
			chip8.cpuTick()
		}
	}()

	go func() {
		for {
			chip8.delayTimerTick()
		}
	}()

	go func() {
		for {
			chip8.soundTimerTick()
		}
	}()

	err := RunFrontend(chip8)
	if err != nil {
		return errors.Wrap(err, GetStackTraceAsString())
	}

	return nil
}
