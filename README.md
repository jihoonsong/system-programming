# SIC/XE Machine

This is a course project of System Programming lecture at Sogang University.

This SIC/XE machine can do assembly, linking and loading, and execute the loaded program.

## Execution

Use makefile to build. The execution file is 20131567.out - my student id.
```sh
make
./20131567.out
```

## Usage

Supported commands are:

0. Print all supported commands.
```
help
```

1. Print all files in current diretory. Similar to 'ls -F'.
```
dir
```

2. Quit program.
```
quit
```

3. Prints all executed command so far.
```
history
```

4. Dump memory.
```
dump        // Dump 160 bytes start from last dumped address.
dump 32F    // Dump 160 bytes start from 0x32F.
dump 25, 3F // Dump memory from 0x25 to 3F.
```

5. Set memory.
```
edit 15, 3A // Set memory at 0x15 to 3A.
```

6. Set memory in range.
```
fill 2A, 3A, 4A // Set memory between 2A and 3A to 4A.
```

7. Clear memory.
```
reset
```

8. Get opcode of mnemonic.
```
opcode ADD // Print opcode corresponding to mnemonic ADD.
```

9. Print opcode table.
```
opcodelist
```

10. Print content of file. (Same as cat command in Linux)
```
type README.md // Show content of 'README.md'.
```

11. Assemble .asm file.
```
assemble copy.asm // Assemble 'copy.asm' and produce 'copy.lst' and 'copy.obj'.
```

12. Print symbol table used in the last success assembly.
```
symbol
```

13. Set the starting address where a program will be loaded.
```
progaddr 4000 // A program will be loaded on 0x4000.
```

14. Load .obj files on memory.
```
loader proga.obj progb.obj progc.obj
```

15. Set breakpoints.
```
bp 4036 // Set breakpoint at 0x4036.
```

16. Clear all breakpoints.
```
bp clear
```

17. Show all breakpoints.
```
bp
```

18. Run the last loaded program.
```
run // Execute the program until PC reaches any breakpoint.
```

## Built With

* Ubuntu 16.04.6 LTS
* gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.12)
* C11

## License

This project is licensed under the MIT License.
See [LICENSE.md](LICENSE.md) for details.
