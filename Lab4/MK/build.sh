#!/bin/bash

cd src
# build kernel
make 1>make_stdout.txt 2>make_stderr.txt
# disassemble kernel
objdump -d kernel > kernel.asm.txt
cd ..
