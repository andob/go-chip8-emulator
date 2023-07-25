package chip8

import (
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/inpututil"
	"github.com/pkg/errors"
	"golang.org/x/image/colornames"
)

type Frontend struct {
	blockWidth           int
	blockHeight          int
	block                *ebiten.Image
	chip8                *System
	windowWidth          int
	windowHeight         int
	keyboardMapping      map[ebiten.Key]int
	previouslyPressedKey *ebiten.Key
}

func (frontend *Frontend) Update() error {
	var pressedKeys []ebiten.Key
	pressedKeys = inpututil.AppendPressedKeys(pressedKeys)

	chip8 := frontend.chip8
	chip8.Lock.Lock()
	defer chip8.Lock.Unlock()

	if len(pressedKeys) == 0 {
		chip8.PressedKey = nil
		frontend.previouslyPressedKey = nil
	} else {
		pressedKey := pressedKeys[0]
		if len(pressedKeys) == 1 && frontend.previouslyPressedKey == nil {
			frontend.previouslyPressedKey = &pressedKey
		} else if len(pressedKeys) > 1 && frontend.previouslyPressedKey != nil {
			//multiple keys are pressed
			for _, newlyPressedKey := range pressedKeys {
				if newlyPressedKey != *frontend.previouslyPressedKey {
					pressedKey = newlyPressedKey
				}
			}
		}

		if chip8Key, mappingExist := frontend.keyboardMapping[pressedKey]; mappingExist {
			chip8KeyReference := uint8(chip8Key)
			chip8.PressedKey = &chip8KeyReference
		}
	}

	return nil
}

func (frontend *Frontend) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
	return frontend.windowWidth, frontend.windowHeight
}

func (frontend *Frontend) Draw(screen *ebiten.Image) {
	screen.Clear()

	chip8 := frontend.chip8
	chip8.Lock.RLock()
	defer chip8.Lock.RUnlock()

	for x := 0; x < len(chip8.Display); x++ {
		for y := 0; y < len(chip8.Display[0]); y++ {
			if chip8.Display[x][y] {
				options := ebiten.DrawImageOptions{}
				options.GeoM.Translate(float64(x*frontend.blockWidth), float64(y*frontend.blockHeight))
				screen.DrawImage(frontend.block, &options)
			}
		}
	}
}

func RunFrontend(chip8 *System) error {

	frontend := Frontend{chip8: chip8}

	frontend.keyboardMapping = map[ebiten.Key]int{
		ebiten.Key1:     0x01,
		ebiten.Key2:     0x02,
		ebiten.KeyUp:    0x02,
		ebiten.Key3:     0x03,
		ebiten.Key4:     0x0C,
		ebiten.KeyQ:     0x04,
		ebiten.KeyLeft:  0x04,
		ebiten.KeyW:     0x05,
		ebiten.KeySpace: 0x05,
		ebiten.KeyE:     0x06,
		ebiten.KeyRight: 0x06,
		ebiten.KeyR:     0x0D,
		ebiten.KeyA:     0x07,
		ebiten.KeyS:     0x08,
		ebiten.KeyDown:  0x08,
		ebiten.KeyD:     0x09,
		ebiten.KeyF:     0x0E,
		ebiten.KeyZ:     0x0A,
		ebiten.KeyX:     0x00,
		ebiten.KeyC:     0x0B,
		ebiten.KeyV:     0x0F,
	}

	frontend.blockWidth = 15
	frontend.blockHeight = 15
	frontend.block = ebiten.NewImage(frontend.blockWidth, frontend.blockHeight)
	frontend.block.Clear()
	frontend.block.Fill(colornames.Greenyellow)

	frontend.windowWidth = len(chip8.Display) * frontend.blockWidth
	frontend.windowHeight = len(chip8.Display[0]) * frontend.blockHeight

	ebiten.SetWindowSize(frontend.windowWidth, frontend.windowHeight)
	ebiten.SetWindowTitle("CHIP-8")
	if err := ebiten.RunGame(&frontend); err != nil {
		return errors.Wrap(err, GetStackTraceAsString())
	}

	return nil
}
