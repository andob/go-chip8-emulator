mod system;
mod frontend;

use anyhow::{Context, Result};
use std::{fs, thread};
use std::fs::File;
use std::io::Read;
use std::sync::mpsc::{channel, Receiver, Sender};
use std::time::Instant;
use system::font::get_default_font;
use crate::emulator::frontend::{Frontend, FrontendChannels};
use crate::emulator::system::{cpu_tick, System};
use crate::emulator::system::constants::{DISPLAY_HEIGHT, DISPLAY_WIDTH, RAM_LENGTH, REGISTERS_LENGTH, TIMER_LAG};
use crate::file_and_line;

pub fn new_emulator(rom_file_path : String) -> Result<System>
{
    let mut chip8 = System
    {
        ram: [0; RAM_LENGTH],
        registers: [0; REGISTERS_LENGTH],
        registers_backup: [0; REGISTERS_LENGTH],
        program_counter: 0,
        index: 0,
        stack: Vec::new(),
        delay_timer: 0,
        sound_timer: 0,
        display: [[false; DISPLAY_HEIGHT]; DISPLAY_WIDTH],
        font: get_default_font(),
        pressed_key: None,
    };

    //load font into ram
    for character in chip8.font.chars.iter()
    {
        for (index, value) in character.data.iter().enumerate()
        {
            chip8.ram[(character.address+(index as u16)) as usize] = *value;
        }
    }

    //load ROM into RAM
    chip8.program_counter = 0x200; //load ROM into RAM index 200

    let mut rom_file = File::open(&rom_file_path).context(file_and_line!())?;
    let rom_file_metadata = fs::metadata(&rom_file_path).context(file_and_line!())?;
    let mut rom_file_bytes = vec![0; rom_file_metadata.len() as usize];
    rom_file.read(&mut rom_file_bytes).context(file_and_line!())?;
    for (index, value) in rom_file_bytes.iter().enumerate()
    {
        chip8.ram[(chip8.program_counter+index as u16) as usize] = *value;
    }

    return Ok(chip8);
}

struct BackendChannels
{
    display_sender : Sender<[[bool; 32]; 64]>,
    pressed_key_receiver : Receiver<Option<u8>>,
    quit_signal_receiver : Receiver<()>,
}

impl System
{
    pub fn start(&mut self)
    {
        let mut frontend = Frontend::new();
        let (frontend_channels, backend_channels) = self.create_channels();

        thread::scope(|thread_scope|
        {
            thread_scope.spawn(|| self.run_backend(backend_channels));
            thread_scope.spawn(|| frontend.run(frontend_channels));
        });
    }

    fn create_channels(&self) -> (FrontendChannels, BackendChannels)
    {
        let (display_sender, display_receiver) = channel::<[[bool; DISPLAY_HEIGHT]; DISPLAY_WIDTH]>();
        let (pressed_key_sender, pressed_key_receiver) = channel::<Option<u8>>();
        let (quit_signal_sender, quit_signal_receiver) = channel::<()>();

        let frontend_channels = FrontendChannels { display_receiver, pressed_key_sender, quit_signal_sender };
        let backend_channels = BackendChannels { display_sender, pressed_key_receiver, quit_signal_receiver };

        return (frontend_channels, backend_channels);
    }

    fn run_backend(&mut self, channels : BackendChannels)
    {
        let mut elapsed_time_ns = 0;
        loop
        {
            let start_instant = Instant::now();

            if let Ok(_) = channels.quit_signal_receiver.try_recv() { return; }

            if let Ok(pressed_key) = channels.pressed_key_receiver.try_recv() { self.pressed_key = pressed_key; }

            cpu_tick(self);

            if let Ok(_) = channels.display_sender.send(self.display) {}

            elapsed_time_ns += start_instant.elapsed().as_nanos();
            if elapsed_time_ns >= TIMER_LAG
            {
                if self.sound_timer > 0 { self.sound_timer -= 1; }
                if self.delay_timer > 0 { self.delay_timer -= 1; }
                elapsed_time_ns -= TIMER_LAG;
            }
        }
    }
}
