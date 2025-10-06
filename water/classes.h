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

#ifndef __classes_h
#define __classes_h

#include "jni.h"
#include "nspr.h"
#include "utf8.h"

/* yes, these numbers are a little weird */
/* see JVM spec table 4.2                */
#define CONSTANT_CLASS_TAG                   7
#define CONSTANT_FIELDREF_TAG                9
#define CONSTANT_METHODREF_TAG               10
#define CONSTANT_INTERFACE_METHODREF_TAG     11
#define CONSTANT_STRING_TAG                  8
#define CONSTANT_INTEGER_TAG                 3
#define CONSTANT_FLOAT_TAG                   4
#define CONSTANT_LONG_TAG                    5
#define CONSTANT_DOUBLE_TAG                  6
#define CONSTANT_NAMEANDTYPE_TAG             12
#define CONSTANT_UTF8_TAG                    1

typedef struct waterConstantClass {
    jshort class_index;
    struct waterClass *resolved_class;
} waterConstantClass;

typedef struct waterConstantReference {
    /* stuff from the class file */
    jshort class_index;
    jshort name_and_type_index;

    /* stuff we figure out */
    struct {
	struct waterClass *class;
	struct waterField *field;
	struct waterMethod *method;
    } resolved;
} waterConstantReference;

typedef struct waterConstantString {
    jshort string_index;
} waterConstantString;

typedef struct waterConstantNameAndType {
    jshort name_index;
    jshort descriptor_index;
} waterConstantNameAndType;

typedef struct waterConstantPoolEntry {
    jbyte tag;

    union {
	waterConstantClass class;
	waterConstantReference reference;
	waterConstantString string;
	jint int_value;
	jfloat float_value;
	jlong long_value;
	jdouble double_value;
	waterConstantNameAndType name_and_type;
	waterUTF8String utf8_value;
    } value;
} waterConstantPoolEntry;

typedef struct waterSourceFileAttribute {
    jshort sourcefile_index;
} waterSourceFileAttribute;

typedef struct waterConstantValueAttribute {
    jshort constantvalue_index;
} waterConstantValueAttribute;

typedef struct waterExceptionInfo {
    jshort start_pc;
    jshort end_pc;
    jshort handler_pc;
    jshort catch_type;
    struct waterClass *resolved_catch_type;
} waterExceptionInfo;

typedef struct waterCodeAttribute {
    jshort max_stack;
    jshort max_locals;
    jint code_length;
    unsigned char *code;
    jshort exception_table_length;
    waterExceptionInfo *exception_table;

    jshort attributes_count;
    struct waterAttribute *attributes;
} waterCodeAttribute;

typedef struct waterExceptionsAttribute {
    jshort number_of_exceptions;
    jshort *exception_index_table;
} waterExceptionsAttribute;

typedef struct waterLineNumberInfo {
    jshort start_pc;
    jshort line_number;
} waterLineNumberInfo;

typedef struct waterLineNumberTableAttribute {
    jshort line_number_table_length;
    waterLineNumberInfo *line_number_table;
} waterLineNumberTableAttribute;

typedef struct waterLocalVariableInfo {
    jshort start_pc;
    jshort length;
    jshort name_index;
    jshort descriptor_index;
    jshort index;
} waterLocalVariableInfo;

typedef struct waterLocalVariableTableAttribute {
    jshort local_variable_table_length;
    waterLocalVariableInfo *local_variable_table;
} waterLocalVariableTableAttribute;

typedef struct waterAttribute {
    jshort name_index;
    jint length;
    unsigned char *contents;

    union {
	waterSourceFileAttribute source_file;
	waterConstantValueAttribute constant;
	waterCodeAttribute code;
	waterExceptionsAttribute exceptions;
	waterLineNumberTableAttribute line_numbers;
	waterLocalVariableTableAttribute local_variables;
    } parsed_contents;
} waterAttribute;

typedef struct waterField {
    struct waterClass *class;
    waterUTF8String *name;
    jshort access_flags;
    int width;

    /* for a static field, this is just the value. */
    /* for an instance field, it is a jint offset  */
    /* into the object's data area.                */
    jvalue value;
    int constant_value_initialized;
} waterField;

typedef struct waterMethod {
    jshort index;
    struct waterClass *class;
    struct waterClass *childmost_class;
    waterUTF8String *name;
    waterUTF8String *descriptor;
    jshort access_flags;
    waterCodeAttribute *code;
    void *native_code;

    int num_arguments;
    jshort *argument_types;
    int args_size;
    int locals_size;
    jshort return_type;
} waterMethod;

/* field/method/class access flags */
#define ACC_PUBLIC             0x0001
#define ACC_PRIVATE            0x0002
#define ACC_PROTECTED          0x0004
#define ACC_STATIC             0x0008
#define ACC_FINAL              0x0010
#define ACC_SYNCHRONIZED       0x0020
#define ACC_SUPER              0x0020
#define ACC_VOLATILE           0x0040
#define ACC_TRANSIENT          0x0080
#define ACC_NATIVE             0x0100
#define ACC_INTERFACE          0x0200
#define ACC_ABSTRACT           0x0400

typedef struct waterFieldOrMethod {    
    jshort access_flags;
    jshort name_index;
    jshort descriptor_index;
    jshort attributes_count;
    waterAttribute *attributes;

    union {
	struct waterField *field;
	struct waterMethod *method;
    } resolved;
} waterFieldOrMethod;

/* class states */
#define CLASS_FETUS                  1
#define CLASS_PARTIALLY_LOADED       2
#define CLASS_LOADED                 3
#define CLASS_PREPARED               4
#define CLASS_VERIFIED               5
#define CLASS_INITIALIZED            6
#define CLASS_LOADING_FAILED         -1
#define CLASS_PREPARATION_FAILED     -2
#define CLASS_VERIFICATION_FAILED    -3
#define CLASS_INITIALIZATION_FAILED  -4

typedef struct waterClass {
    /* there are three types of class:                             */
    /*    (1) normal classes, loaded from Java .class files.       */
    /*        for these, class_type is 0.                          */
    /*    (2) primitive classes.  there is a class defined for     */
    /*        each primitive type.  for these, class_type is       */
    /*        equal to the JNI_* type defined in jni_types.h.      */
    /*    (3) array classes.  these represent arrays of one        */
    /*        of the first two class types.  there is potentially  */
    /*        one for each class of type (1) or (2), but they      */
    /*        will only be created as needed.                      */
    /*        for an array of type (1) classes, the class_type is  */
    /*        JNI_OBJECT_ARRAY.  for an array of type (2) classes, */
    /*        the class_type is JNI_*_ARRAY with * corresponding   */
    /*        to the primitive type.                               */
    jint class_type;

    /* one other special type for the bytecode verifier--          */
    /* 'placeholder' class for a class that isn't really loaded.   */
    /* these placeholders should never be visible to the rest of   */
    /* the VM.                                                     */
    int is_placeholder;
    struct waterClass *real_class;

    /* array_class points to the corresponding array class for this class   */
    /* (if it has been created).  elements_class points to the class of the */
    /* elements if this is an array class.                                  */
    struct waterClass *elements_class;
    struct waterClass *array_class;

    /* the java.lang.Class object corresponding to this class. */
    /* it has to be unique.                                    */
    struct waterObject *class_object;

    /* who loaded this class (null if the built-in class loader did) */
    struct waterObject *class_loader;

    /* stuff loaded directly from the class file */
    jushort minor_version;
    jushort major_version;

    jushort constant_pool_count;
    waterConstantPoolEntry *constant_pool;

    jushort access_flags;
    jushort this_class;
    jushort super_class;

    jushort interfaces_count;
    jushort *interfaces;

    int num_resolved_interfaces;
    struct waterClass **resolved_interfaces;

    jushort fields_count;
    waterFieldOrMethod *fields;
    
    jushort methods_count;
    waterFieldOrMethod *methods;

    jushort attributes_count;
    waterAttribute *attributes;

    /* stuff we figure out ourselves */
    const unsigned char *name;

    /* synchronization for class initialization */
    PRMonitor *monitor;
    PRThread *initializing_thread;
    int state;    

    /* info about the class's superclass, fields, and methods */
    struct waterClass *resolved_superclass;
    int num_resolved_fields;
    struct waterField *resolved_fields;
    int instance_data_size;

    int method_table_entries;
    struct waterMethod *method_table;
    
    /* so we can create a list of known classes */
    struct waterClass *next;
    struct waterClass *prev;
} waterClass;

#define WATER_CLASS(class) ((waterClass *)class)
#define WATER_METHOD(method) ((waterMethod *)method)
#define WATER_FIELD(field) ((waterField *)field)

#define WATER_CLASS_DECL(class, ptr) waterClass *class = (waterClass *)ptr
#define WATER_METHOD_DECL(method, ptr) waterMethod *method = (waterMethod *)ptr

jclass water_jni_DefineClass(JNIEnv *env, jobject class_loader,
			     const jbyte *buf, jsize buflen);
jclass water_jni_FindClass(JNIEnv *env, const char *name);
jclass water_jni_GetSuperclass(JNIEnv *env, jclass clazz);
jboolean water_jni_IsAssignableFrom(JNIEnv *env, jclass clazz1, jclass clazz2);
jmethodID water_jni_GetMethodID(JNIEnv *env, jclass clazz,
				const char *name, const char *sig);
jmethodID water_jni_GetStaticMethodID(JNIEnv *env, jclass clazz, 
				      const char *name, const char *sig);
jfieldID water_jni_GetFieldID(JNIEnv *env, jclass clazz,
			      const char *name, const char *sig);
jfieldID water_jni_GetStaticFieldID(JNIEnv *env, jclass clazz,
				    const char *name, const char *sig);
jclass water_jni_GetObjectClass(JNIEnv *env, jobject object);

waterClass *water_findClass(JNIEnv *env, struct waterObject *class_loader,
			    const char *name, int desired_state,
			    const char *buf, int buflen);
waterClass *water_resolveClassReference(JNIEnv *env,
					waterClass *class, 
					waterConstantReference *ref);
waterMethod *water_resolveMethodReference(JNIEnv *env,
					  waterClass *class, 
					  waterConstantReference *ref);
waterField *water_resolveFieldReference(JNIEnv *env, waterClass *class,
					waterConstantReference *ref);
waterClass *water_resolveClassByIndex(JNIEnv *env, waterClass *class, 
				      jshort class_index, int desired_state);
void water_freeClass(waterClass *class);
void water_linkNativeMethod(JNIEnv *env, waterMethod *method);
waterUTF8String *water_getClassName(waterClass *class, jshort pool_index);
int water_isSuperclass(JNIEnv *env, waterClass *parent, waterClass *child);
int water_implementsInterface(JNIEnv *env, 
			      waterClass *interface, waterClass *class);
struct waterObject *water_findClassObjectString(JNIEnv *env, const char *name);
struct waterObject *water_findClassObjectClass(JNIEnv *env,
					       struct waterClass *class);
waterClass *water_getPrimitiveClass(JNIEnv *env, int jni_type);
waterClass *water_getCorrespondingArrayClass(JNIEnv *env, waterClass *class);
waterClass *water_getArrayClassByName(JNIEnv *env, const char *name,
				      struct waterObject *class_loader,
				      int desired_state);
int water_arrayDimensions(JNIEnv *env, waterClass *class);
struct waterObject *water_getPrimitiveClassObject(JNIEnv *env, int jni_type);
int water_castIsOK(water_JNIEnv *wenv, waterClass *target, waterClass *source);
void water_loadDefaultNatives(water_JavaVM *vm);
int water_loadNativeLibrary(water_JavaVM *vm, const char *name);
struct waterObject *water_getThreadObject(JNIEnv *env);
waterMethod *water_virtualMethodLookup(waterClass *class,
				       waterUTF8String *method_name,
				       waterUTF8String *method_descriptor);
waterUTF8String *water_getUTF8Constant(waterClass *class, jshort pool_index);
char **water_getClassPathEntries(JNIEnv *env);
int water_raiseState(JNIEnv *env, waterClass *class, int desired_state,
		     const char *buf, int buflen);

#endif
