/*
 * water
 * a Java virtual machine
 * 
 * Copyright (C) 1998-2010 Dan McGuirk <mcguirk@gmail.com>
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

#ifndef __byteops_h
#define __byteops_h

/* just copy bytes, leaving the order alone. */
#define COPY_BYTES(dst, src, length)                              \
    memcpy(dst, src, length)

/* swap byte order */
/* this only needs to be done when loading class files, so it doesn't have  */
/* to be phenomenally fast.  everything else is done in machine word order. */
#define COPY_BYTES_SWAP(dst, src, length)                                    \
do {                                                                         \
    int INDEX_VAR;                                                           \
    for(INDEX_VAR = 0; INDEX_VAR < length; INDEX_VAR++) {                    \
	*((char *)(dst)+INDEX_VAR) = *((char *)(src)+(length)-INDEX_VAR-1);  \
    }                                                                        \
} while(0)

/* copy the bytes of a big-endian number into a machine-ordered number. */
/* that means if the machine is little-endian, the bytes have to be     */
/* swapped.  otherwise, copy normally.                                  */
#if BYTE_ORDER == BIG_ENDIAN
#define COPY_BYTES_BIGENDIAN(dst, src, length) \
    COPY_BYTES(dst, src, length)
#elif BYTE_ORDER == LITTLE_ENDIAN
#define COPY_BYTES_BIGENDIAN(dst, src, length) \
    COPY_BYTES_SWAP(dst, src, length)
#else
#error machine endianness is unknown -- define BYTE_ORDER as \
BIG_ENDIAN or LITTLE_ENDIAN
#endif

#define COPY_BYTES_LVALUE(lvalue, buf, pos, length) \
    COPY_BYTES(&(lvalue), ((char *)(buf)+(pos)), length)
#define COPY_BYTES_LVALUE_BIGENDIAN(lvalue, buf, pos, length) \
    COPY_BYTES_BIGENDIAN(&(lvalue), ((char *)(buf)+(pos)), length)

/* GET_* reads big-endian data from class files. */
#define CHECK_LENGTH(pos, buflen, size) \
do {                                                                         \
    if(((pos) + (size)) > ((buflen) + 1)) {                                  \
	water_throwException(WATER_ENV(env), "java/lang/ClassFormatError");  \
	return -1;                                                           \
    }                                                                        \
} while(0)

#define CLASSFILE_GET_SOMETHING(lvalue, buf, buflen, pos, length)   \
do {                                                                \
    CHECK_LENGTH((pos), (buflen), (length));                        \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), (length));  \
    (pos) += (length);                                              \
} while(0)

#define GET_BYTE(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 1)
#define GET_SHORT(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 2)
#define GET_CHAR(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 2)
#define GET_INT(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 4)
#define GET_LONG(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 8)
#define GET_FLOAT(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 4)
#define GET_DOUBLE(lvalue, buf, buflen, pos) \
    CLASSFILE_GET_SOMETHING((lvalue), (buf), (buflen), (pos), 8)

/* GET_*_NOINC also (are supposed to) read big-endian data. */
#define GET_BYTE_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 1)
#define GET_SHORT_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 2)
#define GET_CHAR_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 2)
#define GET_INT_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 4)
#define GET_LONG_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 8)
#define GET_FLOAT_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 4)
#define GET_DOUBLE_NOINC(lvalue, buf, pos) \
    COPY_BYTES_LVALUE_BIGENDIAN((lvalue), (buf), (pos), 8)

/* these read machine-formatted data */
#define GET_BYTE_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 1)
#define GET_SHORT_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 2)
#define GET_CHAR_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 2)
#define GET_INT_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 4)
#define GET_LONG_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 8)
#define GET_FLOAT_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 4)
#define GET_DOUBLE_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 8)
#define GET_REFERENCE_MACHINE(lvalue, buf, pos) \
    COPY_BYTES_LVALUE((lvalue), (buf), (pos), 4)

#define STORE_BYTE_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 1)
#define STORE_SHORT_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 2)
#define STORE_CHAR_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 2)
#define STORE_INT_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 4)
#define STORE_LONG_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 8)
#define STORE_FLOAT_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 4)
#define STORE_DOUBLE_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 8)
#define STORE_REFERENCE_MACHINE(value, buf, pos) \
    COPY_BYTES(((char *)(buf)+(pos)), &value, 4)

#endif
