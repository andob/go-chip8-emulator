package chip8

type Stack []uint16

func (stack *Stack) push(value uint16) {
	*stack = append(*stack, value)
}

func (stack *Stack) pop() uint16 {
	if len(*stack) == 0 {
		return 0
	}
	last := (*stack)[len(*stack)-1]
	*stack = (*stack)[:len(*stack)-1]
	return last
}
