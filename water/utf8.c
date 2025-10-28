#include <stdlib.h>
#include <string.h>
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

#include "utf8.h"
#include <string.h>

/* some of these functions are pretty broken, but at least all the */
/* brokenness is in one place.                                     */

waterUTF8String *water_newUTF8String(void)
{
    waterUTF8String *result = 
	(waterUTF8String *)calloc(1, sizeof(waterUTF8String));

    if(!result) {
	/* EXCEPTION: couldn't allocate memory */
	fprintf(stderr, "memory allocation failed in water_newUTF8String.\n");
	return 0;
    }

    return result;
}

void water_freeUTF8String(waterUTF8String *string)
{
    if(string) {
	if(string->contents)
	    free(string->contents);
	free(string);
    }
}

waterUTF8String *water_NormalToUTF8(const unsigned char *string)
{
    waterUTF8String *result = water_newUTF8String();

    water_UTF8SetValueNormal(result, string);

    return result;
}

unsigned char *water_UTF8ToNormal(const waterUTF8String *utf8_string)
{
    unsigned char *result =
	(unsigned char *)malloc(utf8_string->length + 1);

    memcpy(result, utf8_string->contents, utf8_string->length);
    result[utf8_string->length] = '\0';

    return result;
}

int water_UTF8Compare(const waterUTF8String *str1, 
		      const waterUTF8String *str2)
{
    /* XXX only tests equality, not order */
    /* XXX completely broken anyway, I don't even know if UTF8 */
    /* encoding is unique                                      */
    return !(str1->length == str2->length &&
	     memcmp(str1->contents, str2->contents, str1->length) == 0);
}

void water_UTF8SetValue(waterUTF8String *utf8_string,
			int length, const unsigned char *contents)
{
    if(utf8_string->contents)
	free(utf8_string->contents);

    utf8_string->length = length;
    utf8_string->contents = (unsigned char *)malloc(length);
    memcpy(utf8_string->contents, contents, length);
}

void water_UTF8SetValueNormal(waterUTF8String *utf8_string, 
			      const unsigned char *value)
{
    if(utf8_string->contents)
	free(utf8_string->contents);

    utf8_string->contents = strdup(value);
    utf8_string->length = strlen(value);
}
