Jesse Johnson
COP 3402, Fall 2021

Implements a compiler for a programming language called PL/0 in C.
Created the Parser, Lexical analyzer, and Virtual Machine components.

To execute code:

make
./a.out <file_name> <options>

Where 'make' is the code to compile multiple .c files. Compiles using Makefile file
Where <file_name> is the file name, without <>, that contains the instruction.
<options> is the flags available for this program:
    -l (print the list and table of lexemes/tokens)
    -s (print the symbol table)
    -a (print the generated assembly code)
    -v (print virtual machine execution trace) 

To compare output with another file disregarding spacing:
diff -w -B <output.txt> <compare_file.txt>

To bash script to test program:
bash <test_file.sh>
