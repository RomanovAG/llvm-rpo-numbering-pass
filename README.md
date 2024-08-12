**1:**
```
$ export LLVM_DIR=<path to llvm-project/build>
$ cd llvm-rpo-numbering-pass/build
$ cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR
$ make
```
**2:**
```
$ cd llvm-project
$ ./build/bin/clang-20 -O0 -S -emit-llvm ./llvm-rpo-numbering-pass/tests/rpo-input.c -o rpo-input.ll
$ ./build/bin/opt -load-pass-plugin ./llvm-rpo-numbering-pass/build/librpo-numbering-pass.so -passes=rpo-numbering-pass -disable-output rpo-input.ll
```
