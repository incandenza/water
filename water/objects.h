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

#ifndef __objects_h
#define __objects_h

#include "jni.h"
#include "classes.h"

typedef struct waterObject {
    waterClass *class;
    /* each object requires a monitor for synchronization */
    PRMonitor *monitor;

    /* the object's data */
    unsigned char data[1];
} waterObject;

/* an array is a regular object, with 8 bytes of header info at the  */
/* beginning of the data, followed by the elements.                  */

/* XXX the beginning of the array's data will have to be aligned however   */
/* much an int has to be so we can get the type size.  nothing else should */
/* have to be aligned, though.                                             */

/* XXX the beginning of the elements themselves should be aligned properly */
/* for the given element type, too, because for JNI we have to give direct */
/* pointers into the array's elements.  (or we can give copies and leave   */
/* the arrays themselves unaligned--that would be fine, too.)              */
#define ARRAY_DATA_HEADER_SIZE 8
#define ARRAY_TYPE_SIZE(array) (*((jsize *)(&(array)->data[0])))
#define ARRAY_TYPE_SIZE_PTR(array) (((unsigned char *)(&(array)->data[0])))
#define ARRAY_LENGTH_PTR(array) (((unsigned char *)(&(array)->data[4])))
#define ARRAY_ELEMENT_PTR(array, offset)                         \
    ((unsigned char *)                                           \
     &((array)->data[ARRAY_DATA_HEADER_SIZE +                    \
		    ARRAY_TYPE_SIZE((array)) * (offset)]))

#define WATER_OBJECT(obj) ((waterObject *)obj)

#ifdef DEBUG_djm
#define IS_VALID_REFERENCE(obj) \
     ((obj == 0) || \
      ((WATER_OBJECT(obj)->class != 0) && \
       ((WATER_OBJECT(obj)->class->class_type == 0 && \
	 WATER_OBJECT(obj)->class->minor_version == 3 && \
	 WATER_OBJECT(obj)->class->major_version == 45) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_BOOLEAN) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_BYTE) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_CHAR) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_SHORT) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_INT) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_LONG) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_FLOAT) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_DOUBLE) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_BOOLEAN_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_BYTE_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_CHAR_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_SHORT_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_INT_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_LONG_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_FLOAT_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_DOUBLE_ARRAY) || \
	(WATER_OBJECT(obj)->class->class_type == JNI_OBJECT_ARRAY))))

/* to call methods on a reference, it must be non-null and non-primitive. */
#define IS_CALLABLE_REFERENCE(obj) \
     ((WATER_OBJECT(obj)->class != 0) && \
      ((WATER_OBJECT(obj)->class->class_type == 0 && \
	WATER_OBJECT(obj)->class->minor_version == 3 && \
	WATER_OBJECT(obj)->class->major_version == 45) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_BOOLEAN_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_BYTE_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_CHAR_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_SHORT_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_INT_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_LONG_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_FLOAT_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_DOUBLE_ARRAY) || \
       (WATER_OBJECT(obj)->class->class_type == JNI_OBJECT_ARRAY)))
#endif

jobject water_jni_NewObject(JNIEnv *env, jclass clazz, jmethodID ID, ...);
jobject water_jni_NewObjectA(JNIEnv *env, jclass clazz, jmethodID ID, 
			     jvalue *args);
jobject water_jni_NewObjectV(JNIEnv *env, jclass clazz, jmethodID ID, va_list ap);

#define NEW_ARRAY_FUNC_DECL(type, definetype, name)     \
    type ## Array name(JNIEnv *env, jsize length);      

NEW_ARRAY_FUNC_DECL(jboolean, JNI_BOOLEAN, water_jni_NewBooleanArray)
NEW_ARRAY_FUNC_DECL(jbyte,    JNI_BYTE,    water_jni_NewByteArray)
NEW_ARRAY_FUNC_DECL(jchar,    JNI_CHAR,    water_jni_NewCharArray)
NEW_ARRAY_FUNC_DECL(jshort,   JNI_SHORT,   water_jni_NewShortArray)
NEW_ARRAY_FUNC_DECL(jint,     JNI_INT,     water_jni_NewIntArray)
NEW_ARRAY_FUNC_DECL(jlong,    JNI_LONG,    water_jni_NewLongArray)
NEW_ARRAY_FUNC_DECL(jfloat,   JNI_FLOAT,   water_jni_NewFloatArray)
NEW_ARRAY_FUNC_DECL(jdouble,  JNI_DOUBLE,  water_jni_NewDoubleArray)

#undef NEW_ARRAY_FUNC_DECL

#define GET_ARRAY_ELEMENTS_FUNC_DECL(type, definetype, name)        \
    type *name(JNIEnv *env, type ## Array array, jboolean *isCopy);

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
     void name(JNIEnv *env, type ## Array array, type *elems, jint mode);

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

#define SET_ARRAY_REGION_FUNC_DECL(type, name)      \
     void name(JNIEnv *env, type ## Array j_array,  \
	       jsize start, jsize len, type *buf);

SET_ARRAY_REGION_FUNC_DECL(jboolean,  water_jni_SetBooleanArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jbyte,     water_jni_SetByteArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jchar,     water_jni_SetCharArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jshort,    water_jni_SetShortArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jint,      water_jni_SetIntArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jlong,     water_jni_SetLongArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jfloat,    water_jni_SetFloatArrayRegion)
SET_ARRAY_REGION_FUNC_DECL(jdouble,   water_jni_SetDoubleArrayRegion)

#undef SET_ARRAY_REGION_FUNC_DECL

jsize water_jni_GetArrayLength(JNIEnv *env, jarray j_array);

#define GET_FIELD_DECL(jtype, name) \
    jtype name(JNIEnv *env, jobject obj, jfieldID fieldID);

GET_FIELD_DECL(jobject,   water_jni_GetObjectField)
GET_FIELD_DECL(jboolean,  water_jni_GetBooleanField)
GET_FIELD_DECL(jbyte,     water_jni_GetByteField)
GET_FIELD_DECL(jchar,     water_jni_GetCharField)
GET_FIELD_DECL(jshort,    water_jni_GetShortField)
GET_FIELD_DECL(jint,      water_jni_GetIntField)
GET_FIELD_DECL(jlong,     water_jni_GetLongField)
GET_FIELD_DECL(jfloat,    water_jni_GetFloatField)
GET_FIELD_DECL(jdouble,   water_jni_GetDoubleField)

#undef GET_FIELD_DECL

#define SET_FIELD_DECL(jtype, name) \
    void name(JNIEnv *env, jobject obj, jfieldID fieldID, jtype value);

SET_FIELD_DECL(jobject,   water_jni_SetObjectField)
SET_FIELD_DECL(jboolean,  water_jni_SetBooleanField)
SET_FIELD_DECL(jbyte,     water_jni_SetByteField)
SET_FIELD_DECL(jchar,     water_jni_SetCharField)
SET_FIELD_DECL(jshort,    water_jni_SetShortField)
SET_FIELD_DECL(jint,      water_jni_SetIntField)
SET_FIELD_DECL(jlong,     water_jni_SetLongField)
SET_FIELD_DECL(jfloat,    water_jni_SetFloatField)
SET_FIELD_DECL(jdouble,   water_jni_SetDoubleField)

#undef SET_FIELD_DECL

#define GET_STATIC_FIELD_DECL(jtype, name) \
    jtype name(JNIEnv *env, jclass clazz, jfieldID fieldID);

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

jstring water_jni_NewStringUTF(JNIEnv *env, const char *bytes);
jsize water_jni_GetStringUTFLength(JNIEnv *env, jstring string);
const char *water_jni_GetStringUTFChars(JNIEnv *env, jstring string, 
					jboolean *isCopy);
void water_jni_ReleaseStringUTFChars(JNIEnv *env, jstring string, const char *utf);
jarray water_jni_NewObjectArray(JNIEnv *env, jsize length, 
				jclass elementClass, jobject initialElement);
jobject water_jni_GetObjectArrayElement(JNIEnv *env, 
					jobjectArray array, jsize index);
void water_jni_SetObjectArrayElement(JNIEnv *env, jobjectArray array,
				     jsize index, jobject value);

waterObject *water_newJavaLangClass(JNIEnv *env, waterClass *class);
waterObject *water_newString(JNIEnv *env, waterUTF8String *value);
waterObject *water_newStringFromBytes(JNIEnv *env, const unsigned char *bytes);
waterObject *water_newObject(JNIEnv *env, waterClass *class);

waterObject *water_newPrimitiveArray(JNIEnv *env, jsize length, int type);
waterObject *water_newObjectArray(JNIEnv *env, jsize length, 
				  waterClass *class);
waterObject *water_newMultiArray(JNIEnv *env, waterClass *class,
				 jint dimensions, jint *dimensions_array);
unsigned char *water_getStringBytes(JNIEnv *env, waterObject *string);
int water_nativeTypeSize(int type);
int water_javaTypeStackSize(int type);
char *water_typeName(int type);

#endif
