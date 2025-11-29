#include <stdint.h>
#include "emulator/stack.h"

bool Stack::isEmpty() const {
    return pointer == 0;
}

void Stack::push(uint16_t value) {
    if (pointer+1 < stackSize) {
        data[pointer] = value;
        pointer++;
    }
}

uint16_t Stack::pop() {
    if (pointer > 0) {
        pointer--;
        return data[pointer];
    }

    return 0;
}
