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

#include "jni.h"
#include "classes.h"
#include "objects.h"
#include "call.h"
#include "byteops.h"
#include "exception.h"
#include <string.h>
#include <assert.h>

jobject water_jni_NewObject(JNIEnv *env, jclass clazz, 
			    jmethodID methodID, ...)
{
    va_list ap;
    jobject result;

    va_start(ap, methodID);
    result = water_jni_NewObjectV(env, clazz, methodID, ap);
    va_end(ap);

    return result;
}

jobject water_jni_NewObjectA(JNIEnv *env, jclass clazz, 
			     jmethodID methodID, jvalue *args)
{
    abort();
    return 0;
}

jobject water_jni_NewObjectV(JNIEnv *env, jclass clazz,
			     jmethodID methodID, va_list ap)
{
    waterObject *new_object;
    
    new_object = water_newObject(env, WATER_CLASS(clazz));
    water_jni_CallNonvirtualVoidMethodV(env, new_object, methodID, ap);

    return new_object;
}

#define NEW_ARRAY_FUNC_DECL(type, definetype, name)     \
    type ## Array name(JNIEnv *env, jsize length)       \
    {                                                   \
        return water_newPrimitiveArray(env, length, definetype); \
    }

NEW_ARRAY_FUNC_DECL(jboolean, JNI_BOOLEAN, water_jni_NewBooleanArray)
NEW_ARRAY_FUNC_DECL(jbyte,    JNI_BYTE,    water_jni_NewByteArray)
NEW_ARRAY_FUNC_DECL(jchar,    JNI_CHAR,    water_jni_NewCharArray)
NEW_ARRAY_FUNC_DECL(jshort,   JNI_SHORT,   water_jni_NewShortArray)
NEW_ARRAY_FUNC_DECL(jint,     JNI_INT,     water_jni_NewIntArray)
NEW_ARRAY_FUNC_DECL(jlong,    JNI_LONG,    water_jni_NewLongArray)
NEW_ARRAY_FUNC_DECL(jfloat,   JNI_FLOAT,   water_jni_NewFloatArray)
NEW_ARRAY_FUNC_DECL(jdouble,  JNI_DOUBLE,  water_jni_NewDoubleArray)

#undef NEW_ARRAY_FUNC_DECL

jarray water_jni_NewObjectArray(JNIEnv *env, jsize length, 
				jclass elementClass, jobject initialElement)
{
    /* XXX initialElement parameter ignored for now */
    return water_newObjectArray(env, length, elementClass);
}

#define GET_ARRAY_ELEMENTS_FUNC_DECL(type, definetype, name)        \
    type *name(JNIEnv *env, type ## Array array, jboolean *isCopy)  \
    {                                                               \
        *isCopy = JNI_FALSE;                                        \
        return (type *) ARRAY_ELEMENT_PTR((array), 0);              \
    }

GET_ARRAY_ELEMENTS_FUNC_DECL(jboolean, JNI_BOOLEAN, 
			     water_jni_GetBooleanArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jbyte,    JNI_BYTE,    
			     water_jni_GetByteArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jchar,    JNI_CHAR,    
			     water_jni_GetCharArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jshort,   JNI_SHORT,   
			     water_jni_GetShortArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jint,     JNI_INT,     
			     water_jni_GetIntArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jlong,    JNI_LONG,    
			     water_jni_GetLongArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jfloat,   JNI_FLOAT,   
			     water_jni_GetFloatArrayElements)
GET_ARRAY_ELEMENTS_FUNC_DECL(jdouble,  JNI_DOUBLE,  
			     water_jni_GetDoubleArrayElements)

#undef GET_ARRAY_ELEMENTS_FUNC_DECL

/* these functions do nothing */
#define RELEASE_ARRAY_ELEMENTS_FUNC_DECL(type, name)                      \
     void name(JNIEnv *env, type ## Array array, type *elems, jint mode)  \
     {                                                                    \
     }

RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jboolean, 
				 water_jni_ReleaseBooleanArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jbyte,
				 water_jni_ReleaseByteArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jchar,
				 water_jni_ReleaseCharArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jshort,
				 water_jni_ReleaseShortArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jint,
				 water_jni_ReleaseIntArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jlong,
				 water_jni_ReleaseLongArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jfloat,
				 water_jni_ReleaseFloatArrayElements)
RELEASE_ARRAY_ELEMENTS_FUNC_DECL(jdouble,
				 water_jni_ReleaseDoubleArrayElements)

#undef RELEASE_ARRAY_ELEMENTS_FUNC_DECL

#define GET_FIELD_DECL(jtype, machinetype, name) \
    jtype name(JNIEnv *env, jobject obj, jfieldID fieldID)      \
    {                                                           \
        jtype result;                                           \
                                                                \
	GET_ ## machinetype ## _MACHINE                         \
            (result,                                            \
	     &((waterObject *)obj)->data                        \
	     [((waterField *)fieldID)->value.i], 0);            \
                                                                \
	return result;                                          \
    }

GET_FIELD_DECL(jobject,   REFERENCE,  water_jni_GetObjectField)
GET_FIELD_DECL(jboolean,  BYTE,       water_jni_GetBooleanField)
GET_FIELD_DECL(jbyte,     BYTE,       water_jni_GetByteField)
GET_FIELD_DECL(jchar,     CHAR,       water_jni_GetCharField)
GET_FIELD_DECL(jshort,    SHORT,      water_jni_GetShortField)
GET_FIELD_DECL(jint,      INT,        water_jni_GetIntField)
GET_FIELD_DECL(jlong,     LONG,       water_jni_GetLongField)
GET_FIELD_DECL(jfloat,    FLOAT,      water_jni_GetFloatField)
GET_FIELD_DECL(jdouble,   DOUBLE,     water_jni_GetDoubleField)

#undef GET_FIELD_DECL

#define SET_FIELD_DECL(jtype, machinetype, name) \
    void name(JNIEnv *env, jobject obj, jfieldID fieldID, jtype value) \
    {                                                                  \
	STORE_ ## machinetype ## _MACHINE                              \
            (value,                                                    \
	     &((waterObject *)obj)->data                               \
	     [((waterField *)fieldID)->value.i], 0);                   \
    }

SET_FIELD_DECL(jobject,   REFERENCE,  water_jni_SetObjectField)
SET_FIELD_DECL(jboolean,  BYTE,       water_jni_SetBooleanField)
SET_FIELD_DECL(jbyte,     BYTE,       water_jni_SetByteField)
SET_FIELD_DECL(jchar,     CHAR,       water_jni_SetCharField)
SET_FIELD_DECL(jshort,    SHORT,      water_jni_SetShortField)
SET_FIELD_DECL(jint,      INT,        water_jni_SetIntField)
SET_FIELD_DECL(jlong,     LONG,       water_jni_SetLongField)
SET_FIELD_DECL(jfloat,    FLOAT,      water_jni_SetFloatField)
SET_FIELD_DECL(jdouble,   DOUBLE,     water_jni_SetDoubleField)

#undef SET_FIELD_DECL

#define GET_STATIC_FIELD_DECL(jtype, name) \
    jtype name(JNIEnv *env, jclass clazz, jfieldID fieldID) \
    {                                                       \
	/* XXX bad */                                       \
	return *(jtype *)&WATER_FIELD(fieldID)->value;      \
    }

GET_STATIC_FIELD_DECL(jobject,   water_jni_GetStaticObjectField)
GET_STATIC_FIELD_DECL(jboolean,  water_jni_GetStaticBooleanField)
GET_STATIC_FIELD_DECL(jbyte,     water_jni_GetStaticByteField)
GET_STATIC_FIELD_DECL(jchar,     water_jni_GetStaticCharField)
GET_STATIC_FIELD_DECL(jshort,    water_jni_GetStaticShortField)
GET_STATIC_FIELD_DECL(jint,      water_jni_GetStaticIntField)
GET_STATIC_FIELD_DECL(jlong,     water_jni_GetStaticLongField)
GET_STATIC_FIELD_DECL(jfloat,    water_jni_GetStaticFloatField)
GET_STATIC_FIELD_DECL(jdouble,   water_jni_GetStaticDoubleField)

#undef GET_STATIC_FIELD_DECL

waterObject *water_newPrimitiveArray(JNIEnv *env, jsize length, int type)
{
    /* XXX GC interaction */

    int type_size = water_nativeTypeSize(type);
    waterObject *new_array = 
	(waterObject *)calloc(1, 
			      sizeof(waterObject) + 
			      ARRAY_DATA_HEADER_SIZE +
			      (type_size * length) - 1);
    if(!new_array) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }

    new_array->class = water_getPrimitiveClass(env, type | JNI_IS_ARRAY);
    new_array->monitor = PR_NewMonitor();

    STORE_INT_MACHINE(type_size, ARRAY_TYPE_SIZE_PTR(new_array), 0);
    STORE_INT_MACHINE(length, ARRAY_LENGTH_PTR(new_array), 0);

    return new_array;
}

waterObject *water_newObjectArray(JNIEnv *env, jsize length, waterClass *class)
{
    int type_size = water_nativeTypeSize(JNI_OBJECT);
    waterObject *new_array = 
	(waterObject *)calloc(1, 
			      sizeof(waterObject) + 
			      ARRAY_DATA_HEADER_SIZE +
			      (type_size * length) - 1);
    if(!new_array) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }

    if(!class->array_class) {
	class->array_class = water_getCorrespondingArrayClass(env, class);
	if(!class->array_class) {
	    return 0;
	}
    }

    new_array->class = class->array_class;
    /* arrays don't need monitors, right? */
    /* potentially they could.  you can call the methods in java.lang.Object */
    /* on them.  none of those methods are synchronized at the moment,       */
    /* but they could be.                                                    */
    new_array->monitor = PR_NewMonitor();

    STORE_INT_MACHINE(type_size, ARRAY_TYPE_SIZE_PTR(new_array), 0);
    STORE_INT_MACHINE(length, ARRAY_LENGTH_PTR(new_array), 0);

    return new_array;
}

waterObject *water_newMultiArray(JNIEnv *env, waterClass *class,
				 jint dimensions, jint *dimensions_array)
{
    waterObject *outer_array, *inner_array;
    int i;

    /* if we're down to one dimension, create normally. */
    if(dimensions == 1) {
	if(dimensions_array[0] < 0) {
	    water_throwException(WATER_ENV(env),
				 "java/lang/NegativeArraySizeException");
	    return 0;
	}

	if(class->elements_class->class_type == 0) {
	    /* an array of objects */
	    return water_newObjectArray(env, dimensions_array[0], 
					class->elements_class);
	} else {
	    /* an array of primitives */
	    return 
		water_newPrimitiveArray(env, dimensions_array[0], 
					class->elements_class->class_type);
	}
    }

    /* create an array of arrays */
    outer_array = water_newObjectArray(env, dimensions_array[0], 
				       class->elements_class);
    if(WATER_ENV(env)->exception_thrown) {
	return 0;
    }
    for(i = 0; i < dimensions_array[0]; i++) {
	inner_array = water_newMultiArray(env, class->elements_class,
					  dimensions - 1, 
					  &dimensions_array[1]);
	if(WATER_ENV(env)->exception_thrown) {
	    return 0;
	}
	STORE_REFERENCE_MACHINE(inner_array, 
				ARRAY_ELEMENT_PTR(outer_array, i), 0);
    }

    return outer_array;
}

#define SET_ARRAY_REGION_FUNC_DECL(type, name)                                \
    void name(JNIEnv *env, type ## Array j_array,                             \
	      jsize start, jsize len, type *buf)                              \
    {                                                                         \
        int array_length;                                                     \
        GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR((j_array)), 0);        \
        if(start < 0 || len < 0 || (start + len) > array_length) {            \
	    water_throwException(WATER_ENV(env),                              \
				 "java/lang/ArrayIndexOutOfBoundsException"); \
	    return;                                                           \
	}                                                                     \
	memcpy(ARRAY_ELEMENT_PTR(j_array, start), buf,                        \
	       len * ARRAY_TYPE_SIZE(j_array));                               \
    }

SET_ARRAY_REGION_FUNC_DECL(jboolean,  water_jni_SetBooleanArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jbyte,     water_jni_SetByteArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jchar,     water_jni_SetCharArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jshort,    water_jni_SetShortArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jint,      water_jni_SetIntArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jlong,     water_jni_SetLongArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jfloat,    water_jni_SetFloatArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jdouble,   water_jni_SetDoubleArrayRegion)

#undef SET_ARRAY_REGION_FUNC_DECL


jsize water_jni_GetArrayLength(JNIEnv *env, jarray j_array)
{
    int array_length;
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(j_array), 0);
    return array_length;
}


waterObject *water_newJavaLangClass(JNIEnv *env, waterClass *class)
{
    WATER_ENV_DECL(wenv);
    waterObject *new_class_object;

    assert(wenv->java_lang_class_class);
    new_class_object = water_newObject(env, wenv->java_lang_class_class);
    if(!new_class_object) {
	return 0;
    }

    *((waterClass **)&new_class_object->data[wenv->java_lang_class_class->
					    instance_data_size - 4]) = class;

    return new_class_object;
}

/* 
 * here we need to magically create a string without calling any
 * constructors, because the constructors try to create other strings
 * themselves, so we end up in a bad situation.
 */
waterObject *water_newString(JNIEnv *env, waterUTF8String *value)
{
    WATER_ENV_DECL(wenv);
    jcharArray char_array;
    waterObject *new_string;
    jchar *string_in_chars;
    jint int_value;
    int i;

    assert(wenv->java_lang_string_class);
    new_string = water_newObject(env, wenv->java_lang_string_class);

    /* XXX convert UTF8 to Unicode properly here instead */
    string_in_chars = (jchar *)calloc(value->length, sizeof(jchar));
    if(!string_in_chars) {
	/* EXCEPTION */
	return 0;
    }
    for(i = 0; i < value->length; i++) {
	string_in_chars[i] = value->contents[i];
    }

    char_array = water_jni_NewCharArray(env, value->length);
    water_jni_SetCharArrayRegion(env, char_array, 0, value->length,
				 string_in_chars);
    free(string_in_chars);

    STORE_REFERENCE_MACHINE(char_array, &new_string->data, 0);
    /* we also have the private fields "int offset" and "int count" */
    /* "count" seems to be the number of characters in the string,  */
    /* and "offset" the index of the first character (usually 0).   */

    /* private int offset */
    int_value = 0;
    STORE_INT_MACHINE(int_value, &new_string->data, sizeof(jobject));

    /* private int count */
    int_value = value->length;
    STORE_INT_MACHINE(int_value, &new_string->data, 
		      sizeof(jobject)+sizeof(jint));

    return new_string;
}

waterObject *water_newStringFromBytes(JNIEnv *env, const unsigned char *bytes)
{
    waterUTF8String *new_string_utf8;
    waterObject *result;

    new_string_utf8 = water_NormalToUTF8(bytes);
    if(!new_string_utf8) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }
    result = water_newString(env, new_string_utf8);

    free(new_string_utf8);

    return result;
}

waterObject *water_newObject(JNIEnv *env, waterClass *class)
{
    /* XXX GC interaction */
    waterObject *new_object;

    new_object = 
	(waterObject *)calloc(1, 
			      sizeof(waterObject) + 
			      class->instance_data_size - 1);
    if(!new_object) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }

    new_object->class = class;

    /* make sure this is fast or figure out a faster way to do this */
    /* --seems to be plenty fast enough (inlined, etc.)             */
    new_object->monitor = PR_NewMonitor();

    return new_object;
}

jstring water_jni_NewStringUTF(JNIEnv *env, const char *bytes)
{
    return water_newStringFromBytes(env, bytes);
}

jsize water_jni_GetStringUTFLength(JNIEnv *env, jstring string)
{
    /* it is probably OK just to call the utfLength() method here, although  */
    /* that may initialize a bunch of classes.  we don't call this ourselves,*/
    /* only programs invoking the VM through JNI should.  so it shouldn't be */
    /* called during the initialization of anything.  (although it could be  */
    /* if some native initializer methods are written that way, so it's      */
    /* possible that this may have to change.)                               */
    jclass string_class;
    jmethodID utf_length_method;
    jint result;

    string_class = (*env)->FindClass(env, "java/lang/String");
    assert(string_class);
    utf_length_method = (*env)->GetMethodID(env, string_class,
					    "utfLength", "()I");
    assert(utf_length_method);
    result = (*env)->CallIntMethod(env, string, utf_length_method);

    return result;
}

const char *water_jni_GetStringUTFChars(JNIEnv *env, jstring string, 
					jboolean *isCopy)
{
    char *utf_chars;
    jclass string_class;
    jmethodID get_bytes_method;
    jbyteArray byte_array;
    jsize array_length;
    jbyte *array_elements;
    int i;

    string_class = (*env)->FindClass(env, "java/lang/String");
    assert(string_class);
    get_bytes_method = (*env)->GetMethodID(env, string_class,
					   "getBytes", "()[B");
    assert(get_bytes_method);
    byte_array = (*env)->CallObjectMethod(env, string, get_bytes_method);
    assert(byte_array);
    array_elements = (*env)->GetByteArrayElements(env, byte_array, 0);
    assert(array_elements);
    array_length = (*env)->GetArrayLength(env, byte_array);
    utf_chars = (char *)malloc(array_length+1);
    assert(array_length);

    for(i = 0; i < array_length; i++) {
	utf_chars[i] = array_elements[i];
    }
    utf_chars[i] = '\0';
    (*env)->ReleaseByteArrayElements(env, byte_array, array_elements, 0);
    
    return utf_chars;
}

void water_jni_ReleaseStringUTFChars(JNIEnv *env, jstring string, char *utf)
{
    free(utf);
}

jobject water_jni_GetObjectArrayElement(JNIEnv *env, 
					jobjectArray array, jsize index)
{
    jobject result;

    /* XXX check for exceptions */
    GET_REFERENCE_MACHINE(result, ARRAY_ELEMENT_PTR(array, index), 0);

    return result;
}

void water_jni_SetObjectArrayElement(JNIEnv *env, jobjectArray array,
				     jsize index, jobject value)
{
    /* XXX check for exceptions */
    STORE_REFERENCE_MACHINE(value, ARRAY_ELEMENT_PTR(array, index), 0);
}

unsigned char *water_getStringBytes(JNIEnv *env, waterObject *string)
{
    waterObject *char_array;
    int offset, num_chars;
    unsigned char *result;
    int i;

    /* our cheap way of pulling the bytes out of a string when */
    /* java.lang.String.getBytes() is not available.           */

    /* XXX use JNI to pull this instead. */
    GET_REFERENCE_MACHINE(char_array, &string->data[0], 0);
    GET_INT_MACHINE(offset, &string->data[0], 4);
    GET_INT_MACHINE(num_chars, &string->data[0], 8);

    result = (unsigned char *)malloc(num_chars + 1);
    if(!result) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }
    for(i = 0; i < num_chars; i++) {
	short full_char;

	GET_SHORT_MACHINE(full_char, 
			  ARRAY_ELEMENT_PTR(char_array, i + offset), 0);
	result[i] = full_char;
    }
    result[i] = '\0';

    return result;
}

int water_nativeTypeSize(int type)
{
    if(type & JNI_IS_ARRAY) {
	return sizeof(jarray);
    }

    switch(type) {
    case JNI_OBJECT:       return sizeof(jobject);
    case JNI_BOOLEAN:      return sizeof(jboolean);
    case JNI_BYTE:         return sizeof(jbyte);
    case JNI_CHAR:         return sizeof(jchar);
    case JNI_SHORT:        return sizeof(jshort);
    case JNI_INT:          return sizeof(jint);
    case JNI_LONG:         return sizeof(jlong);
    case JNI_FLOAT:        return sizeof(jfloat);
    case JNI_DOUBLE:       return sizeof(jdouble);
    default:                
	/* bad type */
	assert(0);
	return -1;
    }
}

int water_javaTypeStackSize(int type)
{
    /* the size of the type on the stack. */
    /* that means shorts, chars, booleans, etc. (anything smaller than */
    /* an int) are promoted to ints.                                   */
    if(type & JNI_IS_ARRAY) {
	return 4;
    }

    switch(type) {
    case JNI_OBJECT:       return sizeof(jobject);
    case JNI_BOOLEAN:      return sizeof(jint);
    case JNI_BYTE:         return sizeof(jint);
    case JNI_CHAR:         return sizeof(jint);
    case JNI_SHORT:        return sizeof(jint);
    case JNI_INT:          return sizeof(jint);
    case JNI_LONG:         return sizeof(jlong);
    case JNI_FLOAT:        return sizeof(jfloat);
    case JNI_DOUBLE:       return sizeof(jdouble);
    default:               
	/* bad type */
	assert(0);
	return -1;
    }
}

char *water_typeName(int type)
{
    switch(type) {
	/* shouldn't be asking for this one anyway */
    case JNI_OBJECT:       return "reference"; 
    case JNI_BOOLEAN:      return "boolean";
    case JNI_BYTE:         return "byte";
    case JNI_CHAR:         return "char";
    case JNI_SHORT:        return "short";
    case JNI_INT:          return "int";
    case JNI_LONG:         return "long";
    case JNI_FLOAT:        return "float";
    case JNI_DOUBLE:       return "double";
    case JNI_BOOLEAN_ARRAY:      return "[Z";
    case JNI_BYTE_ARRAY:         return "[B";
    case JNI_CHAR_ARRAY:         return "[C";
    case JNI_SHORT_ARRAY:        return "[S";
    case JNI_INT_ARRAY:          return "[I";
    case JNI_LONG_ARRAY:         return "[J";
    case JNI_FLOAT_ARRAY:        return "[F";
    case JNI_DOUBLE_ARRAY:       return "[D";
    default:               
	/* bad type */
	assert(0);
	return "";
    }
}
