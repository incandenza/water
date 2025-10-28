#!/bin/bash

# Simple build script for water JVM on modern macOS

echo "Building water JVM..."

# Create directories
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/include
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/bin

# Use NSPR stubs instead of real NSPR for simpler build
echo "Using NSPR stubs for simpler build"

# Build water with simplified flags
cd water

# Create object directory
mkdir -p Darwin25.1.0_x86_64_DBG.OBJ

# Compile with minimal dependencies and suppress warnings
gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/create.o create.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/classes.o classes.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/entry.o entry.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/globals.o globals.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/call.o call.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/stack.o stack.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/objects.o objects.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/utf8.o utf8.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exception.o exception.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/verify.o verify.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/water.o water.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec.o exec.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec-instr-pre.o exec-instr-pre.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec-instr-protos.o exec-instr-protos.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec-instr-table.o exec-instr-table.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec-instr-switch.o exec-instr-switch.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/exec-instr-macros.o exec-instr-macros.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/miscjni.o miscjni.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/native-jni.o native-jni/native-jni.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/native-net.o native-net/native-net.c

# Skip native-vmspecific for now due to missing prthread.h
# gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
#     -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
#     -I. -I../include \
#     -o Darwin25.1.0_x86_64_DBG.OBJ/native-vmspecific.o native-vmspecific/native-vmspecific.c

# Compile assembly file
gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/native-asm.o native-asm.S

# Compile NSPR stubs
gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/nspr_stubs.o ../nspr_stubs.c

# Link the executable
gcc -g -o ../dist/Darwin25.1.0_x86_64_DBG.OBJ/water \
    Darwin25.1.0_x86_64_DBG.OBJ/*.o \
    -lm

echo "Build complete! Binary is at: dist/Darwin25.1.0_x86_64_DBG.OBJ/water"