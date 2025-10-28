/*
 * water
 * a Java virtual machine
 * 
 * Copyright (C) 1998-2025 Dan McGuirk <mcguirk@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __jni_types_h
#define __jni_types_h

/* NSPR type definitions */
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

typedef uint8_t   PRUint8;
typedef int8_t    PRInt8;
typedef uint16_t  PRUint16;
typedef int16_t   PRInt16;
typedef int32_t   PRInt32;
typedef int64_t   PRInt64;
typedef uint32_t  PRUint32;
typedef uint64_t  PRUint64;
typedef double    PRFloat64;
typedef void*     PRMonitor;
typedef struct { int dummy; } PRThread;

/* NSPR stub definitions */
typedef struct { 
    int type;
    int size; 
} PRFileInfo;

typedef struct { int dummy; } PRFileDesc;

#define PR_SUCCESS 0
#define PR_FILE_FILE 1
#define PR_FILE_DIRECTORY 2
#define PR_RDONLY 0x01

/* NSPR function stubs */
static inline int PR_GetFileInfo(const char *name, PRFileInfo *info) { return -1; }
static inline void *PR_Open(const char *name, int flags, int mode) { return NULL; }
static inline int PR_Read(void *fd, void *buf, int len) { return -1; }
static inline int PR_Close(void *fd) { return -1; }
static inline void PR_EnterMonitor(void *monitor) {}
static inline void PR_ExitMonitor(void *monitor) {}

/* use the types from the NSPR */
typedef PRUint8   jboolean;
typedef PRInt8    jbyte;
typedef PRUint16  jchar;
typedef PRInt16   jshort;
typedef PRInt32   jint;
typedef PRInt64   jlong;
/* XXX there is no NSPR 32-bit float type */
typedef float jfloat;
typedef PRFloat64 jdouble;

/* not defined by java, but used by the VM */
typedef PRUint32  juint;
typedef PRUint64  julong;
typedef PRUint8   jubyte;
typedef PRUint16  jushort;

#define JNI_FALSE 0
#define JNI_TRUE  1

/* these correspond to the types in the 'newarray' instruction    */
/* (except for 'object') which is why they may be a little weird. */
#define JNI_OBJECT                    1
#define JNI_BOOLEAN                   4
#define JNI_BYTE                      8
#define JNI_CHAR                      5
#define JNI_SHORT                     9
#define JNI_INT                       10
#define JNI_LONG                      11
#define JNI_FLOAT                     6
#define JNI_DOUBLE                    7

/* special types for the bytecode verifier */
#define JNI_RETURN_ADDRESS            12
#define JNI_ANY_WORD                  13

#define JNI_MAX_TYPE                  13

#define JNI_NONE                      99
#define JNI_INVALID                  100

#define JNI_IS_ARRAY               0x100

/* special flags for the bytecode verifier to distinguish the two halves */
/* of double-word types.                                                 */
#define JNI_IS_FIRST_HALF          0x200
#define JNI_IS_SECOND_HALF         0x400

#define JNI_MAX_ARRAY_TYPE     (JNI_MAX_TYPE | JNI_IS_ARRAY)

#define JNI_OBJECT_ARRAY       (JNI_OBJECT  | JNI_IS_ARRAY)
#define JNI_BOOLEAN_ARRAY      (JNI_BOOLEAN | JNI_IS_ARRAY)
#define JNI_BYTE_ARRAY         (JNI_BYTE    | JNI_IS_ARRAY)
#define JNI_CHAR_ARRAY         (JNI_CHAR    | JNI_IS_ARRAY)
#define JNI_SHORT_ARRAY        (JNI_SHORT   | JNI_IS_ARRAY)
#define JNI_INT_ARRAY          (JNI_INT     | JNI_IS_ARRAY)
#define JNI_LONG_ARRAY         (JNI_LONG    | JNI_IS_ARRAY)
#define JNI_FLOAT_ARRAY        (JNI_FLOAT   | JNI_IS_ARRAY)
#define JNI_DOUBLE_ARRAY       (JNI_DOUBLE  | JNI_IS_ARRAY)

typedef jint jsize;

typedef struct waterObject *jstring;
typedef struct waterObject *jarray;
typedef struct waterObject *jobjectArray;
typedef struct waterObject *jbooleanArray;
typedef struct waterObject *jbyteArray;
typedef struct waterObject *jcharArray;
typedef struct waterObject *jshortArray;
typedef struct waterObject *jintArray;
typedef struct waterObject *jlongArray;
typedef struct waterObject *jfloatArray;
typedef struct waterObject *jdoubleArray;
typedef struct waterObject *jthrowable;

typedef struct waterClass *jclass;
typedef struct waterField *jfieldID;

typedef struct waterObject *jobject;
typedef struct waterMethod *jmethodID;

typedef union {
    jboolean z;
    jbyte    b;
    jchar    c;
    jshort   s;
    jint     i;
    jlong    j;
    jfloat   f;
    jdouble  d;
    jobject  l;
} jvalue;

#endif
