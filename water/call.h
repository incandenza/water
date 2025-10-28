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

#ifndef __call_h
#define __call_h

#include "jni.h"
#include "classes.h"
#include "objects.h"

#define WATER_ARGS_ALREADY_PUSHED  0
#define WATER_ARGS_NEED_PUSHING    1

#define WATER_NO_ARGS              0

#define CALL_METHOD_DECL(returntype, name) \
    returntype name(JNIEnv *env, jobject obj, jmethodID methodID, ...); \
    returntype name ## A(JNIEnv *env, jobject obj, jmethodID methodID,  \
			 jvalue *args);                                 \
    returntype name ## V(JNIEnv *env, jobject obj, jmethodID methodID,  \
			 va_list args)
     
    CALL_METHOD_DECL(jobject,   water_jni_CallObjectMethod);
    CALL_METHOD_DECL(jboolean,  water_jni_CallBooleanMethod);
    CALL_METHOD_DECL(jbyte,     water_jni_CallByteMethod);
    CALL_METHOD_DECL(jchar,     water_jni_CallCharMethod);
    CALL_METHOD_DECL(jshort,    water_jni_CallShortMethod);
    CALL_METHOD_DECL(jint,      water_jni_CallIntMethod);
    CALL_METHOD_DECL(jlong,     water_jni_CallLongMethod);
    CALL_METHOD_DECL(jfloat,    water_jni_CallFloatMethod);
    CALL_METHOD_DECL(jdouble,   water_jni_CallDoubleMethod);
    CALL_METHOD_DECL(void,      water_jni_CallVoidMethod);
    
    CALL_METHOD_DECL(jobject,   water_jni_CallNonvirtualObjectMethod);
    CALL_METHOD_DECL(jboolean,  water_jni_CallNonvirtualBooleanMethod);
    CALL_METHOD_DECL(jbyte,     water_jni_CallNonvirtualByteMethod);
    CALL_METHOD_DECL(jchar,     water_jni_CallNonvirtualCharMethod);
    CALL_METHOD_DECL(jshort,    water_jni_CallNonvirtualShortMethod);
    CALL_METHOD_DECL(jint,      water_jni_CallNonvirtualIntMethod);
    CALL_METHOD_DECL(jlong,     water_jni_CallNonvirtualLongMethod);
    CALL_METHOD_DECL(jfloat,    water_jni_CallNonvirtualFloatMethod);
    CALL_METHOD_DECL(jdouble,   water_jni_CallNonvirtualDoubleMethod);
    CALL_METHOD_DECL(void,      water_jni_CallNonvirtualVoidMethod);

#undef CALL_METHOD_DECL    

#define CALL_STATIC_METHOD_DECL(returntype, name) \
    returntype name(JNIEnv *env, jclass clazz, jmethodID methodID, ...); \
    returntype name ## A(JNIEnv *env, jclass clazz, jmethodID methodID,  \
			 jvalue *args);                                  \
    returntype name ## V(JNIEnv *env, jclass clazz, jmethodID methodID,  \
			 va_list args)

    CALL_STATIC_METHOD_DECL(jobject,   water_jni_CallStaticObjectMethod);
    CALL_STATIC_METHOD_DECL(jboolean,  water_jni_CallStaticBooleanMethod);
    CALL_STATIC_METHOD_DECL(jbyte,     water_jni_CallStaticByteMethod);
    CALL_STATIC_METHOD_DECL(jchar,     water_jni_CallStaticCharMethod);
    CALL_STATIC_METHOD_DECL(jshort,    water_jni_CallStaticShortMethod);
    CALL_STATIC_METHOD_DECL(jint,      water_jni_CallStaticIntMethod);
    CALL_STATIC_METHOD_DECL(jlong,     water_jni_CallStaticLongMethod);
    CALL_STATIC_METHOD_DECL(jfloat,    water_jni_CallStaticFloatMethod);
    CALL_STATIC_METHOD_DECL(jdouble,   water_jni_CallStaticDoubleMethod);
    CALL_STATIC_METHOD_DECL(void,      water_jni_CallStaticVoidMethod);

#undef CALL_STATIC_METHOD_DECL


void water_callMethod(water_JNIEnv *wenv, waterObject *object,
		      waterMethod *method, 
		      int args_already_pushed, va_list ap);

#endif
