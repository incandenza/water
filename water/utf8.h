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

#ifndef __utf8_h
#define __utf8_h

#include "jni_types.h"

typedef struct waterUTF8String {
    jshort length;
    unsigned char *contents;
} waterUTF8String;

waterUTF8String *water_newUTF8String(void);
void water_freeUTF8String(waterUTF8String *string);
waterUTF8String *water_NormalToUTF8(const unsigned char *string);
unsigned char *water_UTF8ToNormal(const waterUTF8String *utf8_string);
int water_UTF8Compare(const waterUTF8String *str1, const 
		      waterUTF8String *str2);
void water_UTF8SetValue(waterUTF8String *utf8_string,
			int length, const unsigned char *contents);
void water_UTF8SetValueNormal(waterUTF8String *utf8_string, 
			      const unsigned char *value);

#endif
