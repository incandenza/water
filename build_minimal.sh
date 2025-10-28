#!/bin/bash

# Simple build script for water JVM on modern macOS - minimal version

echo "Building water JVM (minimal)..."

# Create directories
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/include
mkdir -p dist/Darwin25.1.0_x86_64_DBG.OBJ/bin

# Create minimal nspr.h
cat > dist/Darwin25.1.0_x86_64_DBG.OBJ/include/nspr.h << 'EOF'
#ifndef NSPR_H
#define NSPR_H

#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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
typedef int            PRIntn;
typedef int64_t        PRTime;
typedef PRUint32       PRIntervalTime;

typedef struct PRMonitor {
    int dummy;
} PRMonitor;

typedef struct PRThread {
    int dummy;
} PRThread;

typedef struct PRFileDesc {
    int fd;
} PRFileDesc;

typedef enum {
    PR_FILE_FILE = 1,
    PR_FILE_DIRECTORY = 2,
    PR_FILE_OTHER = 3
} PRFileType;

typedef struct PRFileInfo {
    PRFileType type;
    PRInt32 size;
    PRTime creationTime;
    PRTime modifyTime;
} PRFileInfo;

#define PR_TRUE         1
#define PR_FALSE        0
#define PR_SUCCESS      0
#define PR_FAILURE      -1
#define PR_RDONLY       O_RDONLY
#define PR_INTERVAL_NO_TIMEOUT 0xFFFFFFFF

/* Function declarations */
PRStatus PR_EnterMonitor(PRMonitor *mon);
PRStatus PR_ExitMonitor(PRMonitor *mon);
PRMonitor *PR_NewMonitor(void);
PRStatus PR_GetFileInfo(const char *name, PRFileInfo *info);
PRFileDesc *PR_Open(const char *name, PRInt32 flags, PRInt32 mode);
PRInt32 PR_Read(PRFileDesc *fd, void *buf, PRInt32 amount);
PRStatus PR_Close(PRFileDesc *fd);
PRThread *PR_CurrentThread(void);
PRStatus PR_Wait(PRMonitor *mon, PRIntervalTime timeout);
PRStatus PR_Notify(PRMonitor *mon);
PRInt32 PR_GetErrorTextLength(void);
PRInt32 PR_GetErrorText(char *text);

#endif /* NSPR_H */
EOF

# Build water with simplified flags
cd water

# Create object directory
mkdir -p Darwin25.1.0_x86_64_DBG.OBJ

# Only compile the core files that might work
echo "Compiling core files..."

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/entry.o entry.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/globals.o globals.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/utf8.o utf8.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/water.o water.c

gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -I. -I../include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/create.o create.c

# Compile NSPR stubs
gcc -c -g -w -DXP_UNIX -DDARWIN -DMACOSX \
    -I../dist/Darwin25.1.0_x86_64_DBG.OBJ/include \
    -o Darwin25.1.0_x86_64_DBG.OBJ/nspr_stubs.o ../nspr_stubs.c

echo "Linking minimal binary..."

# Link the executable (minimal version)
gcc -g -o ../dist/Darwin25.1.0_x86_64_DBG.OBJ/water \
    Darwin25.1.0_x86_64_DBG.OBJ/entry.o \
    Darwin25.1.0_x86_64_DBG.OBJ/globals.o \
    Darwin25.1.0_x86_64_DBG.OBJ/utf8.o \
    Darwin25.1.0_x86_64_DBG.OBJ/water.o \
    Darwin25.1.0_x86_64_DBG.OBJ/create.o \
    Darwin25.1.0_x86_64_DBG.OBJ/nspr_stubs.o \
    -lm

echo "Minimal build complete! Binary is at: dist/Darwin25.1.0_x86_64_DBG.OBJ/water"