#include "system.h"
#include <cstdlib>
#include <iostream>

System::System(const vector<u8>& rom_bytes) : font(Font::get_default())
{
    //initialise fields
    this->ram.fill(0);
    this->registers.fill(0);
    this->registers_backup.fill(0);
    this->program_counter = this->index = 0;
    this->delay_timer = this->sound_timer = 0;
    this->is_key_pressed = this->pressed_key = 0;

    //load font into RAM
    for (FontCharacter character : this->font.get_characters())
    {
        auto character_data = character.get_data();
        for (u16 index = 0; index < character_data.size(); index++)
        {
            const u16 address = character.get_address() + index;
            this->ram[address % RAM_SIZE] = character_data[index];
            index++;
        }
    }

    //load ROM into RAM
    this->program_counter = 0x200; //load ROM into RAM index 200

    for (u16 index = 0; index < rom_bytes.size(); index++)
    {
        this->ram[this->program_counter + index] = rom_bytes[index];
    }

    //clear display
    this->cls();
}

u16 System::next_opcode()
{
    if (this->program_counter+1 < RAM_SIZE)
    {
        const u16 first = this->ram[this->program_counter];
        const u16 second = this->ram[this->program_counter+1];
        this->program_counter += 2;
        return first << 8 | second;
    }

    return 0;
}

void System::cpu_tick()
{
    //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    const u16 opcode = this->next_opcode();

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
    else if (opcode) cout << "Opcode not implemented " << hex << opcode << "!\n";
}

void System::cls()
{
    for (u8 i = 0; i < this->display.size(); i++)
    {
        for (u8 j = 0; j < this->display[i].size(); j++)
        {
            this->display[i][j] = false;
        }
    }
}

void System::ret()
{
    //return from subroutine
    if (!this->_stack.empty())
    {
        this->program_counter = this->_stack.top();
        this->_stack.pop();
    }
}

void System::jp1(const u16 pointer)
{
    //jump to address (goto)
    this->program_counter = pointer;
}

void System::call(const u16 pointer)
{
    //call subroutine
    this->_stack.push(this->program_counter);
    this->program_counter = pointer;
}

void System::se1(u8 i, u8 value)
{
    //skip next instruction if register == value
    if (i < REGISTERS_SIZE && this->registers[i] == value)
    {
        this->next_opcode();
    }
}

void System::sne1(u8 i, u8 value)
{
    //skip next instruction if register != value
    if (i < REGISTERS_SIZE && this->registers[i] != value)
    {
        this->next_opcode();
    }
}

void System::se2(u8 i, u8 j)
{
    //skip next instruction if register1 == register2
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        if (this->registers[i] == this->registers[j])
        {
            this->next_opcode();
        }
    }
}

void System::ld1(u8 i, u8 value)
{
    //load value into register
    if (i < REGISTERS_SIZE)
    {
        this->registers[i] = value;
    }
}

void System::add1(u8 i, u8 value)
{
    //add: Ri = Ri + value
    if (i < REGISTERS_SIZE)
    {
        const u16 result = static_cast<u16>(this->registers[i]) + value;

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result > 0xFF ? 1 : 0;
    }
}

void System::ld2(u8 i, u8 j)
{
    //store the value of register2 into register1
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        this->registers[i] = this->registers[j];
    }
}

void System::_or(u8 i, u8 j)
{
    //bitwise or: Ri = Ri | Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        this->registers[i] |= this->registers[j];
    }
}

void System::_and(u8 i, u8 j)
{
    //bitwise and: Ri = Ri & Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        this->registers[i] &= this->registers[j];
    }
}

void System::_xor(u8 i, u8 j)
{
    //bitwise xor: Ri = Ri ^ Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        this->registers[i] ^= this->registers[j];
    }
}

void System::add2(u8 i, u8 j)
{
    //add: Ri = Ri + Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        const u16 result = static_cast<u16>(this->registers[i]) + this->registers[j];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result > 0xFF ? 1 : 0;
    }
}

void System::sub(u8 i, u8 j)
{
    //substract: Ri = Ri - Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        const i16 result = static_cast<i16>(this->registers[i]) - this->registers[j];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result < 0 ? 0 : 1;
    }
}

void System::subn(u8 i, u8 j)
{
    //substract: Ri = Rj - Ri
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        const i16 result = static_cast<i16>(this->registers[j]) - this->registers[i];

        this->registers[i] = result & 0xFF;
        this->registers[0x0F] = result < 0 ? 0 : 1;
    }
}

void System::shr(u8 i)
{
    //shift right: Ri = Ri >> 1
    if (i < REGISTERS_SIZE)
    {
        this->registers[0x0F] = this->registers[i] & 0b00000001;
        this->registers[i] >>= 1;
    }
}

void System::shl(u8 i)
{
    //shift left: Ri = Ri << 1
    if (i < REGISTERS_SIZE)
    {
        this->registers[0x0F] = (this->registers[i] & 0b10000000) >> 7;
        this->registers[i] <<= 1;
    }
}

void System::sne2(u8 i, u8 j)
{
    //skip next instruction if Ri != Rj
    if (i < REGISTERS_SIZE && j < REGISTERS_SIZE)
    {
        if (this->registers[i] != this->registers[j])
        {
            this->next_opcode();
        }
    }
}

void System::ld3(u16 value)
{
    //store value into index register
    this->index = value;
}

void System::jp2(u16 pointer)
{
    //jump to address (goto) R0+pointer
    this->program_counter = static_cast<u16>(this->registers[0]) + pointer;
}

void System::rnd(u8 i, u8 value)
{
    //generate random number: Rx = random & value
    const u8 result = (rand() % 0x100) & value;
    this->registers[i] = result;
}

void System::draw(u8 ix, u8 iy, u8 height)
{
    if (ix < REGISTERS_SIZE && iy < REGISTERS_SIZE)
    {
        this->registers[0x0F] = 0x00;

        const i32 x = static_cast<i32>(this->registers[ix]) % DISPLAY_WIDTH;
        const i32 y = static_cast<i32>(this->registers[iy]) % DISPLAY_HEIGHT;

        for (u32 row = 0; row < height; row++)
        {
            const i32 ny = (y + row) % DISPLAY_HEIGHT;
            const i32 index = (static_cast<i32>(this->index) + row) % RAM_SIZE;
            const i32 sprite_octet = this->ram[index];

            for (i8 bit_index = 7; bit_index >= 0; bit_index--)
            {
                const i32 nx = (x + (7 - bit_index)) % DISPLAY_WIDTH;
                const i32 bit = ((sprite_octet & (0x00000001 << bit_index)) >> bit_index) == 1;
                const bool old_value = this->display[nx][ny];
                const bool new_value = bit != old_value;
                this->display[nx][ny] = new_value;

                //pixel turned off
                if (old_value && !new_value)
                    this->registers[0x0F] = 0x01;
            }
        }
    }
}

void System::skp(u8 i)
{
    //skip next instruction if key i is pressed
    if (i < REGISTERS_SIZE)
    {
        const u8 expected_key = this->registers[i];
        if (this->is_key_pressed && this->pressed_key == expected_key)
        {
            this->next_opcode();
        }
    }
}

void System::sknp(u8 i)
{
    //skip next instruction if key i is not pressed
    if (i < REGISTERS_SIZE)
    {
        const u8 expected_key = this->registers[i];
        if (this->is_key_pressed && this->pressed_key != expected_key)
        {
            this->next_opcode();
        }
        else if (!this->is_key_pressed)
        {
            this->next_opcode();
        }
    }
}

void System::ld4(u8 i)
{
    //set Ri = delay timer
    if (i < REGISTERS_SIZE)
    {
        this->registers[i] = this->delay_timer;
    }
}

void System::ldk(u8 i)
{
    //block until a key is pressed, and store the key into Ri
    if (i < REGISTERS_SIZE)
    {
        while (!this->is_key_pressed) {}

        this->registers[i] = this->pressed_key;
    }
}

void System::lddt(u8 i)
{
    //set delay timer = Ri
    if (i < REGISTERS_SIZE)
    {
        this->delay_timer = this->registers[i];
    }
}

void System::ldst(u8 i)
{
    //set sound timer = Ri
    if (i < REGISTERS_SIZE)
    {
        this->sound_timer = this->registers[i];
    }
}

void System::add3(u8 i)
{
    //set I = I + Ri
    if (i < REGISTERS_SIZE)
    {
        const u32 result = static_cast<u32>(this->registers[i]) + this->index;

        this->index = result & 0xFFFF;
        this->registers[0x0F] = result > 0x00FF ? 1 : 0;
    }
}

void System::ldf(u8 i)
{
    //location of sprite of digit from font
    if (i < REGISTERS_SIZE)
    {
        const u8 char_index = this->registers[i];
        if (char_index < NOF_CHARS_IN_FONT)
        {
            this->index = this->font.get_characters()[char_index].get_address();
        }
    }
}

void System::ldb(u8 i)
{
    //store binary coded decimal representation of Ri in RAM at I,I+1,I+2
    if (i < REGISTERS_SIZE && this->index+2 < RAM_SIZE)
    {
        const u8 value = this->registers[i];
        this->ram[this->index+0] = (value / 100) % 10;
        this->ram[this->index+1] = (value / 10) % 10;
        this->ram[this->index+2] = value % 10;
    }
}

void System::reg2mem(u8 amount)
{
    //store registers R0..R_amount into RAM at location I+0..I+amount
    if (amount < REGISTERS_SIZE)
    {
        for (u8 i = 0; i <= amount; i++)
        {
            const u16 pointer = this->index + i;
            if (pointer < RAM_SIZE)
            {
                this->ram[pointer] = this->registers[i];
            }
        }
    }
}

void System::mem2reg(u8 amount)
{
    //store RAM at location I+0..I+amount into registers R0..R_amount
    if (amount < REGISTERS_SIZE)
    {
        for (u8 i = 0; i <= amount; i++)
        {
            const u16 pointer = this->index + i;
            if (pointer < RAM_SIZE)
            {
                this->registers[i] = this->ram[pointer];
            }
        }
    }
}

void System::reg2backup(u8 amount)
{
    //store registers R0..R_amount into backup
    if (amount < REGISTERS_SIZE)
    {
        for (u8 i = 0; i <= amount; i++)
        {
            this->registers_backup[i] = this->registers[i];
        }
    }
}

void System::backup2reg(u8 amount)
{
    //restore registers R0..R_amount from backup
    if (amount < REGISTERS_SIZE)
    {
        for (u8 i = 0; i <= amount; i++)
        {
            this->registers[i] = this->registers_backup[i];
        }
    }
}

void System::iterate_display(const function<void(u8, u8, bool)>& callback) const
{
    for (u8 x = 0; x < DISPLAY_WIDTH; x++)
    {
        for (u8 y = 0; y < DISPLAY_HEIGHT; y++)
        {
            callback(x, y, display[x][y]);
        }
    }
}

void System::vblank()
{
    for (u32 i = 0; i < 10000; i++)
    {
        this->cpu_tick();
    }

    if (this->delay_timer > 0)
    {
        this->delay_timer--;
    }

    if (this->sound_timer > 0)
    {
        this->sound_timer--;
    }
}

void System::notify_on_key_down(const u8 key)
{
    this->is_key_pressed = true;
    this->pressed_key = key;
}

void System::notify_on_key_up(const u8 key)
{
    this->is_key_pressed = false;
    this->pressed_key = key;
}
