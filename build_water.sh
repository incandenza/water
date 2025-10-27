#!/bin/bash

# Simple build script for water JVM on modern macOS

echo "Building water JVM..."

# Create directories
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/include
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/bin

# Create minimal nspr.h
cat > dist/Darwin25.1.0_x86_64_DBG.OBJ/include/nspr.h << 'EOF'
#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t         PRInt8;
typedef uint8_t        PRUint8;
typedef int16_t        PRInt16;
typedef uint16_t       PRUint16;
typedef int32_t        PRInt32;
typedef uint32_t       PRUint32;
typedef int64_t        PRInt64;
typedef uint64_t       PRUint64;
typedef float          PRFloat32;
typedef double         PRFloat64;
typedef int            PRStatus;
typedef size_t         PRSize;
typedef ptrdiff_t      PRPtrdiff;
typedef int            PRBool;
typedef struct PRMonitor  PRMonitor;
typedef struct PRThread   PRThread;

#define PR_TRUE         1
#define PR_FALSE        0
#define PR_SUCCESS      0
#define PR_FAILURE      -1
EOF

# Build water with simplified flags
cd water

# Create object directory
mkdir -p Darwin25.1.0_x86_64_DBG.OBJ

# Compile with minimal dependencies
gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/create.o create.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/classes.o classes.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/entry.o entry.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/globals.o globals.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/call.o call.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/stack.o stack.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/objects.o objects.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/utf8.o utf8.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exception.o exception.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/verify.o verify.c

gcc -c -g -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/water.o water.c

# Link the executable
gcc -g -o ../dist/Darwin25.1.0_x86_64_DBG.OBJ/water \
    Darwin25.1.0_x86_64_DBG.OBJ/*.o \
    -lm

echo "Build complete! Binary is at: dist/Darwin25.1.0_x86_64_DBG.OBJ/water"