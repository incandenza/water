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
#include "miscjni.h"
#include "classes.h"
#include "call.h"
#include "stack.h"
#include "objects.h"
#include "exception.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NSPR stub definitions */
typedef void* PRMonitor;
typedef struct { int dummy; } PRNetAddr;
typedef struct { char dummy; } PRHostEnt;

#define PR_SUCCESS 0
#define PR_NewMonitor() malloc(sizeof(PRMonitor))
#define PR_EnterMonitor(m) do {} while(0)
#define PR_ExitMonitor(m) do {} while(0)

static void not_implemented(void) { abort(); }
static jobject water_jni_NewGlobalRef(JNIEnv *env, jobject obj) {
    /* For now, just return the same object - no global ref tracking */
    return obj;
}

static void water_jni_DeleteGlobalRef(JNIEnv *env, jobject obj) {
    /* No-op for now - no global ref tracking */
}

static void water_jni_DeleteLocalRef(JNIEnv *env, jobject obj) {
    /* No-op for now - local refs are handled by stack */
}

static jboolean water_jni_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2) {
    return (obj1 == obj2) ? JNI_TRUE : JNI_FALSE;
}

static jobject water_jni_AllocObject(JNIEnv *env, jclass clazz) {
    /* Allocate object without calling constructor */
    return water_newObject(env, clazz);
}

static jboolean water_jni_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz) {
    if (obj == NULL) return JNI_FALSE;
    return water_jni_IsAssignableFrom(env, obj->class, clazz);
}

/* String functions */
static jstring water_jni_NewString(JNIEnv *env, const jchar *unicodeChars, jsize len) {
    /* For now, convert Unicode chars to bytes and use existing UTF-8 functions */
    /* This is a simplified implementation - proper UTF-16 to UTF-8 conversion needed */
    unsigned char *bytes = malloc(len + 1);
    if (bytes == NULL) return NULL;
    
    for (jsize i = 0; i < len; i++) {
        bytes[i] = (unsigned char)(unicodeChars[i] & 0xFF); /* Truncate to 8-bit */
    }
    bytes[len] = '\0';
    
    jstring result = water_newStringFromBytes(env, bytes);
    free(bytes);
    return result;
}

static jsize water_jni_GetStringLength(JNIEnv *env, jstring string) {
    /* Get the actual string length in characters */
    unsigned char *bytes = water_getStringBytes(env, string);
    if (bytes == NULL) return 0;
    
    /* Simple implementation - count bytes */
    jsize len = strlen((char *)bytes);
    return len;
}

static const jchar *water_jni_GetStringChars(JNIEnv *env, jstring string, jboolean *isCopy) {
    unsigned char *bytes = water_getStringBytes(env, string);
    if (bytes == NULL) return NULL;
    
    jsize len = strlen((char *)bytes);
    jchar *unicodeChars = malloc((len + 1) * sizeof(jchar));
    if (unicodeChars == NULL) return NULL;
    
    for (jsize i = 0; i < len; i++) {
        unicodeChars[i] = (jchar)bytes[i];
    }
    unicodeChars[len] = 0;
    
    if (isCopy) *isCopy = JNI_TRUE;
    return unicodeChars;
}

static void water_jni_ReleaseStringChars(JNIEnv *env, jstring string, const jchar *chars) {
    free((void *)chars);
}

/* Array region Get functions - simplified implementations */
static void water_jni_GetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize len, jboolean *buf) {
    /* For now, just zero the buffer - proper implementation would copy from array */
    if (buf) memset(buf, 0, len * sizeof(jboolean));
}

static void water_jni_GetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf) {
    if (buf) memset(buf, 0, len * sizeof(jbyte));
}

static void water_jni_GetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf) {
    if (buf) memset(buf, 0, len * sizeof(jchar));
}

static void water_jni_GetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf) {
    if (buf) memset(buf, 0, len * sizeof(jshort));
}

static void water_jni_GetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf) {
    if (buf) memset(buf, 0, len * sizeof(jint));
}

static void water_jni_GetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf) {
    if (buf) memset(buf, 0, len * sizeof(jlong));
}

static void water_jni_GetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf) {
    if (buf) memset(buf, 0, len * sizeof(jfloat));
}

static void water_jni_GetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf) {
    if (buf) memset(buf, 0, len * sizeof(jdouble));
}

/* Advanced JNI functions */
static jint water_jni_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods) {
    /* For now, just return success - proper implementation would store native methods */
    return 0; /* JNI_OK */
}

static jint water_jni_UnregisterNatives(JNIEnv *env, jclass clazz) {
    /* No-op for now */
    return 0; /* JNI_OK */
}

static jint water_jni_MonitorEnter(JNIEnv *env, jobject obj) {
    if (obj == NULL) return -1; /* JNI_ERR */
    /* Use NSPR monitor functionality */
    if (obj->monitor == NULL) {
        obj->monitor = PR_NewMonitor();
        if (obj->monitor == NULL) return -1; /* JNI_ERR */
    }
    PR_EnterMonitor(obj->monitor);
    return 0; /* JNI_OK */
}

static jint water_jni_MonitorExit(JNIEnv *env, jobject obj) {
    if (obj == NULL || obj->monitor == NULL) return -1; /* JNI_ERR */
    PR_ExitMonitor(obj->monitor);
    return 0; /* JNI_OK */
}

static jint water_jni_GetJavaVM(JNIEnv *env, JavaVM **vm) {
    /* Return the VM instance - would need proper VM structure */
    *vm = (JavaVM *)env; /* Simplified - should point to actual VM */
    return 0; /* JNI_OK */
}

/* Static field setter functions */
static void water_jni_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value) {
    /* Simplified implementation - would need proper field access */
}

static void water_jni_SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value) {
    /* Simplified implementation */
}

static void water_jni_SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value) {
    /* Simplified implementation */
}

/* VM functions */
static jint water_jni_AttachCurrentThread(JavaVM *vm, JNIEnv **env, void *args) {
    /* Simplified implementation - would need proper thread management */
    return 0; /* JNI_OK */
}

static jint water_jni_DetachCurrentThread(JavaVM *vm) {
    /* Simplified implementation */
    return 0; /* JNI_OK */
}
static void not_implemented_7(void) { fprintf(stderr, "unimpl: 7\n"); abort(); }
static void not_implemented_8(void) { fprintf(stderr, "unimpl: 8\n"); abort(); }
static void not_implemented_9(void) { fprintf(stderr, "unimpl: 9\n"); abort(); }
static void not_implemented_10(void) { fprintf(stderr, "unimpl: 10\n"); abort(); }
static void not_implemented_11(void) { fprintf(stderr, "unimpl: 11\n"); abort(); }
static void not_implemented_12(void) { fprintf(stderr, "unimpl: 12\n"); abort(); }
static void not_implemented_13(void) { fprintf(stderr, "unimpl: 13\n"); abort(); }
static void not_implemented_14(void) { fprintf(stderr, "unimpl: 14\n"); abort(); }
static void not_implemented_15(void) { fprintf(stderr, "unimpl: 15\n"); abort(); }
static void not_implemented_16(void) { fprintf(stderr, "unimpl: 16\n"); abort(); }
static void not_implemented_17(void) { fprintf(stderr, "unimpl: 17\n"); abort(); }
static void not_implemented_18(void) { fprintf(stderr, "unimpl: 18\n"); abort(); }
static void not_implemented_19(void) { fprintf(stderr, "unimpl: 19\n"); abort(); }
static void not_implemented_20(void) { fprintf(stderr, "unimpl: 20\n"); abort(); }

static struct JNINativeInterface water_jnienv_hooks = {
    0, /* reserved 0 */
    0, /* reserved 1 */
    0, /* reserved 2 */
    0, /* reserved 3 */
    water_jni_GetVersion, 
    water_jni_DefineClass,
    water_jni_FindClass,
    0, /* reserved 4 */
    0, /* reserved 5 */
    0, /* reserved 6 */
    water_jni_GetSuperclass,
    water_jni_IsAssignableFrom,
    0, /* reserved 7 */
    water_jni_Throw,
    water_jni_ThrowNew,
    water_jni_ExceptionOccurred,
    water_jni_ExceptionDescribe,
    water_jni_ExceptionClear,
    water_jni_FatalError,
    0, /* reserved 8 */
    0, /* reserved 9 */
    water_jni_NewGlobalRef, /* NewGlobalRef */
    water_jni_DeleteGlobalRef, /* DeleteGlobalRef */
    water_jni_DeleteLocalRef, /* DeleteLocalRef */
    water_jni_IsSameObject, /* IsSameObject */
    0, /* reserved 10 */
    0, /* reserved 11 */
    water_jni_AllocObject, /* AllocObject */
    water_jni_NewObject,
    water_jni_NewObjectA,
    water_jni_NewObjectV,
    water_jni_GetObjectClass,
    water_jni_IsInstanceOf, /* IsInstanceOf */
    water_jni_GetMethodID,
    water_jni_CallObjectMethod,
    water_jni_CallObjectMethodA,
    water_jni_CallObjectMethodV,
    water_jni_CallBooleanMethod,
    water_jni_CallBooleanMethodA,
    water_jni_CallBooleanMethodV,
    water_jni_CallByteMethod,
    water_jni_CallByteMethodA,
    water_jni_CallByteMethodV,
    water_jni_CallCharMethod,
    water_jni_CallCharMethodA,
    water_jni_CallCharMethodV,
    water_jni_CallShortMethod,
    water_jni_CallShortMethodA,
    water_jni_CallShortMethodV,
    water_jni_CallIntMethod,
    water_jni_CallIntMethodA,
    water_jni_CallIntMethodV,
    water_jni_CallLongMethod,
    water_jni_CallLongMethodA,
    water_jni_CallLongMethodV,
    water_jni_CallFloatMethod,
    water_jni_CallFloatMethodA,
    water_jni_CallFloatMethodV,
    water_jni_CallDoubleMethod,
    water_jni_CallDoubleMethodA,
    water_jni_CallDoubleMethodV,
    water_jni_CallVoidMethod,
    water_jni_CallVoidMethodA,
    water_jni_CallVoidMethodV,
    water_jni_CallNonvirtualObjectMethod,
    water_jni_CallNonvirtualObjectMethodA,
    water_jni_CallNonvirtualObjectMethodV,
    water_jni_CallNonvirtualBooleanMethod,
    water_jni_CallNonvirtualBooleanMethodA,
    water_jni_CallNonvirtualBooleanMethodV,
    water_jni_CallNonvirtualByteMethod,
    water_jni_CallNonvirtualByteMethodA,
    water_jni_CallNonvirtualByteMethodV,
    water_jni_CallNonvirtualCharMethod,
    water_jni_CallNonvirtualCharMethodA,
    water_jni_CallNonvirtualCharMethodV,
    water_jni_CallNonvirtualShortMethod,
    water_jni_CallNonvirtualShortMethodA,
    water_jni_CallNonvirtualShortMethodV,
    water_jni_CallNonvirtualIntMethod,
    water_jni_CallNonvirtualIntMethodA,
    water_jni_CallNonvirtualIntMethodV,
    water_jni_CallNonvirtualLongMethod,
    water_jni_CallNonvirtualLongMethodA,
    water_jni_CallNonvirtualLongMethodV,
    water_jni_CallNonvirtualFloatMethod,
    water_jni_CallNonvirtualFloatMethodA,
    water_jni_CallNonvirtualFloatMethodV,
    water_jni_CallNonvirtualDoubleMethod,
    water_jni_CallNonvirtualDoubleMethodA,
    water_jni_CallNonvirtualDoubleMethodV,
    water_jni_CallNonvirtualVoidMethod,
    water_jni_CallNonvirtualVoidMethodA,
    water_jni_CallNonvirtualVoidMethodV,
    water_jni_GetFieldID,
    water_jni_GetObjectField,
    water_jni_GetBooleanField,
    water_jni_GetByteField,
    water_jni_GetCharField,
    water_jni_GetShortField,
    water_jni_GetIntField,
    water_jni_GetLongField,
    water_jni_GetFloatField,
    water_jni_GetDoubleField,
    water_jni_SetObjectField,
    water_jni_SetBooleanField,
    water_jni_SetByteField,
    water_jni_SetCharField,
    water_jni_SetShortField,
    water_jni_SetIntField,
    water_jni_SetLongField,
    water_jni_SetFloatField,
    water_jni_SetDoubleField,
    water_jni_GetStaticMethodID,
    water_jni_CallStaticObjectMethod,
    water_jni_CallStaticObjectMethodA,
    water_jni_CallStaticObjectMethodV,
    water_jni_CallStaticBooleanMethod,
    water_jni_CallStaticBooleanMethodA,
    water_jni_CallStaticBooleanMethodV,
    water_jni_CallStaticByteMethod,
    water_jni_CallStaticByteMethodA,
    water_jni_CallStaticByteMethodV,
    water_jni_CallStaticCharMethod,
    water_jni_CallStaticCharMethodA,
    water_jni_CallStaticCharMethodV,
    water_jni_CallStaticShortMethod,
    water_jni_CallStaticShortMethodA,
    water_jni_CallStaticShortMethodV,
    water_jni_CallStaticIntMethod,
    water_jni_CallStaticIntMethodA,
    water_jni_CallStaticIntMethodV,
    water_jni_CallStaticLongMethod,
    water_jni_CallStaticLongMethodA,
    water_jni_CallStaticLongMethodV,
    water_jni_CallStaticFloatMethod,
    water_jni_CallStaticFloatMethodA,
    water_jni_CallStaticFloatMethodV,
    water_jni_CallStaticDoubleMethod,
    water_jni_CallStaticDoubleMethodA,
    water_jni_CallStaticDoubleMethodV,
    water_jni_CallStaticVoidMethod,
    water_jni_CallStaticVoidMethodA,
    water_jni_CallStaticVoidMethodV,
    water_jni_GetStaticFieldID, /* GetStaticFieldID */
    water_jni_GetStaticObjectField,
    water_jni_GetStaticBooleanField,
    water_jni_GetStaticByteField,
    water_jni_GetStaticCharField,
    water_jni_GetStaticShortField,
    water_jni_GetStaticIntField,
    water_jni_GetStaticLongField,
    water_jni_GetStaticFloatField,
    water_jni_GetStaticDoubleField,
    water_jni_SetStaticObjectField, /* set static fields (9) */
    water_jni_SetStaticBooleanField,
    water_jni_SetStaticByteField,
    water_jni_SetStaticCharField,
    water_jni_SetStaticShortField,
    water_jni_SetStaticIntField,
    water_jni_SetStaticLongField,
    water_jni_SetStaticFloatField,
    water_jni_SetStaticDoubleField,
    water_jni_NewString, /* NewString */
    water_jni_GetStringLength, /* GetStringLength */
    water_jni_GetStringChars, /* GetStringChars */
    water_jni_ReleaseStringChars, /* ReleaseStringChars */
    water_jni_NewStringUTF,
    water_jni_GetStringUTFLength,
    water_jni_GetStringUTFChars,
    water_jni_ReleaseStringUTFChars,
    water_jni_GetArrayLength,
    water_jni_NewObjectArray,
    water_jni_GetObjectArrayElement,
    water_jni_SetObjectArrayElement,
    water_jni_NewBooleanArray,
    water_jni_NewByteArray,
    water_jni_NewCharArray,
    water_jni_NewShortArray,
    water_jni_NewIntArray,
    water_jni_NewLongArray,
    water_jni_NewFloatArray,
    water_jni_NewDoubleArray,
    water_jni_GetBooleanArrayElements,
    water_jni_GetByteArrayElements,
    water_jni_GetCharArrayElements,
    water_jni_GetShortArrayElements,
    water_jni_GetIntArrayElements,
    water_jni_GetLongArrayElements,
    water_jni_GetFloatArrayElements,
    water_jni_GetDoubleArrayElements,
    water_jni_ReleaseBooleanArrayElements,
    water_jni_ReleaseByteArrayElements,
    water_jni_ReleaseCharArrayElements,
    water_jni_ReleaseShortArrayElements,
    water_jni_ReleaseIntArrayElements,
    water_jni_ReleaseLongArrayElements,
    water_jni_ReleaseFloatArrayElements,
    water_jni_ReleaseDoubleArrayElements,
    water_jni_GetBooleanArrayRegion, /* get array region (8) */
    water_jni_GetByteArrayRegion,
    water_jni_GetCharArrayRegion,
    water_jni_GetShortArrayRegion,
    water_jni_GetIntArrayRegion,
    water_jni_GetLongArrayRegion,
    water_jni_GetFloatArrayRegion,
    water_jni_GetDoubleArrayRegion,
    water_jni_SetBooleanArrayRegion,
    water_jni_SetByteArrayRegion,
    water_jni_SetCharArrayRegion,
    water_jni_SetShortArrayRegion,
    water_jni_SetIntArrayRegion,
    water_jni_SetLongArrayRegion,
    water_jni_SetFloatArrayRegion,
    water_jni_SetDoubleArrayRegion,
    water_jni_RegisterNatives, /* RegisterNatives */
    water_jni_UnregisterNatives, /* UnregisterNatives */
    water_jni_MonitorEnter, /* MonitorEnter */
    water_jni_MonitorExit, /* MonitorExit */
    water_jni_GetJavaVM, /* GetJavaVM */
};

static struct JNIInvokeInterface water_javavm_hooks = {
    0, /* reserved 0 */
    0, /* reserved 1 */
    0, /* reserved 2 */

    water_jni_DestroyJavaVM, /* DestroyJavaVM */
    water_jni_AttachCurrentThread, /* AttachCurrentThread */
    water_jni_DetachCurrentThread, /* DetachCurrentThread */
};


JNIEnv *water_CreateJNIEnv(JavaVM *vm) 
{
    water_JNIEnv *wenv = 
	(water_JNIEnv *)calloc(1, sizeof(water_JNIEnv));
    
    if(!wenv) 
	return 0;

    wenv->next = 0;
    wenv->hooks = &water_jnienv_hooks;
    wenv->wvm = WATER_VM(vm);
    WATER_VM(vm)->envs = JNIENV(wenv);

    wenv->classpath = getenv("CLASSPATH");
    if(!wenv->classpath)
	wenv->classpath = ".";

    water_createStack(&wenv->stack, &wenv->stack_top, &wenv->stack_end);
    if(!wenv->stack)
	return 0;
    wenv->opstack_top = wenv->stack_top;

    wenv->exception_thrown = 0;
    wenv->call_depth = 0;

    /* there a few classes we just need to have loaded. */
    /* we have to load these without verifying them first because */
    /* the verifier depends on having them loaded.                */
    wenv->java_lang_object_class =
	water_findClass(JNIENV(wenv), 0, 
			"java/lang/Object", CLASS_PREPARED, 0, 0);
    wenv->java_lang_string_class = 
	water_findClass(JNIENV(wenv), 0, 
			"java/lang/String", CLASS_PREPARED, 0, 0);
    wenv->java_lang_class_class = 
	water_findClass(JNIENV(wenv), 0, 
			"java/lang/Class", CLASS_PREPARED, 0, 0);
    wenv->java_lang_throwable_class = 
	water_findClass(JNIENV(wenv), 0, 
			"java/lang/Throwable", CLASS_PREPARED, 0, 0);
    wenv->java_lang_cloneable_interface = 
	water_findClass(JNIENV(wenv), 0, 
			"java/lang/Cloneable", CLASS_PREPARED, 0, 0);
    if(!wenv->java_lang_object_class ||
       !wenv->java_lang_string_class ||
       !wenv->java_lang_class_class ||
       !wenv->java_lang_throwable_class ||
       !wenv->java_lang_cloneable_interface) {
	/* can't cope with not having these.  let creating the VM fail. */
	return 0;
    }

    /* now verify if we're verifying local classes. */
    if(water_raiseState(JNIENV(wenv), wenv->java_lang_object_class, 
			CLASS_VERIFIED, 0, 0) < 0) {
	return 0;
    }
    if(water_raiseState(JNIENV(wenv), wenv->java_lang_string_class, 
			CLASS_VERIFIED, 0, 0) < 0) {
	return 0;
    }
    if(water_raiseState(JNIENV(wenv), wenv->java_lang_class_class, 
			CLASS_VERIFIED, 0, 0) < 0) {
	return 0;
    }
    if(water_raiseState(JNIENV(wenv), wenv->java_lang_throwable_class, 
			CLASS_VERIFIED, 0, 0) < 0) {
	return 0;
    }
    if(water_raiseState(JNIENV(wenv), wenv->java_lang_cloneable_interface, 
			CLASS_VERIFIED, 0, 0) < 0) {
	return 0;
    }

    wenv->thread_status = THREAD_ALIVE;

    return JNIENV(wenv);
}

void water_FreeJNIEnv(JNIEnv *env)
{
    WATER_ENV_DECL(wenv);

    /* get off any list we might be on */
    /* XXX the first node is never removed */
    if(wenv->prev) {
	((water_JNIEnv *)wenv->prev)->next = wenv->next;
    }

    /* free a couple of random things */
    if(wenv->tokenized_classpath) {
	free(wenv->tokenized_classpath);
    }

    if(wenv->classpath_entries) {
	free(wenv->classpath_entries);
    }

    /* XXX free the classes */

    /* free the stack */
    water_freeStack(wenv->stack);
}

JavaVM *water_CreateJavaVM()
{
    water_JavaVM *vm = 
	(water_JavaVM *)malloc(sizeof(water_JavaVM));

    if(!vm)
	return 0;

    _water_existing_vms = 
	(water_JavaVM **)
	  realloc(_water_existing_vms,
		  sizeof(water_JavaVM *) * ++_water_num_existing_vms);
    if(!_water_existing_vms) {
	--_water_num_existing_vms;
	free(vm);
	return 0;
    }

    vm->hooks = &water_javavm_hooks;

    vm->global_class_monitor = PR_NewMonitor();

    /* try to load our native methods */
    vm->native_libs = 0;
    vm->num_native_libs = 0;
    water_loadDefaultNatives(vm);

    _water_existing_vms[_water_num_existing_vms - 1] = vm;

    return (JavaVM *)vm;
}

jint water_jni_DestroyJavaVM(JavaVM *vm)
{
    WATER_VM_DECL(wvm);
    JNIEnv *env, *env_next;

    /* free each env */
    env = wvm->envs;
    while(env) {
	env_next = ((water_JNIEnv *)env)->next;
	water_FreeJNIEnv(env);
	env = env_next;
    }

    free(wvm);

    return 0;
}
