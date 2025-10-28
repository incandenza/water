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

#include "jni.h"
#include "create.h"
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>

/* entry points to the JNI code */

jint JNI_GetDefaultJavaVMInitArgs(void *vm_args)
{
    /* check whether they want 1.2-style or 1.1-style arguments */
    JavaVMInitArgs *args_1_2 = (JavaVMInitArgs *)vm_args;

    if(args_1_2->version == 0x00010001) {
	JDK1_1InitArgs *args_1_1 = (JDK1_1InitArgs *)vm_args;
	static char *no_properties[] = {0};

	/* copied these (mostly) from the values that Sun's VM returns */
      	args_1_1->properties = no_properties;
	args_1_1->checkSource = 0;
	args_1_1->nativeStackSize = 131072;
	args_1_1->javaStackSize = 409600;
	args_1_1->minHeapSize = 1048576;
	args_1_1->maxHeapSize = 16777216;
	args_1_1->verifyMode = 0;
	args_1_1->classpath = getenv("CLASSPATH");
	args_1_1->vfprintf = 0;
	args_1_1->exit = 0;
	args_1_1->abort = 0;
	args_1_1->enableClassGC = 0;
	args_1_1->enableVerboseGC = 0;
	args_1_1->disableAsyncGC = 1;
    } else if(args_1_2->version == 0x00010002) {
	/* there is no reason to call JNI_GetDefaultJavaVMInitArgs   */
	/* when using 1.2-style args, according to the JNI 1.2 docs. */
	/* don't die, just don't do anything.                        */
    } else {
	/* unrecognized args structure version */
	return -1;
    }

    return 0;
}

jint JNI_GetCreatedJavaVMs(JavaVM **vmBuf, jsize bufLen, jsize *nVMs)
{
    jint i;

    for(i = 0; i < _water_num_existing_vms && i < bufLen; i++) {
	vmBuf[i] = (JavaVM *)_water_existing_vms[i];
    }

    return 0;
}

jint JNI_CreateJavaVM(JavaVM **p_vm, JNIEnv **p_env, void *vm_args)
{
    JNIEnv *env;
    JavaVM *vm;
    jclass cls;
    jmethodID mid;

    if((vm = water_CreateJavaVM()) == 0) {
	return -1;
    }

    if((env = water_CreateJNIEnv(vm)) == 0) {
	free(vm);
	return -1;
    }

    /* run System.initializeSystemClass, if it's there */
    cls = (*env)->FindClass(env, "java.lang.System");
    if(cls) {
      mid = (*env)->GetStaticMethodID(env, cls, 
				      "initializeSystemClass", "()V");
      if(mid) {
	(*env)->CallStaticVoidMethod(env, cls, mid);
      }
    }

    *p_env = env;
    *p_vm = vm;

    return 0;
}

