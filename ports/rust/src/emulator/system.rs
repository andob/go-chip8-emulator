pub mod font;
pub mod constants;

use std::thread;
use std::time::Duration;
use rand::Rng;
use crate::emulator::system::constants::{CPU_LAG, DISPLAY_HEIGHT, DISPLAY_WIDTH, RAM_LENGTH, REGISTERS_LENGTH};
use crate::emulator::system::font::Font;

pub struct System
{
    pub ram : [u8; RAM_LENGTH],
    pub registers : [u8; REGISTERS_LENGTH],
    pub registers_backup : [u8; REGISTERS_LENGTH],
    pub program_counter : u16,
    pub index : u16,
    pub stack : Vec<u16>,
    pub delay_timer : u8,
    pub sound_timer : u8,
    pub display : [[bool; DISPLAY_HEIGHT]; DISPLAY_WIDTH],
    pub font : Font,
    pub pressed_key : Option<u8>,
}

fn next_opcode(chip8: &mut System) -> u16
{
    if ((chip8.program_counter+1) as usize) < chip8.ram.len()
    {
        let first = chip8.ram[chip8.program_counter as usize];
        let second = chip8.ram[(chip8.program_counter+1) as usize];
        chip8.program_counter += 2;
        return ((first as u16) << 8) | (second as u16);
    }

    return 0
}

pub fn cpu_tick(chip8 : &mut System)
{
    //http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
    let opcode = next_opcode(chip8);
    if opcode == 0x00E0
    {
        cls(chip8);
    }
    else if opcode == 0x00EE
    {
        ret(chip8);
    }
    else if opcode>>0xC == 0x1
    {
        jp1(chip8, opcode&0x0FFF);
    }
    else if opcode>>0xC == 0x2
    {
        call(chip8, opcode&0x0FFF);
    }
    else if opcode>>0xC == 0x3
    {
        se1(chip8, ((opcode>>8)&0x0F) as u8, (opcode&0xFF) as u8);
    }
    else if opcode>>0xC == 0x4
    {
        sne1(chip8, ((opcode>>8)&0x0F) as u8, (opcode&0xFF) as u8);
    }
    else if opcode>>0xC == 0x5 && opcode&0x000F == 0
    {
        se2(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x6
    {
        ld1(chip8, ((opcode>>8)&0x0F) as u8, (opcode&0xFF) as u8);
    }
    else if opcode>>0xC == 0x7
    {
        add1(chip8, ((opcode>>8)&0x0F) as u8, (opcode&0xFF) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x0
    {
        ld2(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x1
    {
        or(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x2
    {
        and(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x3
    {
        xor(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x4
    {
        add2(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x5
    {
        sub(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x6
    {
        shr(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0x7
    {
        subn(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x8 && opcode&0x000F == 0xE
    {
        shl(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0x9 && opcode&0x000F == 0x0
    {
        sne2(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xA
    {
        ld3(chip8, opcode&0x0FFF);
    }
    else if opcode>>0xC == 0xB
    {
        jp2(chip8, opcode&0x0FFF);
    }
    else if opcode>>0xC == 0xC
    {
        rnd(chip8, ((opcode>>8)&0x0F) as u8, (opcode&0xFF) as u8);
    }
    else if opcode>>0xC == 0xD
    {
        draw(chip8, ((opcode>>8)&0x0F) as u8, ((opcode>>4)&0x0F) as u8, (opcode&0x0F) as u8);
    }
    else if opcode>>0xC == 0xE && opcode&0xFF == 0x9E
    {
        skp(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xE && opcode&0xFF == 0xA1
    {
        sknp(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x07
    {
        ld4(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x0A
    {
        ldk(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x15
    {
        lddt(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x18
    {
        ldst(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x1E
    {
        add3(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x29
    {
        ldf(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x33
    {
        ldb(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x55
    {
        reg2mem(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x65
    {
        mem2reg(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x75
    {
        reg2backup(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode>>0xC == 0xF && opcode&0xFF == 0x85
    {
        backup2reg(chip8, ((opcode>>8)&0x0F) as u8);
    }
    else if opcode != 0
    {
        println!("Opcode not implemented {}!", format!("{:X}", opcode));
    }

    thread::sleep(Duration::from_nanos(CPU_LAG));
}

fn cls(chip8 : &mut System)
{
    for i in 0..chip8.display.len()
    {
        for j in 0..chip8.display[i].len()
        {
            chip8.display[i][j] = false;
        }
    }
}

fn ret(chip8 : &mut System)
{
    //return from subroutine
    if let Some(pointer) = chip8.stack.pop()
    {
        chip8.program_counter = pointer;
    }
}

fn jp1(chip8 : &mut System, pointer : u16)
{
    //jump to address (goto)
    chip8.program_counter = pointer
}

fn call(chip8 : &mut System, pointer : u16)
{
    //call subroutine
    chip8.stack.push(chip8.program_counter);
    chip8.program_counter = pointer;
}

fn se1(chip8 : &mut System, i : u8, value : u8)
{
    //skip next instruction if register == value
    if i < chip8.registers.len() as u8 && chip8.registers[i as usize] == value
    {
        next_opcode(chip8);
    }
}

fn sne1(chip8 : &mut System, i : u8, value : u8)
{
    //skip next instruction if register != value
    if i < chip8.registers.len() as u8 && chip8.registers[i as usize] != value
    {
        next_opcode(chip8);
    }
}

fn se2(chip8 : &mut System, i : u8, j : u8)
{
    //skip next instruction if register1 == register2
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        if chip8.registers[i as usize] == chip8.registers[j as usize]
        {
            next_opcode(chip8);
        }
    }
}

fn ld1(chip8 : &mut System, i : u8, value : u8)
{
    //load value into register
    if i < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] = value;
    }
}

fn add1(chip8 : &mut System, i : u8, value : u8)
{
    //add: Ri = Ri + value
    if i < chip8.registers.len() as u8
    {
        let result = (chip8.registers[i as usize] as u16) + (value as u16);
        chip8.registers[i as usize] = (result & 0xFF) as u8;
    }
}

fn ld2(chip8 : &mut System, i : u8, j : u8)
{
    //store the value of register2 into register1
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] = chip8.registers[j as usize];
    }
}

fn or(chip8 : &mut System, i : u8, j : u8)
{
    //bitwise or: Ri = Ri | Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] |= chip8.registers[j as usize];
        chip8.registers[0x0F] = 0;
    }
}

fn and(chip8 : &mut System, i : u8, j : u8)
{
    //bitwise and: Ri = Ri & Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] &= chip8.registers[j as usize];
        chip8.registers[0x0F] = 0;
    }
}

fn xor(chip8 : &mut System, i : u8, j : u8)
{
    //bitwise xor: Ri = Ri ^ Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] ^= chip8.registers[j as usize];
        chip8.registers[0x0F] = 0;
    }
}

fn add2(chip8 : &mut System, i : u8, j : u8)
{
    //add: Ri = Ri + Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        let result = (chip8.registers[i as usize] as u16) + (chip8.registers[j as usize] as u16);
        chip8.registers[i as usize] = (result & 0xFF) as u8;
        if result > 0xFF { chip8.registers[0x0F] = 1; }
        else { chip8.registers[0x0F] = 0; }
    }
}

fn sub(chip8 : &mut System, i : u8, j : u8)
{
    //substract: Ri = Ri - Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        let result = (chip8.registers[i as usize] as i16) - (chip8.registers[j as usize] as i16);
        chip8.registers[i as usize] = (result & 0xFF) as u8;
        if result < 0 { chip8.registers[0x0F] = 0; }
        else { chip8.registers[0x0F] = 1; }
    }
}

fn subn(chip8 : &mut System, i : u8, j : u8)
{
    //substract: Ri = Rj - Ri
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        let result = (chip8.registers[j as usize] as i16) - (chip8.registers[i as usize] as i16);
        chip8.registers[i as usize] = (result & 0xFF) as u8;
        if result < 0 { chip8.registers[0x0F] = 0; }
        else { chip8.registers[0x0F] = 1; }
    }
}

fn shr(chip8 : &mut System, i : u8)
{
    //shift right: Ri = Ri >> 1
    if i < chip8.registers.len() as u8
    {
        let carry = chip8.registers[i as usize] & 0b00000001;
        chip8.registers[i as usize] >>= 1;
        chip8.registers[0x0F] = carry;
    }
}

fn shl(chip8 : &mut System, i : u8)
{
    //shift left: Ri = Ri << 1
    if i < chip8.registers.len() as u8
    {
        let carry = (chip8.registers[i as usize] & 0b10000000) >> 7;
        chip8.registers[i as usize] <<= 1;
        chip8.registers[0x0F] = carry;
    }
}

fn sne2(chip8 : &mut System, i : u8, j : u8)
{
    //skip next instruction if Ri != Rj
    if i < chip8.registers.len() as u8 && j < chip8.registers.len() as u8
    {
        if chip8.registers[i as usize] != chip8.registers[j as usize]
        {
            next_opcode(chip8);
        }
    }
}

fn ld3(chip8 : &mut System, value : u16)
{
    //store value into index register
    chip8.index = value;
}

fn jp2(chip8 : &mut System, pointer : u16)
{
    //jump to address (goto) R0+pointer
    chip8.program_counter = (chip8.registers[0] as u16) + pointer;
}

fn rnd(chip8 : &mut System, i : u8, value : u8)
{
    //generate random number: Rx = random & value
    let mut random = rand::thread_rng();
    chip8.registers[i as usize] = random.gen::<u8>() & value;
}

fn draw(chip8 : &mut System, ix : u8, iy : u8, height : u8)
{
    if ix < chip8.registers.len() as u8 && iy < chip8.registers.len() as u8
    {
        chip8.registers[0x0F] = 0x00;

        let display_width = chip8.display.len() as i32;
        let display_height = chip8.display[0].len() as i32;
        let x = (chip8.registers[ix as usize] as i32) % display_width;
        let y = (chip8.registers[iy as usize] as i32) % display_height;

        for row in 0..height
        {
            let ny = (y + (row as i32)) % display_height;
            let index = (((chip8.index as i32) + (row as i32)) % (chip8.ram.len() as i32)) as usize;
            let sprite_byte = chip8.ram[index] as i32;
            for bit_index in (0..=7).rev()
            {
                let nx = (x + (7 - bit_index)) % display_width;
                let bit = (sprite_byte&(0x00000001<<bit_index))>>bit_index == 1;
                let old_value = chip8.display[nx as usize][ny as usize];
                let new_value = bit != old_value;
                chip8.display[nx as usize][ny as usize] = new_value;
                if old_value && !new_value
                {
                    //pixel turned off
                    chip8.registers[0x0F] = 0x01;
                }
            }
        }
    }
}

fn skp(chip8 : &mut System, i : u8)
{
    //skip next instruction if key i is pressed
    if i < chip8.registers.len() as u8
    {
        let expected_key = chip8.registers[i as usize];
        if let Some(pressed_key) = chip8.pressed_key
        {
            if pressed_key == expected_key
            {
                next_opcode(chip8);
            }
        }
    }
}

fn sknp(chip8 : &mut System, i : u8)
{
    //skip next instruction if key i is not pressed
    if i < chip8.registers.len() as u8
    {
        let expected_key = chip8.registers[i as usize];
        if let Some(pressed_key) = chip8.pressed_key
        {
            if pressed_key != expected_key
            {
                next_opcode(chip8);
            }
        }
        else if chip8.pressed_key == None
        {
            next_opcode(chip8);
        }
    }
}

fn ld4(chip8 : &mut System, i : u8)
{
    //set Ri = delay timer
    if i < chip8.registers.len() as u8
    {
        chip8.registers[i as usize] = chip8.delay_timer;
    }
}

fn ldk(chip8 : &mut System, i : u8)
{
    //block until a key is pressed, and store the key into Ri
    if i < chip8.registers.len() as u8
    {
        while chip8.pressed_key == None
        {
            thread::sleep(Duration::from_nanos(CPU_LAG));
        }

        if let Some(pressed_key) = chip8.pressed_key
        {
            chip8.registers[i as usize] = pressed_key;
        }
    }
}

fn lddt(chip8 : &mut System, i : u8)
{
    //set delay timer = Ri
    if i < chip8.registers.len() as u8
    {
        chip8.delay_timer = chip8.registers[i as usize];
    }
}

fn ldst(chip8 : &mut System, i : u8)
{
    //set sound timer = Ri
    if i < chip8.registers.len() as u8
    {
        chip8.sound_timer = chip8.registers[i as usize];
    }
}

fn add3(chip8 : &mut System, i : u8)
{
    //set I = I + Ri
    if i < chip8.registers.len() as u8
    {
        let result = (chip8.registers[i as usize] as u32) + (chip8.index as u32);
        chip8.index = (result & 0xFFFF) as u16;
    }
}

fn ldf(chip8 : &mut System, i : u8)
{
    //location of sprite of digit from font
    if i < chip8.registers.len() as u8
    {
        let char_index = chip8.registers[i as usize];
        if char_index < chip8.font.chars.len() as u8
        {
            chip8.index = chip8.font.chars[char_index as usize].address;
        }
    }
}

fn ldb(chip8 : &mut System, i : u8)
{
    //store binary coded decimal representation of Ri in RAM at I,I+1,I+2
    if i < chip8.registers.len() as u8 && chip8.index+2 < chip8.ram.len() as u16
    {
        let value = chip8.registers[i as usize];
        chip8.ram[(chip8.index+0) as usize] = (value / 100) % 10;
        chip8.ram[(chip8.index+1) as usize] = (value / 10) % 10;
        chip8.ram[(chip8.index+2) as usize] = value % 10;
    }
}

fn reg2mem(chip8 : &mut System, amount : u8)
{
    //store registers R0..R_amount into RAM at location I+0..I+amount
    if amount < chip8.registers.len() as u8
    {
        for i in 0..=amount
        {
            if chip8.index < chip8.ram.len() as u16
            {
                chip8.ram[chip8.index as usize] = chip8.registers[i as usize];
            }

            chip8.index += 1;
        }
    }
}

fn mem2reg(chip8 : &mut System, amount : u8)
{
    //store RAM at location I+0..I+amount into registers R0..R_amount
    if amount < chip8.registers.len() as u8
    {
        for i in 0..=amount
        {
            if chip8.index < chip8.ram.len() as u16
            {
                chip8.registers[i as usize] = chip8.ram[chip8.index as usize];
            }

            chip8.index += 1;
        }
    }
}

fn reg2backup(chip8 : &mut System, amount : u8)
{
    //store registers R0..R_amount into backup
    if amount < chip8.registers.len() as u8
    {
        for i in 0..=amount
        {
            chip8.registers_backup[i as usize] = chip8.registers[i as usize];
        }
    }
}

fn backup2reg(chip8 : &mut System, amount : u8)
{
    //restore registers R0..R_amount from backup
    if amount < chip8.registers.len() as u8
    {
        for i in 0..=amount
        {
            chip8.registers[i as usize] = chip8.registers_backup[i as usize];
        }
    }
}
