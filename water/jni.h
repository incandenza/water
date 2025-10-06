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

#ifndef __jni_h
#define __jni_h

#include "jni_types.h"

typedef int JNINativeMethod;  /* XXX */

typedef struct JNINativeInterface *JNIEnv;
typedef struct JNIInvokeInterface *JavaVM;

struct JNINativeInterface {
    void *reserved0;
    void *reserved1;
    void *reserved2;
    void *reserved3;

    jint (*GetVersion)(JNIEnv *env);

    jclass (*DefineClass)(JNIEnv *env, jobject loader, 
			  const jbyte *buf, jsize bufLen);
    jclass (*FindClass)(JNIEnv *env, const char *name);

    void *reserved4;
    void *reserved5;
    void *reserved6;

    jclass (*GetSuperclass)(JNIEnv *env, jclass clazz);
    jboolean (*IsAssignableFrom)(JNIEnv *env, jclass clazz1, jclass clazz2); 

    void *reserved7;

    jint (*Throw)(JNIEnv *env, jthrowable obj); 
    jint (*ThrowNew)(JNIEnv *env, jclass clazz, const char *message); 
    jthrowable (*ExceptionOccurred)(JNIEnv *env); 
    void (*ExceptionDescribe)(JNIEnv *env); 
    void (*ExceptionClear)(JNIEnv *env); 
    void (*FatalError)(JNIEnv *env, const char *msg); 
    
    void *reserved8;
    void *reserved9;

    jobject (*NewGlobalRef)(JNIEnv *env, jobject obj); 
    void (*DeleteGlobalRef)(JNIEnv *env, jobject globalRef); 
    void (*DeleteLocalRef)(JNIEnv *env, jobject localRef); 
    
    jboolean (*IsSameObject)(JNIEnv *env, jobject ref1, jobject ref2);

    void *reserved10;
    void *reserved11;

    jobject (*AllocObject)(JNIEnv *env, jclass clazz); 
    jobject (*NewObject)(JNIEnv *env, jclass clazz, jmethodID methodID, ...); 
    jobject (*NewObjectA)(JNIEnv *env, jclass clazz, jmethodID methodID, 
			  jvalue *args); 
    jobject (*NewObjectV)(JNIEnv *env, jclass clazz, jmethodID methodID, 
			  va_list args);

    jclass (*GetObjectClass)(JNIEnv *env, jobject obj); 
    jboolean (*IsInstanceOf)(JNIEnv *env, jobject obj, jclass clazz); 

    jmethodID (*GetMethodID)(JNIEnv *env, jclass clazz, 
			     const char *name, const char *sig); 
    
#define CALL_METHOD_DECL(returntype, name) \
    returntype (*name)(JNIEnv *env, jobject obj, jmethodID methodID, ...); \
    returntype (*name ## A)(JNIEnv *env, jobject obj, jmethodID methodID, \
			    jvalue *args); \
    returntype (*name ## V)(JNIEnv *env, jobject obj, jmethodID methodID, \
			    va_list args)

    CALL_METHOD_DECL(jobject,   CallObjectMethod);
    CALL_METHOD_DECL(jboolean,  CallBooleanMethod);
    CALL_METHOD_DECL(jbyte,     CallByteMethod);
    CALL_METHOD_DECL(jchar,     CallCharMethod);
    CALL_METHOD_DECL(jshort,    CallShortMethod);
    CALL_METHOD_DECL(jint,      CallIntMethod);
    CALL_METHOD_DECL(jlong,     CallLongMethod);
    CALL_METHOD_DECL(jfloat,    CallFloatMethod);
    CALL_METHOD_DECL(jdouble,   CallDoubleMethod);
    CALL_METHOD_DECL(void,      CallVoidMethod);

    CALL_METHOD_DECL(jobject,   CallNonvirtualObjectMethod);
    CALL_METHOD_DECL(jboolean,  CallNonvirtualBooleanMethod);
    CALL_METHOD_DECL(jbyte,     CallNonvirtualByteMethod);
    CALL_METHOD_DECL(jchar,     CallNonvirtualCharMethod);
    CALL_METHOD_DECL(jshort,    CallNonvirtualShortMethod);
    CALL_METHOD_DECL(jint,      CallNonvirtualIntMethod);
    CALL_METHOD_DECL(jlong,     CallNonvirtualLongMethod);
    CALL_METHOD_DECL(jfloat,    CallNonvirtualFloatMethod);
    CALL_METHOD_DECL(jdouble,   CallNonvirtualDoubleMethod);
    CALL_METHOD_DECL(void,      CallNonvirtualVoidMethod);

#undef CALL_METHOD_DECL    

    jfieldID (*GetFieldID)(JNIEnv *env, jclass clazz, 
			   const char *name, const char *sig); 

#define GET_FIELD_DECL(type, name) \
    type (*name)(JNIEnv *env, jobject obj, jfieldID fieldID)

    GET_FIELD_DECL(jobject,   GetObjectField);
    GET_FIELD_DECL(jboolean,  GetBooleanField);
    GET_FIELD_DECL(jbyte,     GetByteField);
    GET_FIELD_DECL(jchar,     GetCharField);
    GET_FIELD_DECL(jshort,    GetShortField);
    GET_FIELD_DECL(jint,      GetIntField);
    GET_FIELD_DECL(jlong,     GetLongField);
    GET_FIELD_DECL(jfloat,    GetFloatField);
    GET_FIELD_DECL(jdouble,   GetDoubleField);
    
#undef GET_FIELD_DECL

#define SET_FIELD_DECL(type, name) \
    void (*name)(JNIEnv *env, jobject obj, jfieldID fieldID, type value)

    SET_FIELD_DECL(jobject,   SetObjectField);
    SET_FIELD_DECL(jboolean,  SetBooleanField);
    SET_FIELD_DECL(jbyte,     SetByteField);
    SET_FIELD_DECL(jchar,     SetCharField);
    SET_FIELD_DECL(jshort,    SetShortField);
    SET_FIELD_DECL(jint,      SetIntField);
    SET_FIELD_DECL(jlong,     SetLongField);
    SET_FIELD_DECL(jfloat,    SetFloatField);
    SET_FIELD_DECL(jdouble,   SetDoubleField);

#undef SET_FIELD_DECL

    jmethodID (*GetStaticMethodID)(JNIEnv *env, jclass clazz,
				   const char *name, const char *sig);
    
#define CALL_STATIC_METHOD_DECL(returntype, name) \
    returntype (*name)(JNIEnv *env, jclass clazz, jmethodID methodID, ...); \
    returntype (*name ## A)(JNIEnv *env, jclass clazz, jmethodID methodID, \
			    jvalue *args); \
    returntype (*name ## V)(JNIEnv *env, jclass clazz, jmethodID methodID, \
			    va_list args)

    CALL_STATIC_METHOD_DECL(jobject,   CallStaticObjectMethod);
    CALL_STATIC_METHOD_DECL(jboolean,  CallStaticBooleanMethod);
    CALL_STATIC_METHOD_DECL(jbyte,     CallStaticByteMethod);
    CALL_STATIC_METHOD_DECL(jchar,     CallStaticCharMethod);
    CALL_STATIC_METHOD_DECL(jshort,    CallStaticShortMethod);
    CALL_STATIC_METHOD_DECL(jint,      CallStaticIntMethod);
    CALL_STATIC_METHOD_DECL(jlong,     CallStaticLongMethod);
    CALL_STATIC_METHOD_DECL(jfloat,    CallStaticFloatMethod);
    CALL_STATIC_METHOD_DECL(jdouble,   CallStaticDoubleMethod);
    CALL_STATIC_METHOD_DECL(void,      CallStaticVoidMethod);

#undef CALL_STATIC_METHOD_DECL

    jfieldID (*GetStaticFieldID)(JNIEnv *env, jclass clazz, 
				 const char *name, const char *sig);

#define GET_STATIC_FIELD_DECL(type, name) \
    type (*name)(JNIEnv *env, jclass clazz, jfieldID fieldID)

    GET_STATIC_FIELD_DECL(jobject,   GetStaticObjectField);
    GET_STATIC_FIELD_DECL(jboolean,  GetStaticBooleanField);
    GET_STATIC_FIELD_DECL(jbyte,     GetStaticByteField);
    GET_STATIC_FIELD_DECL(jchar,     GetStaticCharField);
    GET_STATIC_FIELD_DECL(jshort,    GetStaticShortField);
    GET_STATIC_FIELD_DECL(jint,      GetStaticIntField);
    GET_STATIC_FIELD_DECL(jlong,     GetStaticLongField);
    GET_STATIC_FIELD_DECL(jfloat,    GetStaticFloatField);
    GET_STATIC_FIELD_DECL(jdouble,   GetStaticDoubleField);
    
#undef GET_STATIC_FIELD_DECL

#define SET_STATIC_FIELD_DECL(type, name) \
    void (*name)(JNIEnv *env, jclass clazz, jfieldID fieldID, type value)

    SET_STATIC_FIELD_DECL(jobject,   SetStaticObjectField);
    SET_STATIC_FIELD_DECL(jboolean,  SetStaticBooleanField);
    SET_STATIC_FIELD_DECL(jbyte,     SetStaticByteField);
    SET_STATIC_FIELD_DECL(jchar,     SetStaticCharField);
    SET_STATIC_FIELD_DECL(jshort,    SetStaticShortField);
    SET_STATIC_FIELD_DECL(jint,      SetStaticIntField);
    SET_STATIC_FIELD_DECL(jlong,     SetStaticLongField);
    SET_STATIC_FIELD_DECL(jfloat,    SetStaticFloatField);
    SET_STATIC_FIELD_DECL(jdouble,   SetStaticDoubleField);

#undef SET_STATIC_FIELD_DECL

    jstring (*NewString)(JNIEnv *env, const jchar *unicodeChars, jsize len);
    jsize (*GetStringLength)(JNIEnv *env, jstring string); 
    const jchar * (*GetStringChars)(JNIEnv *env, jstring string, 
				    jboolean *isCopy); 
    void (*ReleaseStringChars)(JNIEnv *env, jstring string, 
			       const jchar *chars); 

    jstring (*NewStringUTF)(JNIEnv *env, const char *bytes); 
    jsize (*GetStringUTFLength)(JNIEnv *env, jstring string); 
    const char* (*GetStringUTFChars)(JNIEnv *env, jstring string, 
				     jboolean *isCopy); 
    void (*ReleaseStringUTFChars)(JNIEnv *env, jstring string, 
				  const char *utf); 

    jsize (*GetArrayLength)(JNIEnv *env, jarray array); 
    jarray (*NewObjectArray)(JNIEnv *env, jsize length, 
			     jclass elementClass, jobject initialElement); 
    jobject (*GetObjectArrayElement)(JNIEnv *env, 
				     jobjectArray array, jsize index); 
    void (*SetObjectArrayElement)(JNIEnv *env, jobjectArray array, 
				  jsize index, jobject value); 

#define NEW_ARRAY_DECL(returntype, name) \
    returntype (*name)(JNIEnv *env, jsize length)

    NEW_ARRAY_DECL(jbooleanArray,  NewBooleanArray);
    NEW_ARRAY_DECL(jbyteArray,     NewByteArray);
    NEW_ARRAY_DECL(jcharArray,     NewCharArray);
    NEW_ARRAY_DECL(jshortArray,    NewShortArray);
    NEW_ARRAY_DECL(jintArray,      NewIntArray);
    NEW_ARRAY_DECL(jlongArray,     NewLongArray);
    NEW_ARRAY_DECL(jfloatArray,    NewFloatArray);
    NEW_ARRAY_DECL(jdoubleArray,   NewDoubleArray);

#undef NEW_ARRAY_DECL

#define GET_ARRAY_ELEMS_DECL(type, name) \
    type *(*name)(JNIEnv *env, type ## Array array, jboolean *isCopy)

    GET_ARRAY_ELEMS_DECL(jboolean,  GetBooleanArrayElements);
    GET_ARRAY_ELEMS_DECL(jbyte,     GetByteArrayElements);
    GET_ARRAY_ELEMS_DECL(jchar,     GetCharArrayElements);
    GET_ARRAY_ELEMS_DECL(jshort,    GetShortArrayElements);
    GET_ARRAY_ELEMS_DECL(jint,      GetIntArrayElements);
    GET_ARRAY_ELEMS_DECL(jlong,     GetLongArrayElements);
    GET_ARRAY_ELEMS_DECL(jfloat,    GetFloatArrayElements);
    GET_ARRAY_ELEMS_DECL(jdouble,   GetDoubleArrayElements);

#undef GET_ARRAY_ELEMS_DECL

#define RELEASE_ARRAY_ELEMS_DECL(type, name) \
    void (*name)(JNIEnv *env, type ## Array array, type *elems, jint mode)

    RELEASE_ARRAY_ELEMS_DECL(jboolean,  ReleaseBooleanArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jbyte,     ReleaseByteArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jchar,     ReleaseCharArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jshort,    ReleaseShortArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jint,      ReleaseIntArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jlong,     ReleaseLongArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jfloat,    ReleaseFloatArrayElements);
    RELEASE_ARRAY_ELEMS_DECL(jdouble,   ReleaseDoubleArrayElements);

#undef RELEASE_ARRAY_ELEMS_DECL

#define GET_ARRAY_REGION_DECL(type, name) \
    void (*name)(JNIEnv *env, type ## Array array, \
		 jsize start, jsize len, \
		 type *buf)

    GET_ARRAY_REGION_DECL(jboolean,  GetBooleanArrayRegion);
    GET_ARRAY_REGION_DECL(jbyte,     GetByteArrayRegion);
    GET_ARRAY_REGION_DECL(jchar,     GetCharArrayRegion);
    GET_ARRAY_REGION_DECL(jshort,    GetShortArrayRegion);
    GET_ARRAY_REGION_DECL(jint,      GetIntArrayRegion);
    GET_ARRAY_REGION_DECL(jlong,     GetLongArrayRegion);
    GET_ARRAY_REGION_DECL(jfloat,    GetFloatArrayRegion);
    GET_ARRAY_REGION_DECL(jdouble,   GetDoubleArrayRegion);

#undef GET_ARRAY_REGION_DECL

#define SET_ARRAY_REGION_DECL(type, name) \
    void (*name)(JNIEnv *env, type ## Array array, \
		 jsize start, jsize len, \
		 type *buf)

    SET_ARRAY_REGION_DECL(jboolean,  SetBooleanArrayRegion);
    SET_ARRAY_REGION_DECL(jbyte,     SetByteArrayRegion);
    SET_ARRAY_REGION_DECL(jchar,     SetCharArrayRegion);
    SET_ARRAY_REGION_DECL(jshort,    SetShortArrayRegion);
    SET_ARRAY_REGION_DECL(jint,      SetIntArrayRegion);
    SET_ARRAY_REGION_DECL(jlong,     SetLongArrayRegion);
    SET_ARRAY_REGION_DECL(jfloat,    SetFloatArrayRegion);
    SET_ARRAY_REGION_DECL(jdouble,   SetDoubleArrayRegion);

#undef SET_ARRAY_REGION_DECL

    jint (*RegisterNatives)(JNIEnv *env, jclass clazz, 
			    const JNINativeMethod *methods, jint nMethods); 
    jint (*UnregisterNatives)(JNIEnv *env, jclass clazz); 
    
    jint (*MonitorEnter)(JNIEnv *env, jobject obj); 
    jint (*MonitorExit)(JNIEnv *env, jobject obj); 

    jint (*GetJavaVM)(JNIEnv *env, JavaVM **vm); 
};

struct JNIInvokeInterface {
    void *reserved0;
    void *reserved1;
    void *reserved2;

    jint (*DestroyJavaVM)(JavaVM *vm);
    jint (*AttachCurrentThread)(JavaVM *vm, JNIEnv **p_env, void *thr_args);
    jint (*DetachCurrentThread)(JavaVM *vm);
};

/* this is apparently what Sun wants you to do                   */
/* according to my understanding of the diagram at the beginning */
/* of Chapter 2 of the JNI 1.1 specification.                    */
typedef struct water_JNIEnv {
    struct JNINativeInterface *hooks;

    /* other stuff here */
    struct water_JavaVM *wvm;

    /* each thread will have its own copy of this stuff, so these can be */
    /* thread-specific                                                   */
    JNIEnv *next;
    JNIEnv *prev;

    /* these are not thread-specific */
    char *classpath;

    char *tokenized_classpath;
    char **classpath_entries;

    /* some classes that we need to know about to run bytecode */
    struct waterClass *java_lang_object_class;
    struct waterClass *java_lang_string_class;
    struct waterClass *java_lang_class_class;
    struct waterClass *java_lang_throwable_class;
    struct waterClass *java_lang_cloneable_interface;

    /* ugh */
    struct waterClass *boolean_class;
    struct waterClass *byte_class;
    struct waterClass *char_class;
    struct waterClass *short_class;
    struct waterClass *int_class;
    struct waterClass *long_class;
    struct waterClass *float_class;
    struct waterClass *double_class;

    struct waterObject *boolean_class_object;
    struct waterObject *byte_class_object;
    struct waterObject *char_class_object;
    struct waterObject *short_class_object;
    struct waterObject *int_class_object;
    struct waterObject *long_class_object;
    struct waterObject *float_class_object;
    struct waterObject *double_class_object;

    /* these are thread-specific */
    struct waterObject *thread_object;
    int thread_status;

    unsigned char *stack;
    unsigned char *stack_top;
    unsigned char *stack_end;

    /* exception handling */
    int exception_thrown;
    int call_depth;

    /* stuff we need to know to execute a bytecode instruction */
    struct waterObject *running_object;
    struct waterMethod *running_method;
    struct waterCodeAttribute *code;
    int pc;
    unsigned char *frame_top;
    unsigned char *opstack_top;
} water_JNIEnv;

#define CLASS_HASH_TABLE_SIZE 1023 /* XXX what is a good number? */
typedef struct water_JavaVM {
    struct JNIInvokeInterface *hooks;

    /* other stuff here */
    JNIEnv *envs;

    int num_native_libs;
    void **native_libs;

    PRMonitor *global_class_monitor;
    struct waterClass *loaded_classes[CLASS_HASH_TABLE_SIZE];
} water_JavaVM;

/* thread statuses */
#define THREAD_ALIVE     1
#define THREAD_STOPPED   2
#define THREAD_COMPLETE  3
#define THREAD_UNKNOWN  -1

#define WATER_ENV(env) ((water_JNIEnv *)(env))
#define JNIENV(wenv) ((JNIEnv *)(wenv))
#define WATER_VM(vm) ((water_JavaVM *)(vm))

#define WATER_ENV_DECL(wenv) water_JNIEnv *wenv = (water_JNIEnv *)env
#define WATER_VM_DECL(wvm) water_JavaVM *wvm = (water_JavaVM *)vm

/* virtual machine initialization arguments, 1.2- and 1.1-style */
typedef struct JavaVMOption {
    char *name;
    union {
	jint i;
	jdouble d;
	void *p;
    } value;
} JavaVMOption;

typedef struct JavaVMInitArgs {
    jint version;
    
    jint nOptions;
    JavaVMOption *options;
    jint *result;
} JavaVMInitArgs;

typedef struct JDK1_1InitArgs {
    /* The first two fields were reserved in JDK 1.1, and
       formally introduced in JDK 1.1.2. */
    /* Java VM version */
    jint version;

    /* System properties. */
    char **properties;
    
    /* whether to check the Java source files are newer than 
     * compiled class files. */
    jint checkSource;
    
    /* maximum native stack size of Java-created threads. */
    jint nativeStackSize;
    
    /* maximum Java stack size. */
    jint javaStackSize;
    
    /* initial heap size. */
    jint minHeapSize;
    
    /* maximum heap size. */
    jint maxHeapSize;
    
    /* controls whether Java byte code should be verified:
     * 0 -- none, 1 -- remotely loaded code, 2 -- all code. */
    jint verifyMode;
    
    /* the local directory path for class loading. */
    const char *classpath;
    
    /* a hook for a function that redirects all VM messages. */
    jint (*vfprintf)(FILE *fp, const char *format, 
		     va_list args);
    
    /* a VM exit hook. */
    void (*exit)(jint code);
    
    /* a VM abort hook. */
    void (*abort)();
    
    /* whether to enable class GC. */
    jint enableClassGC;
    
    /* whether GC messages will appear. */
    jint enableVerboseGC;
    
    /* whether asynchronous GC is allowed. */
    jint disableAsyncGC;
    
    /* Three reserved fields. */ 
    jint reserved0;
    jint reserved1;
    jint reserved2;
} JDK1_1InitArgs;

jint JNI_GetDefaultJavaVMInitArgs(void *vm_args);
jint JNI_GetCreatedJavaVMs(JavaVM **vmBuf, jsize bufLen, jsize *nVMs); 
jint JNI_CreateJavaVM(JavaVM **p_vm, JNIEnv **p_env, void *vm_args);

#endif
