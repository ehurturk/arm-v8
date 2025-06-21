# ARMv8 Emulator & Assembler & Extension

## Emulator

Files can be found in `src/emulator/*`

Emulates a *subset* of ARMv8 instructions, given an ARMv8 object file. Prints out the machine state (2MB of main memory) to standard output or to the given output file.

Can be used as:

```sh
./emulate [file_in] [file_out (OPTIONAL)]
```

If no file_out is provided, prints the machine state to `stdout`.

## Assembler

Files can be found in `src/assembler/*`

Assembles a given ARMv8 (subset) assembly file.

Can be used as:

```sh
./assemble [input] [output]
```

## Extension

Files can be found in `extension/*`

Visual & interactive chess engine using bare-metal Raspberry Pi 3B (BCM2837 with Quad Cortex A-53 CPU) and ILI9486 display and XPT2046 touchscreen ICs.

### Backend

Backend consists of the core chess engine, together with the neural network training architecture for chess position evaluations.

### Frontend

Frontend consists of a bootloader, a kernel main, display driver for the ILI9486 display IC, and touchscreen driver for the XPT2046 touchscreen IC. An ELEGOO 3.5 inch display was used to test the chess engine & display.

## Groupmates

This project was made possible with a group of 4 people:

- Emir Hurturk
- Andrei Lazarescu
- Charles Lin
- Aryan Jain
