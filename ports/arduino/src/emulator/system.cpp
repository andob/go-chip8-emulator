#include "emulator/system.h"
#include "emulator/font.h"
#include <Arduino.h>

#define enableOpcodeLogging
#undef enableOpcodeLogging

System::System(const uint8_t* romBytes, const size_t romLength) {
    //initialise fields
    this->programCounter = this->index = 0;
    this->delayTimer = this->soundTimer = 0;
    this->isKeyPressed = this->pressedKey = 0;

    //clear memory
    for (int i = 0; i < ramSize; i++) {
        this->ram[i] = 0;
    }

    for (int i = 0; i < registersSize; i++) {
        this->registers[i] = this->registersBackup[i] = 0;
    }

    //load font into RAM
    for (int i = 0; i < nofCharsInFont; i++) {
        uint16_t baseAddress = baseFontAddress + i * nofBytesInFontChar;
        for (int j = 0; j < nofBytesInFontChar; j++) {
            const uint16_t address = baseAddress + j;
            const uint8_t value = pgm_read_byte(&fontData[i][j]);
            this->ram[address % ramSize] = value;
        }
    }

    //load ROM into RAM
    this->programCounter = 0x200; //load ROM into RAM index 200

    for (uint16_t i = 0; i < romLength; i++) {
        //todo ROM should be loaded from SD card
        //todo this->ram[this->programCounter + i] = romBytes[i];
        this->ram[this->programCounter + i] = pgm_read_byte(&romBytes[i]);
    }

    //clear display
    this->cls();
}

uint16_t System::nextOpcode() {
    if (this->programCounter+1 < ramSize) {
        const uint16_t first = this->ram[this->programCounter];
        const uint16_t second = this->ram[this->programCounter+1];
        this->programCounter += 2;
        return first << 8 | second;
    }

    return 0;
}

void System::cpuTick() {
    //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    const uint16_t opcode = this->nextOpcode();

    if (opcode == 0x00E0) this->cls();
    else if (opcode == 0x00EE) this->ret();
    else if ((opcode>>0xC) == 0x1) this->jp1(opcode&0x0FFF);
    else if ((opcode>>0xC) == 0x2) this->call(opcode&0x0FFF);
    else if ((opcode>>0xC) == 0x3) this->se1((opcode>>8)&0x0F, opcode&0xFF);
    else if ((opcode>>0xC) == 0x4) this->sne1((opcode>>8)&0x0F, opcode&0xFF);
    else if ((opcode>>0xC) == 0x5 && (opcode&0x000F) == 0) this->se2((opcode>>8)&0x0F, (opcode>>4)&0x0F);
    else if ((opcode>>0xC) == 0x6) this->ld1(((opcode>>8)&0x0F), (opcode&0xFF));
    else if ((opcode>>0xC) == 0x7) this->add1(((opcode>>8)&0x0F), (opcode&0xFF));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x0) this->ld2(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x1) this->_or(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x2) this->_and(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x3) this->_xor(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x4) this->add2(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x5) this->sub(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x6) this->shr(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0x7) this->subn(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0x8 && (opcode&0x000F) == 0xE) this->shl(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0x9 && (opcode&0x000F) == 0x0) this->sne2(((opcode>>8)&0x0F), ((opcode>>4)&0x0F));
    else if ((opcode>>0xC) == 0xA) this->ld3(opcode&0x0FFF);
    else if ((opcode>>0xC) == 0xB) this->jp2(opcode&0x0FFF);
    else if ((opcode>>0xC) == 0xC) this->rnd(((opcode>>8)&0x0F), (opcode&0xFF));
    else if ((opcode>>0xC) == 0xD) this->draw(((opcode>>8)&0x0F), ((opcode>>4)&0x0F), (opcode&0x0F));
    else if ((opcode>>0xC) == 0xE && (opcode&0xFF) == 0x9E) this->skp(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xE && (opcode&0xFF) == 0xA1) this->sknp(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x07) this->ld4(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x0A) this->ldk(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x15) this->lddt(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x18) this->ldst(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x1E) this->add3(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x29) this->ldf(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x33) this->ldb(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x55) this->reg2mem(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x65) this->mem2reg(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x75) this->reg2backup(((opcode>>8)&0x0F));
    else if ((opcode>>0xC) == 0xF && (opcode&0xFF) == 0x85) this->backup2reg(((opcode>>8)&0x0F));
    else this->nop(opcode);

    #ifdef enableOpcodeLogging
    this->log(opcode);
    #endif
}

void System::cls() {
    for (uint8_t i = 0; i < displayWidth; i++) {
        for (uint8_t j = 0; j < displayHeight; j++) {
            this->display.setPixel(i, j, false);
        }
    }
}

void System::ret() {
    //return from subroutine
    if (!this->stack.isEmpty()) {
        this->programCounter = this->stack.pop();
    }
}

void System::jp1(const uint16_t pointer) {
    //jump to address (goto)
    this->programCounter = pointer;
}

void System::call(const uint16_t pointer) {
    //call subroutine
    this->stack.push(this->programCounter);
    this->programCounter = pointer;
}

void System::se1(uint8_t i, uint8_t value) {
    //skip next instruction if register == value
    if (i < registersSize && this->registers[i] == value) {
        this->nextOpcode();
    }
}

void System::sne1(uint8_t i, uint8_t value) {
    //skip next instruction if register != value
    if (i < registersSize && this->registers[i] != value) {
        this->nextOpcode();
    }
}

void System::se2(uint8_t i, uint8_t j) {
    //skip next instruction if register1 == register2
    if (i < registersSize && j < registersSize) {
        if (this->registers[i] == this->registers[j]) {
            this->nextOpcode();
        }
    }
}

void System::ld1(uint8_t i, uint8_t value) {
    //load value into register
    if (i < registersSize) {
        this->registers[i] = value;
    }
}

void System::add1(uint8_t i, uint8_t value) {
    //add: Ri = Ri + value
    if (i < registersSize) {
        const uint16_t result = static_cast<uint16_t>(this->registers[i]) + value;

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result > 0xFF ? 1 : 0;
    }
}

void System::ld2(uint8_t i, uint8_t j) {
    //store the value of register2 into register1
    if (i < registersSize && j < registersSize) {
        this->registers[i] = this->registers[j];
    }
}

void System::_or(uint8_t i, uint8_t j) {
    //bitwise or: Ri = Ri | Rj
    if (i < registersSize && j < registersSize) {
        this->registers[i] |= this->registers[j];
    }
}

void System::_and(uint8_t i, uint8_t j) {
    //bitwise and: Ri = Ri & Rj
    if (i < registersSize && j < registersSize) {
        this->registers[i] &= this->registers[j];
    }
}

void System::_xor(uint8_t i, uint8_t j) {
    //bitwise xor: Ri = Ri ^ Rj
    if (i < registersSize && j < registersSize) {
        this->registers[i] ^= this->registers[j];
    }
}

void System::add2(uint8_t i, uint8_t j) {
    //add: Ri = Ri + Rj
    if (i < registersSize && j < registersSize) {
        const uint16_t result = static_cast<uint16_t>(this->registers[i]) + this->registers[j];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result > 0xFF ? 1 : 0;
    }
}

void System::sub(uint8_t i, uint8_t j) {
    //substract: Ri = Ri - Rj
    if (i < registersSize && j < registersSize) {
        const int16_t result = static_cast<int16_t>(this->registers[i]) - this->registers[j];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result < 0 ? 0 : 1;
    }
}

void System::subn(uint8_t i, uint8_t j) {
    //substract: Ri = Rj - Ri
    if (i < registersSize && j < registersSize) {
        const int16_t result = static_cast<int16_t>(this->registers[j]) - this->registers[i];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result < 0 ? 0 : 1;
    }
}

void System::shr(uint8_t i) {
    //shift right: Ri = Ri >> 1
    if (i < registersSize) {
        this->registers[0x0F] = this->registers[i] & 0b00000001;
        this->registers[i] >>= 1;
    }
}

void System::shl(uint8_t i) {
    //shift left: Ri = Ri << 1
    if (i < registersSize) {
        this->registers[0x0F] = (this->registers[i] & 0b10000000) >> 7;
        this->registers[i] <<= 1;
    }
}

void System::sne2(uint8_t i, uint8_t j) {
    //skip next instruction if Ri != Rj
    if (i < registersSize && j < registersSize) {
        if (this->registers[i] != this->registers[j]) {
            this->nextOpcode();
        }
    }
}

void System::ld3(uint16_t value) {
    //store value into index register
    this->index = value;
}

void System::jp2(uint16_t pointer) {
    //jump to address (goto) R0+pointer
    this->programCounter = static_cast<uint16_t>(this->registers[0]) + pointer;
}

void System::rnd(uint8_t i, uint8_t value) {
    //generate random number: Rx = random & value
    const uint8_t result = random(0, 0x100) & value;
    this->registers[i] = result;
}

void System::draw(uint8_t ix, uint8_t iy, uint8_t height) {
    if (ix < registersSize && iy < registersSize) {
        this->registers[0x0F] = 0x00;

        const int32_t x = static_cast<int32_t>(this->registers[ix]) % displayWidth;
        const int32_t y = static_cast<int32_t>(this->registers[iy]) % displayHeight;

        for (uint32_t row = 0; row < height; row++) {
            const int32_t ny = (y + row) % displayHeight;
            const int32_t index = (static_cast<int32_t>(this->index) + row) % ramSize;
            const int32_t spriteByte = this->ram[index];

            for (int8_t bitIndex = 7; bitIndex >= 0; bitIndex--) {
                const int32_t nx = (x + (7 - bitIndex)) % displayWidth;
                const int32_t bit = ((spriteByte & (0x00000001 << bitIndex)) >> bitIndex) == 1;
                const bool oldValue = this->display.getPixel(nx, ny);
                const bool newValue = bit != oldValue;
                this->display.setPixel(nx, ny, newValue);

                //pixel turned off
                if (oldValue && !newValue) {
                    this->registers[0x0F] = 0x01;
                }
            }
        }
    }
}

void System::skp(uint8_t i) {
    //skip next instruction if key i is pressed
    if (i < registersSize) {
        const uint8_t expectedKey = this->registers[i];
        if (this->isKeyPressed && this->pressedKey == expectedKey) {
            this->nextOpcode();
        }
    }
}

void System::sknp(uint8_t i) {
    //skip next instruction if key i is not pressed
    if (i < registersSize) {
        const uint8_t expectedKey = this->registers[i];
        if (this->isKeyPressed && this->pressedKey != expectedKey) {
            this->nextOpcode();
        } else if (!this->isKeyPressed) {
            this->nextOpcode();
        }
    }
}

void System::ld4(uint8_t i) {
    //set Ri = delay timer
    if (i < registersSize) {
        this->registers[i] = this->delayTimer;
    }
}

void System::ldk(uint8_t i) {
    //block until a key is pressed, and store the key into Ri
    if (i < registersSize) {
        while (!this->isKeyPressed) {}

        this->registers[i] = this->pressedKey;
    }
}

void System::lddt(uint8_t i) {
    //set delay timer = Ri
    if (i < registersSize) {
        this->delayTimer = this->registers[i];
    }
}

void System::ldst(uint8_t i) {
    //set sound timer = Ri
    if (i < registersSize) {
        this->soundTimer = this->registers[i];
    }
}

void System::add3(uint8_t i) {
    //set I = I + Ri
    if (i < registersSize) {
        const uint32_t result = static_cast<uint32_t>(this->registers[i]) + this->index;

        this->index = result & 0xFFFF;
        this->registers[0x0F] = result > 0x00FF ? 1 : 0;
    }
}

void System::ldf(uint8_t i) {
    //location of sprite of digit from font
    if (i < registersSize) {
        const uint8_t charIndex = this->registers[i];
        if (charIndex < nofCharsInFont) {
            this->index = baseFontAddress + charIndex * nofBytesInFontChar;
        }
    }
}

void System::ldb(uint8_t i) {
    //store binary coded decimal representation of Ri in RAM at I,I+1,I+2
    if (i < registersSize && this->index+2 < ramSize) {
        const uint8_t value = this->registers[i];
        this->ram[this->index+0] = (value / 100) % 10;
        this->ram[this->index+1] = (value / 10) % 10;
        this->ram[this->index+2] = value % 10;
    }
}

void System::reg2mem(uint8_t amount) {
    //store registers R[0]..R[amount] into RAM at location I+0..I+amount
    if (amount < registersSize) {
        for (uint8_t i = 0; i <= amount; i++) {
            const uint16_t pointer = this->index + i;
            if (pointer < ramSize) {
                this->ram[pointer] = this->registers[i];
            }
        }
    }
}

void System::mem2reg(uint8_t amount) {
    //store RAM at location I+0..I+amount into registers R[0]..R[amount]
    if (amount < registersSize) {
        for (uint8_t i = 0; i <= amount; i++) {
            const uint16_t pointer = this->index + i;
            if (pointer < ramSize) {
                this->registers[i] = this->ram[pointer];
            }
        }
    }
}

void System::reg2backup(uint8_t amount) {
    //store registers R[0]..R[amount] into backup
    if (amount < registersSize) {
        for (uint8_t i = 0; i <= amount; i++) {
            this->registersBackup[i] = this->registers[i];
        }
    }
}

void System::backup2reg(uint8_t amount) {
    //restore registers R[0]..R[amount] from backup
    if (amount < registersSize) {
        for (uint8_t i = 0; i <= amount; i++) {
            this->registers[i] = this->registersBackup[i];
        }
    }
}

void System::nop(uint8_t opcode) {
    Serial.print("Opcode ");
    Serial.print(opcode, HEX);
    Serial.print(" not implemented!\n");
}

void System::log(const uint8_t opcode) const {
    char buffer[5];
    sprintf(buffer, "%04X", opcode);
    Serial.print(buffer);
    Serial.print(' ');

    for (int i = 0; i < registersSize; i++) {
        sprintf(buffer, "%02X", registers[i]);
        Serial.print(buffer);
        Serial.print(' ');
    }

    sprintf(buffer, "%04X", programCounter);
    Serial.print(buffer);
    Serial.print(' ');

    sprintf(buffer, "%04X", index);
    Serial.print(buffer);
    Serial.print('\n');
}

bool System::wasPixelChanged(const uint8_t x, const uint8_t y) const {
    return this->display.getPixel(x, y) != this->oldDisplay.getPixel(x, y);
}

bool System::getPixel(const uint8_t x, const uint8_t y) const {
    return this->display.getPixel(x, y);
}

void System::vblank() {
    this->oldDisplay.copyFrom(&this->display);

    for (uint32_t i = 0; i < 10000; i++) {
        this->cpuTick();
    }

    if (this->delayTimer > 0) {
        this->delayTimer--;
    }

    if (this->soundTimer > 0) {
        this->soundTimer--;
    }
}

void System::notifyOnKeyDown(const uint8_t key) {
    this->isKeyPressed = true;
    this->pressedKey = key;
}

void System::notifyOnKeyUp(const uint8_t key) {
    this->isKeyPressed = false;
    this->pressedKey = key;
}
