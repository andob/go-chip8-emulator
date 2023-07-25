package chip8

import "runtime"

func GetStackTraceAsString() string {
	b := make([]byte, 2048)
	n := runtime.Stack(b, false)
	s := string(b[:n])
	return s
}
