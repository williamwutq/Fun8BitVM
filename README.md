# Fun8BitVM

Fun8BitVM is a minimalistic 8-bit stack-based virtual machine with support for indirect addressing, pointer stacks, and a compact instruction set ideal for fun.  

With its current limitation of 2^16 instructions per run, it will in no way cause any damage in any form to the host machine.  

> [!Important]
> This is only for fun. If you enjoy it, I will really appriciate it!  

## Author
William Wu  

## Contribution
This is my accidental project uploaded to Github. No improvements will be made.  

## Version
Version 1.0.0  

## Features

- 8-bit wraparound arithmetic  
- Dual indirect pointer stacks for ROM and RAM  
- Compact and orthogonal instruction set  
- Supports conditional and indirect jumps  

## Architecture
Accumulator (A): A single 8-bit unsigned register (uint8) used for all arithmetic and logic operations.  

ROM (ROM[256]): Read-only memory holding program instructions.  

RAM (RAM[256]): General-purpose read/write memory.  

ROM Pointer Stack (ROM_PTR_STACK[16]): 16-entry stack used for indirect addressing into the ROM.  

RAM Pointer Stack (RAM_PTR_STACK[16]): 16-entry stack used for indirect addressing into the RAM.  

All values are 8-bit (uint8) and wrap around on overflow.  

## Instruction Set

- SYS  x  # System operation
  - SYS 0x0 # Exit with no code
  - SYS 0x1 # Exit with code A
  - SYS 0x2 # No operation
  - SYS 0x3 # A = 0
  - SYS 0x4 # A += 1
  - SYS 0x5 # A -= 1
  - SYS 0x6 # A += 4
  - SYS 0x7 # A -= 4
  - SYS 0x8 # A += 16
  - SYS 0x9 # A -= 16
  - SYS 0xA # print A
  - SYS 0xB # print PC
  - SYS 0xC # Jump down 16 instructions
  - SYS 0xD # Jump up 16 instructions
  - SYS 0xE # Jump to start
  - SYS 0xF # Jump to start if A == 0
- LDA  x  # A = RAM[*x]
- STA  x  # RAM[*x] = A
- ADD  x  # A += RAM[*x]
- SUB  x  # A -= RAM[*x]
- JMP  x  # Jump to ROM[*x]
- JZ   x  # Jump to ROM[*x] if A == 0
- SPT  x  # RAM *x = A
- GPT  x  # A = RAM *x
- PNT  x  # ROM *x = A
- RET  x  # A = ROM *x
- AND  x  # A &= RAM[*x]
- OR   x  # A |= RAM[*x]
- XOR  x  # A ^= RAM[*x]
- INC  x  # A += x
  - INC 0x0 # A += 16
- DEC  x  # A -= x
  - DEC 0x0 # A -= 16
