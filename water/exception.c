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

#include "exception.h"
#include "classes.h"
#include "jni.h"
#include "stack.h"
#include "objects.h"
#include <assert.h>

jint water_jni_Throw(JNIEnv *env, jthrowable obj)
{
    water_throwExceptionObject(WATER_ENV(env), WATER_OBJECT(obj));
    return 0;
}

jint water_jni_ThrowNew(JNIEnv *env, jclass clazz, const char *message)
{
    water_throwExceptionWithClassAndMessage(WATER_ENV(env), WATER_CLASS(clazz),
					    message);
    return 0;
}

jthrowable water_jni_ExceptionOccurred(JNIEnv *env)
{
    waterObject *exception_object;

    if(WATER_ENV(env)->exception_thrown) {
	opstack_pop_reference(WATER_ENV(env)->opstack_top, &exception_object);
	opstack_push_reference(WATER_ENV(env)->opstack_top, &exception_object);
	return exception_object;
    } else {
	return 0;
    }
}

void water_jni_ExceptionDescribe(JNIEnv *env)
{
    /* XXX bleh. */
    /* just find System.err, print the results of getMessage() and do a */
    /* printStackTrace().                                               */
    fprintf(stderr, "ExceptionDescribe (unimplemented) called.\n");
    assert(0);
}

void water_jni_ExceptionClear(JNIEnv *env)
{
    waterObject *ignore;

    /* get rid of the exception object on the stack */
    opstack_pop_reference(WATER_ENV(env)->opstack_top, &ignore);

    /* clear the exception */
    WATER_ENV(env)->exception_thrown = 0;
}

void water_jni_FatalError(JNIEnv *env, const char *msg)
{
    fprintf(stderr, "FATAL ERROR: %s\n", msg);
    assert(0);
    abort();
}


void water_throwException(water_JNIEnv *wenv,
			  unsigned char *exception_class_name)
{
    water_throwExceptionWithMessage(wenv, exception_class_name, 0);
}

void water_throwExceptionNSPR(water_JNIEnv *wenv,
			      unsigned char *exception_class_name,
			      unsigned char *alternate_message)
{
    unsigned char *message;
    int message_length;

    message_length = PR_GetErrorTextLength();
    if(message_length == 0) {
	water_throwExceptionWithMessage(wenv, exception_class_name, 
					alternate_message);
    } else {
	message = (char *)malloc(message_length + 1);
	PR_GetErrorText(message);
	water_throwExceptionWithMessage(wenv, exception_class_name, message);
	free(message);
    }
}

void water_throwExceptionWithMessage(water_JNIEnv *wenv,
				     unsigned char *exception_class_name,
				     const char *msg)
{
    jclass exception_class;

    /* XXX if we get an exception throwing an exception, what happens? */
    exception_class = water_jni_FindClass((JNIEnv *)wenv, 
					  exception_class_name);
    assert(exception_class);
    water_throwExceptionWithClassAndMessage(wenv, exception_class, msg);
}

void water_throwExceptionWithClass(water_JNIEnv *wenv, waterClass *class)
{
    water_throwExceptionWithClassAndMessage(wenv, class, 0);
}

void water_throwExceptionWithClassAndMessage(water_JNIEnv *wenv,
					     waterClass *exception_class,
					     const char *msg)
{
    jmethodID method;
    jobject exception_object;

    if(msg) {
	/* got a message--call the constructor that takes a string. */
	method = water_jni_GetMethodID((JNIEnv *)wenv, exception_class,
				       "<init>", "(Ljava/lang/String;)V");
	assert(method);
	exception_object = 
	    water_jni_NewObject((JNIEnv *)wenv, 
				exception_class, method,
				water_newStringFromBytes((JNIEnv *)wenv, msg));
    } else {
	/* no message--call the constructor that takes no arguments. */
	method = water_jni_GetMethodID((JNIEnv *)wenv, exception_class,
				       "<init>", "()V");
	assert(method);
	exception_object = water_jni_NewObject((JNIEnv *)wenv, 
					       exception_class, method);
    }

    assert(exception_object);

    water_throwExceptionObject(wenv, exception_object);
}

void water_throwExceptionObject(water_JNIEnv *wenv, 
				waterObject *exception_object)
{
#ifdef DEBUG_djm
    fprintf(stderr, 
	    "throwing exception.  object is %p, pushing on stack at %p\n",
	    exception_object, wenv->opstack_top);
#endif

    opstack_push_reference(wenv->opstack_top, &exception_object);
    wenv->exception_thrown = 1;
}
