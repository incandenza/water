#include <stdlib.h>
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

#include <jni.h>

int main(int argc, char *argv[]) 
{
    JNIEnv *env;
    JavaVM *jvm;
    JDK1_1InitArgs vm_args;
    jint res;
    jclass cls;
    jmethodID mid;
    jclass string_class;
    jobject args_array;
    jobject string;
    int i;

    if(argc < 2) {
	fprintf(stderr, "usage: water <class>\n");
	exit(1);
    }

    /* IMPORTANT: specify vm_args version # if you use JDK1.1.2 and beyond */
    vm_args.version = 0x00010001;

    JNI_GetDefaultJavaVMInitArgs(&vm_args);

    res = JNI_CreateJavaVM(&jvm,&env,&vm_args);
    if (res < 0) {
        fprintf(stderr, "Can't create Java VM\n");
        exit(1);
    }

    cls = (*env)->FindClass(env, argv[1]);
    if (cls == 0) {
        fprintf(stderr, "Can't find class %s\n", argv[1]);
        exit(1);
    }
 
    mid = (*env)->GetStaticMethodID(env, cls, "main", 
				    "([Ljava/lang/String;)V");
    if (mid == 0) {
        fprintf(stderr, "Class has no 'main' method.\n");
        exit(1);
    }

    /* set up the arguments */
    string_class = (*env)->FindClass(env, "java/lang/String");
    if(!string_class) {
	fprintf(stderr, "couldn't find class java.lang.String.\n");
	return -1;
    }
    args_array = (*env)->NewObjectArray(env, argc - 2, 
					string_class, 0);
    if(!args_array) {
	fprintf(stderr, "couldn't construct arguments array.\n");
	return -1;
    }

    for(i = 2; i < argc; i++) {
	string = (*env)->NewStringUTF(env, argv[i]);
	(*env)->SetObjectArrayElement(env, args_array, i-2, string);
    }

    (*env)->CallStaticVoidMethod(env, cls, mid, args_array);

    (*jvm)->DestroyJavaVM(jvm);

    return 0;
}

