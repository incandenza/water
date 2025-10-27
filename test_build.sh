#!/bin/bash

echo "Attempting to build water JVM with minimal dependencies..."

# Create a simple test program first
cat > test_water.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Water JVM - Minimal Test\n");
    printf("This is a test to see if basic compilation works\n");
    return 0;
}
EOF

echo "Compiling test program..."
gcc -o test_water test_water.c

if [ $? -eq 0 ]; then
    echo "Test compilation successful!"
    ./test_water
else
    echo "Test compilation failed!"
fi

echo "Now trying to compile some water source files individually..."

cd water

# Create minimal headers
mkdir -p ../include
cat > ../include/minimal_jni.h << 'EOF'
#ifndef MINIMAL_JNI_H
#define MINIMAL_JNI_H

typedef void* jobject;
typedef void* jclass;
typedef void* JNIEnv;
typedef int jint;
typedef int jboolean;

struct JavaVM_ {
    void* functions;
};

typedef struct JavaVM_ JavaVM;

#endif
EOF

# Try compiling entry.c with minimal dependencies
echo "Trying to compile entry.c..."
gcc -c -g -w -I../include -o entry_test.o entry.c 2>&1 | head -10

echo "Checking if entry_test.o was created..."
ls -la entry_test.o 2>/dev/null || echo "entry_test.o not created"

echo "Done with minimal test."