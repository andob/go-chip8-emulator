mod emulator;

use std::env;
use std::path::Path;
use crate::emulator::new_emulator;

fn main()
{
    let args = env::args().collect::<Vec<String>>();
    if args.len() <= 1
    {
        panic!("Syntax: chip8 <romfile>");
    }

    let rom_file_path = args[1].clone();
    if !Path::new(&rom_file_path).exists()
    {
        panic!("ROM file does not exist!");
    }

    let mut emulator = new_emulator(rom_file_path)
        .expect("Cannot create emulator!");

    emulator.start();
}
