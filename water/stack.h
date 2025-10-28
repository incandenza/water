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

#ifndef __stack_h
#define __stack_h

#include "byteops.h"

/* in redundancy there is happiness, and joy, and happiness... */
/* conciseness: nice. */

#define PUSH_BYTES(stacktop, ptr, length)     \
do {                                          \
    COPY_BYTES((stacktop), (ptr), (length));  \
    (stacktop) += (length);                   \
} while(0)

#define POP_BYTES(stacktop, ptr, length)                 \
do {                                                     \
    COPY_BYTES((ptr), ((stacktop)-(length)), (length));  \
    (stacktop) -= (length);                              \
} while(0)

#define opstack_push_fieldvalue_word(stacktop, fieldptr)  \
    PUSH_BYTES((stacktop), &(fieldptr)->value, 4)
#define opstack_push_fieldvalue_double_word(stacktop, fieldptr) \
    PUSH_BYTES((stacktop), &(fieldptr)->value, 8)
#define opstack_pop_fieldvalue_word(stacktop, fieldptr)   \
    POP_BYTES((stacktop), &(fieldptr)->value, 4)
#define opstack_pop_fieldvalue_double_word(stacktop, fieldptr) \
    POP_BYTES((stacktop), &(fieldptr)->value, 8)
#define opstack_push_reference(stacktop, wordptr)         \
    PUSH_BYTES((stacktop), (wordptr), 4)
#define opstack_push_int(stacktop, wordptr)               \
    PUSH_BYTES((stacktop), (wordptr), 4)
#define opstack_push_long(stacktop, wordptr)              \
    PUSH_BYTES((stacktop), (wordptr), 8)
#define opstack_push_float(stacktop, wordptr)             \
    PUSH_BYTES((stacktop), (wordptr), 4)
#define opstack_push_double(stacktop, wordptr)            \
    PUSH_BYTES((stacktop), (wordptr), 8)
#define opstack_push_double_word(stacktop, wordptr)       \
    PUSH_BYTES((stacktop), (wordptr), 8)
#define opstack_push_word(stacktop, wordptr)              \
    PUSH_BYTES((stacktop), (wordptr), 4)
#define opstack_pop_reference(stacktop, wordptr)          \
    POP_BYTES((stacktop), (wordptr), 4)
#define opstack_pop_word(stacktop, wordptr)               \
    POP_BYTES((stacktop), (wordptr), 4)
#define opstack_pop_int(stacktop, wordptr)                \
    POP_BYTES((stacktop), (wordptr), 4)
#define opstack_pop_float(stacktop, wordptr)              \
    POP_BYTES((stacktop), (wordptr), 4)
#define opstack_pop_double_word(stacktop, wordptr)        \
    POP_BYTES((stacktop), (wordptr), 8)
#define opstack_pop_double(stacktop, wordptr)             \
    POP_BYTES((stacktop), (wordptr), 8)
#define opstack_pop_long(stacktop, wordptr)               \
    POP_BYTES((stacktop), (wordptr), 8)
#define opstack_pop_word_to_nowhere(stacktop)             \
    do  { (stacktop) -= 4; } while(0)
#define opstack_pop_double_word_to_nowhere(stacktop)      \
    do  { (stacktop) -= 8; } while(0)

#define opstack_dup_word(stacktop)                        \
do {                                                      \
    COPY_BYTES((stacktop), (stacktop)-4, 4);              \
    (stacktop) += 4;                                      \
} while(0)
#define opstack_dup_double_word(stacktop)                 \
do {                                                      \
    COPY_BYTES((stacktop), (stacktop)-8, 8);              \
    (stacktop) += 8;                                      \
} while(0)

#define opstack_push_local_int(stacktop, localtop, offset)\
    PUSH_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_load_local_int(ptr, localtop, offset) \
    COPY_BYTES((ptr), &(localtop)[(offset)*4], 4)  
#define opstack_store_local_int(ptr, localtop, offset) \
    COPY_BYTES(&(localtop)[(offset)*4], (ptr), 4)
#define opstack_push_local_long(stacktop, localtop, offset)\
    PUSH_BYTES((stacktop), &(localtop)[(offset)*4], 8)
#define opstack_push_local_float(stacktop, localtop, offset)\
    PUSH_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_push_local_double(stacktop, localtop, offset)\
    PUSH_BYTES((stacktop), &(localtop)[(offset)*4], 8)
#define opstack_push_local_reference(stacktop, localtop, offset) \
    PUSH_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_pop_local_int(stacktop, localtop, offset) \
    POP_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_pop_local_reference(stacktop, localtop, offset) \
    POP_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_pop_local_float(stacktop, localtop, offset) \
    POP_BYTES((stacktop), &(localtop)[(offset)*4], 4)
#define opstack_pop_local_long(stacktop, localtop, offset) \
    POP_BYTES((stacktop), &(localtop)[(offset)*4], 8)
#define opstack_pop_local_double(stacktop, localtop, offset) \
    POP_BYTES((stacktop), &(localtop)[(offset)*4], 8)

void water_createStack(unsigned char **stack, 
		       unsigned char **stack_top, 
		       unsigned char **stack_end);
void water_freeStack(unsigned char *stack);

#endif
