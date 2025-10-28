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

#ifndef __exception_h
#define __exception_h

#include "jni.h"
#include "classes.h"
#include "objects.h"

jint water_jni_Throw(JNIEnv *env, jthrowable obj);
jint water_jni_ThrowNew(JNIEnv *env, jclass clazz, const char *message);
jthrowable water_jni_ExceptionOccurred(JNIEnv *env);
void water_jni_ExceptionDescribe(JNIEnv *env);
void water_jni_ExceptionClear(JNIEnv *env);
void water_jni_FatalError(JNIEnv *env, const char *msg);

void water_throwException(water_JNIEnv *wenv,
			  unsigned char *exception_class_name);
void water_throwExceptionNSPR(water_JNIEnv *wenv,
			      unsigned char *exception_class_name,
			      unsigned char *alternate_message);
void water_throwExceptionWithMessage(water_JNIEnv *wenv,
				     unsigned char *exception_class_name,
				     const char *msg);
void water_throwExceptionWithClass(water_JNIEnv *wenv, waterClass *class);
void water_throwExceptionWithClassAndMessage(water_JNIEnv *wenv,
					     waterClass *exception_class,
					     const char *msg);
void water_throwExceptionObject(water_JNIEnv *wenv, waterObject *exception_object);


#endif
