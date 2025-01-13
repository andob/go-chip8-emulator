#include <fstream>
#include <iostream>
#include "frontend.h"
#include "system.h"

using namespace std;

int main(const int argc, char** argv)
{
    if (argc <= 1)
    {
        cout << "Syntax: chip8 <romfile>" << endl;
        return EXIT_FAILURE;
    }

    const char* rom_file_path = argv[argc-1];
    ifstream rom_file_stream(rom_file_path, ios_base::in | ios_base::binary);
    if (!rom_file_stream.good())
    {
        cout << "ROM file does not exist!" << endl;
        return EXIT_FAILURE;
    }

    rom_file_stream.seekg(0, ios_base::end);
    const size_t rom_file_length = rom_file_stream.tellg();
    rom_file_stream.seekg(0, ios_base::beg);

    vector<u8> rom_file_bytes;
    rom_file_bytes.reserve(rom_file_length);
    copy( istreambuf_iterator(rom_file_stream),
        istreambuf_iterator<char>(),
        back_inserter(rom_file_bytes));

    unique_ptr<System> system(new System(rom_file_bytes));
    const Frontend frontend(system);
    frontend.run();

    return EXIT_SUCCESS;
}
