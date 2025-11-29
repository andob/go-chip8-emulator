#ifndef STACK_H
#define STACK_H
#include "constants.h"

class Stack {
    uint16_t data[stackSize];
    uint8_t pointer;

public:
    Stack() = default;

    bool isEmpty() const;
    void push(uint16_t value);
    uint16_t pop();
};

#endif
