#!/bin/bash

# compile
make
compiled=$?

# check if code compiles
if [[ $compiled != 0 ]]; then
    echo "does not compile"
    exit
fi

for i in {1..19}; do
    ./a.out error$i.txt > output.txt
    diff -wiB outerror$i.txt output.txt
    correct=$?
    if [[ $correct == 0 ]]; then
        echo "Error $i: PASS"
    fi
done

# symbol table no proc
./a.out sym1.txt -a -s -l -v > output.txt
diff -wiB outsym1.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "Symbol Table 1: PASS"
fi

# symbol table with proc
./a.out sym2.txt -a -s -l -v > output.txt
diff -wiB outsym2.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "Symbol Table 2: PASS"
fi

# while 
./a.out while.txt -a -s -l -v > output.txt
diff -wiB outwhile.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "While: PASS"
fi

# when
./a.out when.txt -a -s -l -v > output.txt
diff -wiB outwhen.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "When: PASS"
fi

# read/write
./a.out rw.txt -a -s -l -v <<< "3 4" > output.txt
diff -wiB outrw.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "Read/Write: PASS"
fi

# arithmetic
./a.out arithmetic.txt -a -s -l -v > output.txt
diff -wiB outarithmetic.txt output.txt 
correct=$?
if [[ $correct == 0 ]]; then
    echo "Arithmetic: PASS"
fi

# procedures
./a.out proc.txt -a -s -l -v <<< "12345 4 5" > output.txt
diff -wiB outproc.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Procedures: PASS"
fi

# test1
./a.out test1.txt -a -s -l -v <<< "5" > output.txt
diff -wiB outtest1.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Test 1: PASS"
fi

# test2
./a.out test2.txt -a -s -l -v > output.txt
diff -wiB outtest2.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Test 2: PASS"
fi

# test3
./a.out test3.txt -a -s -l -v > output.txt
diff -wiB outtest3.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Test 3: PASS"
fi

# basic
./a.out basic.txt -a -s -l -v > output.txt
diff -wiB outbasic.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Basic: PASS"
fi

# tip
./a.out tip.txt -a -s -l -v <<< "1 10 51 17 2 10 51 17 0" > output.txt
diff -wiB outtip.txt output.txt
correct=$?
if [[ $correct == 0 ]]; then
    echo "Tip: PASS"
fi