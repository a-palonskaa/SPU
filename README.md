# SPU (software processing unit)

This repository contains an implementation of a simple processor simulation, including an assembler and a disassembler.
The processor can execute a set of predefined commands on a stack-based architecture, allowing for mathematical operations and control flow(conditional jumps and function calls/returns).

## Assembler

Convertes a higher-level representation of code into bytecode for the processor. Assembles in 2 passes.

## Disasm

Translates bytecode back into a human-readable format.

# Commands list

- Registers names: `ax`, `bx`, `cx`, `dx`, `ex`, `fx`, `gx`, `hx`

### Common

|Name  |Description|
|:----:|:----------|
|`hlt` | halt - end of program
|`push`| push value to stack
|`pop` | pop value from stack
|`out` | pop value from stack and print to ostream
|`dmp` | dump info about current state of processor

### Jumps

You need to specify label in code or address.

|Name  |                            Description                                     |
|:----:|:---------------------------------------------------------------------------|
|`jmp` | jump to specified address of executable file
|`ja`  | jump if `>`
|`jae` | jump if `>=`
|`jb`  | jump if `<`
|`jbe` | jump if `<=`
|`je`  | jump if `==`
|`jne` | jump if `!=`
|`call`| calls method (pushes to stack address of the next command for returning)
|`ret` | pops return address from stack and jumps there (end of method)

`ja` , `jae` , `jb` , `jbe` , `je` , `jne` pops two elements from stack and compares the last popped with the first one.

#### Syntax example

```
jmp meow:
...
meow:
```

```
call func:

func:
...
ret
```

### Maths

These functions pop 1 or 2 elements, make calculations and push result to stack

|Name  |   Description   |
|:----:|:----------------|
|`add` | `+` operator
|`sub` | `-` operator
|`mul` | `*` operator
|`div` | `/` operator
|`sqrt`| 'sqrt' operator
|`sqr` | find square
|`sin` | 'sin' operator
|`cos` | 'cos' operator

### Syntax example
```
push ax          ; push value from rbx register to stack
push 9.87        ; push value 9.87 to stack
jmp label:       ; jump to label named "label"
add              ; pop two elements from stack and push their sum
out;             ; pop element from stack and print
```

## Command line flags

```-i <file name>``` | ```--input_file <file name>```- get code from a file with given name

```-o <file name>``` | ```--output_file <file name>```- print results to a file with given name

```-l <file name>``` | ```--logger_output <file name>```- print logs to a file with given name

```-h``` | ```--help```- print help message with all commands description

## Usage

### Build

`make asm`, `make proc`, `make disasm` - build separately


