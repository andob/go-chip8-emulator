package chip8

import (
	"time"
)

const TimerLag = 16666667 //60Hz in nanoseconds

func (chip8 *System) delayTimerTick() {
	chip8.Lock.Lock()
	if chip8.DelayTimer > 0 {
		chip8.DelayTimer--
	}
	chip8.Lock.Unlock()
	time.Sleep(TimerLag)
}

func (chip8 *System) soundTimerTick() {
	chip8.Lock.Lock()
	if chip8.SoundTimer > 0 {
		chip8.SoundTimer--
	}
	chip8.Lock.Unlock()
	//do beep sound here
	time.Sleep(TimerLag)
}
