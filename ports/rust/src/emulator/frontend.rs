use std::collections::HashMap;
use std::io::ErrorKind;
use std::sync::mpsc::{Receiver, Sender};
use maplit2::hashmap;
use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use sdl2::render::WindowCanvas;
use sdl2::Sdl;
use crate::emulator::system::constants::{DISPLAY_HEIGHT, DISPLAY_WIDTH};

pub struct Frontend
{
    block_width : u32,
    block_height : u32,
    window_width : u32,
    window_height : u32,
    keyboard_mapping : HashMap<Keycode, u8>,
    pressed_keys : Vec<Keycode>,
}

pub struct FrontendChannels
{
    pub display_receiver : Receiver<[[bool; 32]; 64]>,
    pub pressed_key_sender : Sender<Option<u8>>,
    pub quit_signal_sender : Sender<()>,
}

struct ParseKeysResult
{
    should_exit_frontend: bool
}

impl Frontend
{
    pub fn new() -> Frontend
    {
        let block_width = 15;
        let block_height = 15;
        let window_width = block_width * (DISPLAY_WIDTH as u32);
        let window_height = block_height * (DISPLAY_HEIGHT as u32);

        let keyboard_mapping = hashmap!
        {
            Keycode::Kp1   => 0x01,
            Keycode::Kp2   => 0x02,
            Keycode::Up    => 0x02,
            Keycode::Kp3   => 0x03,
            Keycode::Kp4   => 0x0C,
            Keycode::Q     => 0x04,
            Keycode::Left  => 0x04,
            Keycode::W     => 0x05,
            Keycode::Space => 0x05,
            Keycode::E     => 0x06,
            Keycode::Right => 0x06,
            Keycode::R     => 0x0D,
            Keycode::A     => 0x07,
            Keycode::S     => 0x08,
            Keycode::Down  => 0x08,
            Keycode::D     => 0x09,
            Keycode::F     => 0x0E,
            Keycode::Z     => 0x0A,
            Keycode::X     => 0x00,
            Keycode::C     => 0x0B,
            Keycode::V     => 0x0F,
        };

        return Frontend
        {
            block_width: block_width,
            block_height: block_height,
            window_width: window_width,
            window_height: window_height,
            keyboard_mapping: keyboard_mapping,
            pressed_keys: Vec::new(),
        };
    }

    pub fn run(&mut self, channels : FrontendChannels)
    {
        let (driver_index, _driver) = sdl2::render::drivers().enumerate()
            .find(|(_index, driver)| driver.name=="opengl")
            .ok_or(std::io::Error::new(ErrorKind::NotFound, "OpenGL driver not found!")).unwrap();

        let sdl_context = sdl2::init().unwrap();
        let window = sdl_context.video().unwrap()
            .window("CHIP-8", self.window_width, self.window_height).opengl().build().unwrap();
        let mut canvas = window.into_canvas().index(driver_index as u32).build().unwrap();

        loop
        {
            let parse_keys_result = self.parse_keys(&channels, &sdl_context);
            if parse_keys_result.should_exit_frontend { return; }

            self.render(&channels, &mut canvas);
        }
    }

    fn render(&self, channels : &FrontendChannels, canvas : &mut WindowCanvas)
    {
        if let Ok(display) = channels.display_receiver.recv()
        {
            canvas.set_draw_color(Color::BLACK);
            canvas.clear();
            canvas.set_draw_color(Color::YELLOW);

            for x in 0..DISPLAY_WIDTH
            {
                for y in 0..DISPLAY_HEIGHT
                {
                    if display[x][y]
                    {
                        if let Err(error) = canvas.fill_rect(Rect::new(
                            /*x*/ (x as i32)*(self.block_width as i32),
                            /*y*/ (y as i32)*(self.block_height as i32),
                            /*width*/ self.block_width,
                            /*height*/ self.block_height
                        )) { println!("{}", error); };
                    }
                }
            }

            canvas.present();
        }
    }

    fn parse_keys(&mut self, channels : &FrontendChannels, sdl_context : &Sdl) -> ParseKeysResult
    {
        let mut event_pump = sdl_context.event_pump().unwrap();
        for event in event_pump.poll_iter()
        {
            match event
            {
                Event::Quit { .. } | Event::KeyDown { keycode: Some(Keycode::Escape), .. } =>
                {
                    if let Ok(_) = channels.quit_signal_sender.send(()) {}
                    return ParseKeysResult { should_exit_frontend: true };
                }
                Event::KeyDown { keycode: Some(keycode), .. } =>
                {
                    self.pressed_keys.push(keycode);
                }
                Event::KeyUp { keycode : Some(keycode), .. } =>
                {
                    if let Some(index) = self.pressed_keys.iter().position(|pressed_key| *pressed_key==keycode)
                    {
                        self.pressed_keys.remove(index);
                    }
                }
                _ =>
                {
                    //some other kind of event, do not parse it!
                    return ParseKeysResult { should_exit_frontend: false };
                }
            }
        }

        if self.pressed_keys.is_empty()
        {
            if let Ok(_) = channels.pressed_key_sender.send(None) {}
        }
        else
        {
            if let Some(chip8_key) = self.keyboard_mapping.get(&self.pressed_keys[0])
            {
                if let Ok(_) = channels.pressed_key_sender.send(Some(*chip8_key)) {}
            }
        }

        return ParseKeysResult { should_exit_frontend: false };
    }
}
