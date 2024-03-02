
pub fn get_default_font() -> Font
{
    return Font
    {
        chars:
        [
            FontCharacter { name: 0x0, address: 0x050, data: [ 0xF0, 0x90, 0x90, 0x90, 0xF0 ] },
            FontCharacter { name: 0x1, address: 0x055, data: [ 0x20, 0x60, 0x20, 0x20, 0x70 ] },
            FontCharacter { name: 0x2, address: 0x05A, data: [ 0xF0, 0x10, 0xF0, 0x80, 0xF0 ] },
            FontCharacter { name: 0x3, address: 0x05F, data: [ 0xF0, 0x10, 0xF0, 0x10, 0xF0 ] },
            FontCharacter { name: 0x4, address: 0x064, data: [ 0x90, 0x90, 0xF0, 0x10, 0x10 ] },
            FontCharacter { name: 0x5, address: 0x069, data: [ 0xF0, 0x80, 0xF0, 0x10, 0xF0 ] },
            FontCharacter { name: 0x6, address: 0x06E, data: [ 0xF0, 0x80, 0xF0, 0x90, 0xF0 ] },
            FontCharacter { name: 0x7, address: 0x073, data: [ 0xF0, 0x10, 0x20, 0x40, 0x40 ] },
            FontCharacter { name: 0x8, address: 0x078, data: [ 0xF0, 0x90, 0xF0, 0x90, 0xF0 ] },
            FontCharacter { name: 0x9, address: 0x07D, data: [ 0xF0, 0x90, 0xF0, 0x10, 0xF0 ] },
            FontCharacter { name: 0xA, address: 0x082, data: [ 0xF0, 0x90, 0xF0, 0x90, 0x90 ] },
            FontCharacter { name: 0xB, address: 0x087, data: [ 0xE0, 0x90, 0xE0, 0x90, 0xE0 ] },
            FontCharacter { name: 0xC, address: 0x08C, data: [ 0xF0, 0x80, 0x80, 0x80, 0xF0 ] },
            FontCharacter { name: 0xD, address: 0x091, data: [ 0xE0, 0x90, 0x90, 0x90, 0xE0 ] },
            FontCharacter { name: 0xE, address: 0x096, data: [ 0xF0, 0x80, 0xF0, 0x80, 0xF0 ] },
            FontCharacter { name: 0xF, address: 0x09B, data: [ 0xF0, 0x80, 0xF0, 0x80, 0x80 ] },
        ],
    };
}

pub struct Font
{
    pub chars: [FontCharacter; 16]
}

pub struct FontCharacter
{
    pub name : u8,
    pub address : u16,
    pub data : [u8; 5],
}