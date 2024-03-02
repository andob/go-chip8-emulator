
pub const RAM_LENGTH : usize = 4*1024;

pub const REGISTERS_LENGTH : usize = 16;

pub const DISPLAY_WIDTH : usize = 64;

pub const DISPLAY_HEIGHT : usize = 32;

pub const CPU_LAG : u64 = 1500000/4; //nanoseconds

pub const TIMER_LAG : u128 = 16666667; //60Hz in nanoseconds

#[macro_export]
macro_rules! file_and_line
{
    () =>
    {
        format!("{}:{}", file!(), line!())
    };
}
