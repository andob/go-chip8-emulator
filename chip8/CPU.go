package chip8

import (
	"fmt"
	"math/rand"
	"strconv"
	"time"
)

const LAG = 1500000 //nanoseconds

func (chip8 *System) nextOpcode() uint16 {
	if int(chip8.ProgramCounter)+1 < len(chip8.RAM) {
		first := chip8.RAM[chip8.ProgramCounter]
		second := chip8.RAM[chip8.ProgramCounter+1]
		chip8.ProgramCounter += 2
		return (uint16(first) << 8) | uint16(second)
	}
	return 0
}

func (chip8 *System) CPUTick() {
	chip8.Lock.Lock()

	//http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
	opcode := chip8.nextOpcode()
	if opcode == 0x00E0 {
		cls(chip8)
	} else if opcode == 0x00EE {
		ret(chip8)
	} else if opcode>>0xC == 0x1 {
		jp1(chip8, opcode&0x0FFF)
	} else if opcode>>0xC == 0x2 {
		call(chip8, opcode&0x0FFF)
	} else if opcode>>0xC == 0x3 {
		se1(chip8, uint8((opcode>>8)&0x0F), uint8(opcode&0xFF))
	} else if opcode>>0xC == 0x4 {
		sne1(chip8, uint8((opcode>>8)&0x0F), uint8(opcode&0xFF))
	} else if opcode>>0xC == 0x5 && opcode&0x000F == 0 {
		se2(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x6 {
		ld1(chip8, uint8((opcode>>8)&0x0F), uint8(opcode&0xFF))
	} else if opcode>>0xC == 0x7 {
		add1(chip8, uint8((opcode>>8)&0x0F), uint8(opcode&0xFF))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x0 {
		ld2(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x1 {
		or(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x2 {
		and(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x3 {
		xor(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x4 {
		add2(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x5 {
		sub(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x6 {
		shr(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0x7 {
		subn(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0x8 && opcode&0x000F == 0xE {
		shl(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0x9 && opcode&0x000F == 0x0 {
		sne2(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F))
	} else if opcode>>0xC == 0xA {
		ld3(chip8, opcode&0x0FFF)
	} else if opcode>>0xC == 0xB {
		jp2(chip8, opcode&0x0FFF)
	} else if opcode>>0xC == 0xC {
		rnd(chip8, uint8((opcode>>8)&0x0F), uint8(opcode&0xFF))
	} else if opcode>>0xC == 0xD {
		draw(chip8, uint8((opcode>>8)&0x0F), uint8((opcode>>4)&0x0F), uint8(opcode&0x0F))
	} else if opcode>>0xC == 0xE && opcode&0xFF == 0x9E {
		skp(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xE && opcode&0xFF == 0xA1 {
		sknp(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x07 {
		ld4(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x0A {
		ldk(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x15 {
		lddt(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x18 {
		ldst(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x1E {
		add3(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x29 {
		ldf(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x33 {
		ldb(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x55 {
		reg2mem(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x65 {
		mem2reg(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x75 {
		reg2backup(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode>>0xC == 0xF && opcode&0xFF == 0x85 {
		backup2reg(chip8, uint8((opcode>>8)&0x0F))
	} else if opcode != 0 {
		fmt.Printf("Opcode not implemented %s!\n", strconv.FormatInt(int64(opcode), 0xF+1))
	}

	chip8.Lock.Unlock()
	time.Sleep(LAG)
}

func cls(chip8 *System) {
	for i, row := range chip8.Display {
		for j := range row {
			chip8.Display[i][j] = false
		}
	}
}

func ret(chip8 *System) {
	//return from subroutine
	chip8.ProgramCounter = chip8.Stack.pop()
}

func jp1(chip8 *System, pointer uint16) {
	//jump to address (goto)
	chip8.ProgramCounter = pointer
}

func call(chip8 *System, pointer uint16) {
	//call subroutine
	chip8.Stack.push(chip8.ProgramCounter)
	chip8.ProgramCounter = pointer
}

func se1(chip8 *System, i uint8, value uint8) {
	//skip next instruction if register == value
	if int(i) < len(chip8.Registers) && chip8.Registers[i] == value {
		chip8.nextOpcode()
	}
}

func sne1(chip8 *System, i uint8, value uint8) {
	//skip next instruction if register != value
	if int(i) < len(chip8.Registers) && chip8.Registers[i] != value {
		chip8.nextOpcode()
	}
}

func se2(chip8 *System, i uint8, j uint8) {
	//skip next instruction if register1 == register2
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		if chip8.Registers[i] == chip8.Registers[j] {
			chip8.nextOpcode()
		}
	}
}

func ld1(chip8 *System, i uint8, value uint8) {
	//load value into register
	if int(i) < len(chip8.Registers) {
		chip8.Registers[i] = value
	}
}

func add1(chip8 *System, i uint8, value uint8) {
	//add: Ri = Ri + value
	if int(i) < len(chip8.Registers) {
		result := uint16(chip8.Registers[i]) + uint16(value)
		chip8.Registers[i] = uint8(result & 0xFF)
		if result > 0xFF {
			chip8.Registers[0x0F] = 1 //overflow
		} else {
			chip8.Registers[0x0F] = 0
		}
	}
}

func ld2(chip8 *System, i uint8, j uint8) {
	//store the value of register2 into register1
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		chip8.Registers[i] = chip8.Registers[j]
	}
}

func or(chip8 *System, i uint8, j uint8) {
	//bitwise or: Ri = Ri | Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		chip8.Registers[i] |= chip8.Registers[j]
	}
}

func and(chip8 *System, i uint8, j uint8) {
	//bitwise and: Ri = Ri & Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		chip8.Registers[i] &= chip8.Registers[j]
	}
}

func xor(chip8 *System, i uint8, j uint8) {
	//bitwise xor: Ri = Ri ^ Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		chip8.Registers[i] ^= chip8.Registers[j]
	}
}

func add2(chip8 *System, i uint8, j uint8) {
	//add: Ri = Ri + Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		result := uint16(chip8.Registers[i]) + uint16(chip8.Registers[j])
		chip8.Registers[i] = uint8(result & 0xFF)
		if result > 0xFF {
			chip8.Registers[0x0F] = 1 //overflow
		} else {
			chip8.Registers[0x0F] = 0
		}
	}
}

func sub(chip8 *System, i uint8, j uint8) {
	//substract: Ri = Ri - Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		result := int16(chip8.Registers[i]) - int16(chip8.Registers[j])
		chip8.Registers[i] = uint8(result & 0xFF)
		if result < 0 {
			chip8.Registers[0x0F] = 0 //underflow
		} else {
			chip8.Registers[0x0F] = 1
		}
	}
}

func subn(chip8 *System, i uint8, j uint8) {
	//substract: Ri = Rj - Ri
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		result := int16(chip8.Registers[j]) - int16(chip8.Registers[i])
		chip8.Registers[i] = uint8(result & 0xFF)
		if result < 0 {
			chip8.Registers[0x0F] = 0 //underflow
		} else {
			chip8.Registers[0x0F] = 1
		}
	}
}

func shr(chip8 *System, i uint8) {
	//shift right: Ri = Ri >> 1
	if int(i) < len(chip8.Registers) {
		if chip8.Registers[i]&0b00000001 == 1 {
			chip8.Registers[0x0F] = 1
		} else {
			chip8.Registers[0x0F] = 0
		}
		chip8.Registers[i] >>= 1
	}
}

func shl(chip8 *System, i uint8) {
	//shift left: Ri = Ri << 1
	if int(i) < len(chip8.Registers) {
		chip8.Registers[0x0F] = (chip8.Registers[i] & 0b10000000) >> 7
		chip8.Registers[i] <<= 1
	}
}

func sne2(chip8 *System, i uint8, j uint8) {
	//skip next instruction if Ri != Rj
	if int(i) < len(chip8.Registers) && int(j) < len(chip8.Registers) {
		if chip8.Registers[i] != chip8.Registers[j] {
			chip8.nextOpcode()
		}
	}
}

func ld3(chip8 *System, value uint16) {
	//store value into index register
	chip8.Index = value
}

func jp2(chip8 *System, pointer uint16) {
	//jump to address (goto) R0+pointer
	chip8.ProgramCounter = uint16(chip8.Registers[0]) + pointer
}

func rnd(chip8 *System, i uint8, value uint8) {
	//generate random number: Rx = random & value
	chip8.Registers[i] = uint8(rand.Int()%0xFF) & value
}

func draw(chip8 *System, ix uint8, iy uint8, height uint8) {
	if int(ix) < len(chip8.Registers) && int(iy) < len(chip8.Registers) {
		chip8.Registers[0x0F] = 0x00

		displayWidth := len(chip8.Display)
		displayHeight := len(chip8.Display[0])
		x := int(chip8.Registers[ix]) % displayWidth
		y := int(chip8.Registers[iy]) % displayHeight

		for row := 0; row < int(height); row++ {
			ny := (y + row) % displayHeight
			index := (int(chip8.Index) + row) % len(chip8.RAM)
			spriteByte := int(chip8.RAM[index])
			for bitIndex := 7; bitIndex >= 0; bitIndex-- {
				nx := (x + (7 - bitIndex)) % displayWidth
				bit := (spriteByte&(0x00000001<<bitIndex))>>bitIndex == 1
				oldValue := chip8.Display[nx][ny]
				newValue := bit != oldValue
				chip8.Display[nx][ny] = newValue
				if oldValue && !newValue { //pixel turned off
					chip8.Registers[0x0F] = 0x01
				}
			}
		}
	}
}

func skp(chip8 *System, i uint8) {
	//skip next instruction if key i is pressed
	if chip8.PressedKey != nil && *chip8.PressedKey == i {
		chip8.nextOpcode()
	}
}

func sknp(chip8 *System, i uint8) {
	//skip next instruction if key i is not pressed
	if chip8.PressedKey == nil || (chip8.PressedKey != nil && *chip8.PressedKey != i) {
		chip8.nextOpcode()
	}
}

func ld4(chip8 *System, i uint8) {
	//set Ri = delay timer
	if int(i) < len(chip8.Registers) {
		chip8.Registers[i] = chip8.DelayTimer
	}
}

func ldk(chip8 *System, i uint8) {
	//block until a key is pressed, and store the key into Ri
	if int(i) < len(chip8.Registers) {
		for chip8.PressedKey == nil {
			time.Sleep(LAG)
		}
		chip8.Registers[i] = *chip8.PressedKey
	}
}

func lddt(chip8 *System, i uint8) {
	//set delay timer = Ri
	if int(i) < len(chip8.Registers) {
		chip8.DelayTimer = chip8.Registers[i]
	}
}

func ldst(chip8 *System, i uint8) {
	//set sound timer = Ri
	if int(i) < len(chip8.Registers) {
		chip8.SoundTimer = chip8.Registers[i]
	}
}

func add3(chip8 *System, i uint8) {
	//set I = I + Ri
	if int(i) < len(chip8.Registers) {
		result := uint32(chip8.Registers[i]) + uint32(chip8.Index)
		chip8.Index = uint16(result & 0xFFFF)
		if result > 0x00FF {
			chip8.Registers[0x0F] = 1 //overflow
		} else {
			chip8.Registers[0x0F] = 0
		}
	}
}

func ldf(chip8 *System, i uint8) {
	//location of sprite of digit from font
	if int(i) < len(chip8.Registers) {
		charIndex := chip8.Registers[i]
		if int(charIndex) < len(chip8.Font.chars) {
			chip8.Index = chip8.Font.chars[charIndex].Address
		}
	}
}

func ldb(chip8 *System, i uint8) {
	//store binary coded decimal representation of Ri in RAM at I,I+1,I+2
	if int(i) < len(chip8.Registers) && int(chip8.Index)+2 < len(chip8.RAM) {
		value := chip8.Registers[i]
		chip8.RAM[chip8.Index+0] = (value / 100) % 10
		chip8.RAM[chip8.Index+1] = (value / 10) % 10
		chip8.RAM[chip8.Index+2] = value % 10
	}
}

func reg2mem(chip8 *System, amount uint8) {
	//store registers R0..R_amount into RAM at location I+0..I+amount
	if int(amount) < len(chip8.Registers) {
		for i := 0; i <= int(amount); i++ {
			pointer := int(chip8.Index) + i
			if pointer < len(chip8.RAM) {
				chip8.RAM[pointer] = chip8.Registers[i]
			}
		}
	}
}

func mem2reg(chip8 *System, amount uint8) {
	//store RAM at location I+0..I+amount into registers R0..R_amount
	if int(amount) < len(chip8.Registers) {
		for i := 0; i <= int(amount); i++ {
			pointer := int(chip8.Index) + i
			if pointer < len(chip8.RAM) {
				chip8.Registers[i] = chip8.RAM[pointer]
			}
		}
	}
}

func reg2backup(chip8 *System, amount uint8) {
	//store registers R0..R_amount into backup
	if int(amount) < len(chip8.Registers) {
		for i := 0; i <= int(amount); i++ {
			chip8.RegistersBackup[i] = chip8.Registers[i]
		}
	}
}

func backup2reg(chip8 *System, amount uint8) {
	//restore registers R0..R_amount from backup
	if int(amount) < len(chip8.Registers) {
		for i := 0; i <= int(amount); i++ {
			chip8.Registers[i] = chip8.RegistersBackup[i]
		}
	}
}
