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
#include "objects.h"
#include "classes.h"
#include "byteops.h"
#include "exception.h"
#include "stack.h"
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include "prthread.h"

/* XXX I thought this was in <limits.h>--where is it? */
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static water_JNIEnv *getThreadEnv(JNIEnv *env, jobject thread_object);
static void start_thread(void *arg);

void Java_java_lang_System_arraycopy(JNIEnv *env, 
				     jclass class,
				     jobject o1, jint i1,
				     jobject o2, jint i2,
				     jint length)
{
    int o1_length, o2_length;
    int elem_size;
#if defined(DEBUG_djm) && 0
    unsigned char *target, *source;
#endif

    if(o1 == NULL || o2 == NULL) {
	water_throwException(WATER_ENV(env), "java/lang/NullPointerException");
	return;
    }

    /* if either object is not an array, or the arrays are of different */
    /* types, we get an ArrayStoreException.                            */
    if(WATER_OBJECT(o1)->class->class_type == 0 ||
       !(WATER_OBJECT(o1)->class->class_type & JNI_IS_ARRAY) ||
       WATER_OBJECT(o2)->class->class_type == 0 ||
       !(WATER_OBJECT(o2)->class->class_type & JNI_IS_ARRAY) ||
       WATER_OBJECT(o1)->class->class_type != 
       WATER_OBJECT(o2)->class->class_type) {
	water_throwException(WATER_ENV(env), "java/lang/ArrayStoreException");
	return;
    }

    /* if the offsets and lengths aren't good, we get an */
    /* ArrayIndexOutOfBoundsException.                   */
    GET_INT_MACHINE(o1_length, ARRAY_LENGTH_PTR(WATER_OBJECT(o1)), 0);
    GET_INT_MACHINE(o2_length, ARRAY_LENGTH_PTR(WATER_OBJECT(o2)), 0);
    if(i1 < 0 || 
       i2 < 0 || 
       length < 0 ||
       (i1 + length) > o1_length ||
       (i2 + length) > o2_length) {
	water_throwException(WATER_ENV(env), 
			     "java/lang/ArrayIndexOutOfBoundsException");
	return;
    }

#if defined(DEBUG_djm) && 0
    fprintf(stderr, 
	    "arraycopy got args: o1 %p, o2 %p, i1 %d, i2 %d, length %d\n",
	    o1, o2, i1, i2, length);
#endif

    /* the areas may overlap, so use memmove */
    /* element size must be the same for both at this point. */
    GET_INT_MACHINE(elem_size, ARRAY_TYPE_SIZE_PTR(o1), 0);
#if defined(DEBUG_djm) && 0
    fprintf(stderr, "element size is %d\n", elem_size);
    source = ARRAY_ELEMENT_PTR(o1, i1);
    target = ARRAY_ELEMENT_PTR(o2, i2);
    fprintf(stderr, "source is %p and target is %p\n", source, target);
    fprintf(stderr, "source length is %d and target length is %d\n",
	    o1_length, o2_length);
#endif

    memmove(ARRAY_ELEMENT_PTR(o2, i2), ARRAY_ELEMENT_PTR(o1, i1),
	    length * elem_size);
}

jobject Java_java_lang_System_initProperties(JNIEnv *env, jclass class,
					     jobject j_properties)
{
    jmethodID put_method;
    waterObject *properties = (waterObject *)j_properties;
    waterObject *string1, *string2;
    waterUTF8String *string1_utf8, *string2_utf8;
    char *env_val;
    struct utsname uts;
    char cwd[PATH_MAX];

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "initproperties called.\n");
#endif

    /* XXX should load in the properties files here. */
    /* all the properties stuff is for AWT and serialization, etc. */
    /* so there are none that are critical to set right now.       */
    /* (unless there are some that aren't loaded from files, which */
    /* I'll probably find out about...)                            */

    /* actually there are are some that we do need to hard-code    */
    /* defaults for, particularly file.encoding.pkg and            */
    /* file.encoding.                                              */

    put_method = (*env)->GetMethodID(env, properties->class,
				     "put", 
				     "(Ljava/lang/Object;Ljava/lang/Object;)"
				     "Ljava/lang/Object;");
    assert(put_method);

    /* XXX garbage collection */
#define SET_PROPERTY(name, value) \
do {                                                          \
    string1_utf8 = water_NormalToUTF8(name);                  \
    string2_utf8 = water_NormalToUTF8(value);                 \
    assert(string1_utf8);                                     \
    assert(string2_utf8);                                     \
                                                              \
    string1 = water_newString(env, string1_utf8);             \
    string2 = water_newString(env, string2_utf8);             \
    assert(string1);                                          \
    assert(string2);                                          \
                                                              \
    free(string1_utf8);                                       \
    free(string2_utf8);                                       \
                                                              \
    (*env)->CallObjectMethod(env, j_properties, put_method,   \
			     string1, string2);               \
} while(0)

    /* see the API docs for java.lang.System.getProperties()-- the following */
    /* always have to be set.                                                */

    SET_PROPERTY("java.version", 
		 "Water v0 by Dan McGuirk <mcguirk@indirect.com>");
    SET_PROPERTY("java.vendor", "ocean size software");
    SET_PROPERTY("java.vendor.url", "http://www.dontknow.com/");

    env_val = getenv("JAVA_HOME"); if(!env_val) env_val = ".";
    SET_PROPERTY("java.home", env_val);
    SET_PROPERTY("java.class.version", "45.3");
    SET_PROPERTY("java.class.path", WATER_ENV(env)->classpath);
    uname(&uts);
    SET_PROPERTY("os.name", uts.sysname);
    SET_PROPERTY("os.arch", uts.machine);
    SET_PROPERTY("os.version", uts.version);

    SET_PROPERTY("file.separator", "/");
    SET_PROPERTY("path.separator", ":");
    SET_PROPERTY("line.separator", "\n");

    env_val = getenv("USER"); if(!env_val) env_val = "";
    SET_PROPERTY("user.name", env_val);
    env_val = getenv("HOME"); if(!env_val) env_val = "";
    SET_PROPERTY("user.home", env_val);

    cwd[0] = '\0'; /* just in case */
    getcwd(cwd, PATH_MAX);
    SET_PROPERTY("user.dir", cwd);

    SET_PROPERTY("file.encoding", "8859_1");
    SET_PROPERTY("file.encoding.pkg", "sun.io");
    SET_PROPERTY("user.language", "en");

    return properties;
}

jint Java_java_lang_System_identityHashCode(JNIEnv *env, jclass system_class,
					    jobject object)
{
    /* we want to call the hashCode() function from java.lang.Object on */
    /* the object, even though there may be an overridden hashCode()    */
    /* definition.  this is simple using JNI's CallNonvirtual* hooks.   */
    jclass object_class;
    jmethodID hashcode_method;

    object_class = (*env)->FindClass(env, "java/lang/Object");
    if(!object_class) {
	/* exception should be thrown */
	return 0;
    }
    hashcode_method = (*env)->GetMethodID(env, object_class,
					  "hashCode", "()I");
    if(!hashcode_method) {
	/* XXX is exception thrown? */
	return 0;
    }

    return (*env)->CallNonvirtualIntMethod(env, object, hashcode_method);
}

jint Java_java_lang_Object_hashCode(JNIEnv *env, jobject j_object)
{
#if defined(DEBUG_djm) && 0
    printf("java.lang.Object.hashCode called.\n");
#endif

    /* since Object.equals() is only true if the two objects being compared */
    /* are the same object, this is an acceptable hashCode.  specific       */
    /* classes will override this to do something more useful.              */
    /* only problem with this is if our garbage collector decides to move   */
    /* objects.  if so, we must come up with another hashcode.              */
    return (jint)j_object;
}

void Java_java_lang_Object_notify(JNIEnv *env, jobject object)
{
#ifdef DEBUG_djm
    fprintf(stderr, "doing a notify on object %p/monitor %p\n",
	    object, WATER_OBJECT(object)->monitor);
#endif
    PR_Notify(WATER_OBJECT(object)->monitor);
}

void Java_java_lang_Object_notifyAll(JNIEnv *env, jobject object)
{
#ifdef DEBUG_djm
    fprintf(stderr, "doing a notifyAll on object %p/monitor %p\n",
	    object, WATER_OBJECT(object)->monitor);
#endif
    PR_NotifyAll(WATER_OBJECT(object)->monitor);
}

void Java_java_lang_Object_wait(JNIEnv *env, jobject object, jlong timeout)
{
#ifdef DEBUG_djm
    fprintf(stderr, "doing a wait on object %p/monitor %p\n",
	    object, WATER_OBJECT(object)->monitor);
#endif
    if(timeout == 0) {
	PR_Wait(WATER_OBJECT(object)->monitor, PR_INTERVAL_NO_TIMEOUT);
    } else {
	PR_Wait(WATER_OBJECT(object)->monitor, 
		PR_MillisecondsToInterval((long)timeout));
    }
}

jobject Java_java_lang_Object_clone(JNIEnv *env, jobject orig_object)
{
    waterObject *copied_object;
    waterClass *orig_class = WATER_OBJECT(orig_object)->class;

    /* XXX probably should throw NullPointerException */
    assert(orig_object);
    assert(orig_class);

    /* XXX we are supposed to to check if the object's class implements */
    /* the Cloneable interface first, and if it doesn't, throw an       */
    /* exception.                                                       */

    /* we must check for arrays and copy them differently (based on     */
    /* their actual length).                                            */
    if(orig_class->class_type & JNI_IS_ARRAY) {
	/* copy an array. */
	jint array_length;
	jint array_element_size;

	GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(orig_object), 0);
	GET_INT_MACHINE(array_element_size, 
			ARRAY_TYPE_SIZE_PTR(orig_object), 0);

	if(orig_class->class_type == JNI_OBJECT_ARRAY) {
	    copied_object = 
		water_newObjectArray(env, array_length, 
				     orig_class->elements_class);
	} else {
	    copied_object =
		water_newPrimitiveArray(env, array_length,
				orig_class->class_type & ~JNI_IS_ARRAY);
	}
	if(!copied_object) {
	    return 0;
	}
	memcpy(ARRAY_ELEMENT_PTR(copied_object, 0), 
	       ARRAY_ELEMENT_PTR(orig_object, 0),
	       array_length * array_element_size);
    } else {
	/* copy a normal object. */
	copied_object = water_newObject(env, orig_class);
	if(!copied_object) {
	    return 0;
	}
	memcpy(copied_object->data, WATER_OBJECT(orig_object)->data,
	       orig_class->instance_data_size);
    }

    return copied_object;
}

jobject Java_java_lang_Throwable_fillInStackTrace(JNIEnv *env, 
						  jobject throwable_obj)
{
    unsigned char *frame_top, *new_frame_top;
    waterMethod *method, *new_method;
    waterObject *object;
    int pc;
    jclass stringbuffer_class;
    jobject trace_buffer;
    jmethodID stringbuffer_void_constructor;
    jmethodID stringbuffer_append_string;
    jmethodID stringbuffer_append_int;
    jmethodID stringbuffer_tostring;
    jobject string;
    jobject result_string;
    jfieldID backtrace_field;

    stringbuffer_class = (*env)->FindClass(env, "java/lang/StringBuffer");
    if(!stringbuffer_class) {
	assert(1 && 0);
	return 0;
    }
    stringbuffer_void_constructor = 
	(*env)->GetMethodID(env, stringbuffer_class, "<init>", "()V");
    if(!stringbuffer_void_constructor) {
	assert(2 && 0);
	return 0;
    }
    stringbuffer_append_string =
	(*env)->GetMethodID(env, stringbuffer_class, "append", 
			    "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    if(!stringbuffer_append_string) {
	assert(3 && 0);
	return 0;
    }
    stringbuffer_append_int =
	(*env)->GetMethodID(env, stringbuffer_class, "append", 
			    "(I)Ljava/lang/StringBuffer;");
    if(!stringbuffer_append_int) {
	assert(4 && 0);
	return 0;
    }
    stringbuffer_tostring =
	(*env)->GetMethodID(env, stringbuffer_class, "toString", 
			    "()Ljava/lang/String;");
    if(!stringbuffer_tostring) {
	assert(5 && 0);
	return 0;
    }
    trace_buffer = 
	(*env)->NewObject(env, stringbuffer_class, 
			  stringbuffer_void_constructor);
    if(!trace_buffer) {
	assert(6 && 0);
	return 0;
    }

    frame_top = WATER_ENV(env)->frame_top;
    method = WATER_ENV(env)->running_method;
    object = WATER_ENV(env)->running_object;
    pc = WATER_ENV(env)->pc;
#ifdef DEBUG_djm
	fprintf(stderr, "original frame is %p with method %p and pc %d\n",
		frame_top, method, pc);
#endif
    while(frame_top) {
#ifdef DEBUG_djm
	fprintf(stderr, "looking at frame at %p\n", frame_top);
#endif
	/* keep popping frames until we see one with a 'this' pointer that */
	/* is not the current throwable object.  (that is, we don't want   */
	/* to include the constructors for the current object in the stack */
	/* trace).                                                         */
	/* it seems like this would break if, say, you were in an object   */
	/* that derived from Exception which just threw itself when it     */
	/* encountered an exception.  but that won't work anyway, since    */
	/* the stack trace is made when the Throwable object is created.   */
	if(object == throwable_obj) {
#ifdef DEBUG_djm
	    fprintf(stderr, "ignoring frame with object %p\n", object);
#endif
#ifdef DEBUG_djm
	    fprintf(stderr, "popping to next frame...\n");
#endif
	    GET_REFERENCE_MACHINE(new_frame_top, frame_top, 
				  method->args_size + method->locals_size);
	    GET_REFERENCE_MACHINE(object, frame_top,
				  method->args_size + method->locals_size + 4);
	    GET_REFERENCE_MACHINE(new_method, frame_top,
				  method->args_size + method->locals_size + 8);
	    GET_INT_MACHINE(pc, frame_top,
			    method->args_size + method->locals_size + 12);
	    frame_top = new_frame_top;
	    method = new_method;
	    continue;
	}

	/* want to print something like this: */
/* '        ' 'at ' <class name>.<method name><descriptor> '(pc = ' pc ')' */
	string = (*env)->NewStringUTF(env, "        at ");
	(*env)->CallObjectMethod(env, trace_buffer, 
				 stringbuffer_append_string, string);
	string = 
	    water_newString(env, &method->class->constant_pool[
				  method->class->constant_pool[
                                  method->class->this_class].
                                  value.class.class_index].value.utf8_value);
	(*env)->CallObjectMethod(env, trace_buffer, 
				 stringbuffer_append_string, string);
	string = (*env)->NewStringUTF(env, ".");
	(*env)->CallObjectMethod(env, trace_buffer, 
				 stringbuffer_append_string, string);
	string = water_newString(env, method->name);
	(*env)->CallObjectMethod(env, trace_buffer, 
				 stringbuffer_append_string, string);
	string = water_newString(env, method->descriptor);
	(*env)->CallObjectMethod(env, trace_buffer, 
				 stringbuffer_append_string, string);

	if(method->access_flags & ACC_NATIVE) {
	    string = (*env)->NewStringUTF(env, " (native)\n");
	    (*env)->CallObjectMethod(env, trace_buffer, 
				     stringbuffer_append_string, string);
	} else {
	    string = (*env)->NewStringUTF(env, " (pc = ");
	    (*env)->CallObjectMethod(env, trace_buffer, 
				     stringbuffer_append_string, string);
	    (*env)->CallObjectMethod(env, trace_buffer, 
				     stringbuffer_append_int, pc);
	    /* XXX maybe this shouldn't be '\n'--system property end-of-line */
	    /* instead.                                                      */
	    string = (*env)->NewStringUTF(env, ")\n");
	    (*env)->CallObjectMethod(env, trace_buffer, 
				     stringbuffer_append_string, string);
	}

	GET_REFERENCE_MACHINE(new_frame_top, frame_top, 
			      method->args_size + method->locals_size);
	GET_REFERENCE_MACHINE(object, frame_top,
			      method->args_size + method->locals_size + 4);
	GET_REFERENCE_MACHINE(new_method, frame_top,
			      method->args_size + method->locals_size + 8);
	GET_INT_MACHINE(pc, frame_top,
			method->args_size + method->locals_size + 12);
	frame_top = new_frame_top;
	method = new_method;
    }

    result_string = 
	(*env)->CallObjectMethod(env, trace_buffer, stringbuffer_tostring);
    if(!result_string) {
	assert(7 && 0);
	return 0;
    }
    
    backtrace_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, throwable_obj),
			   "backtrace", "Ljava/lang/Object;");
    if(!backtrace_field) {
	assert(8 && 0);
	return 0;
    }
    (*env)->SetObjectField(env, throwable_obj, backtrace_field, result_string);

    return result_string;
}

void Java_java_lang_Throwable_printStackTrace0(JNIEnv *env, 
					       jobject throwable_obj,
					       jobject printstream_obj)
{
    jfieldID backtrace_field;
    jmethodID print_method;
    jobject trace_string;

    backtrace_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, throwable_obj),
			   "backtrace", "Ljava/lang/Object;");
    if(!backtrace_field) {
	assert(9 && 0);
	return;
    }
    trace_string = (*env)->GetObjectField(env, throwable_obj, backtrace_field);
    if(!trace_string) {
	assert(10 && 0);
	return;
    }
    print_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, printstream_obj),
			    "print", "(Ljava/lang/String;)V");
    if(!print_method) {
	assert(11 && 0);
	return;
    }
    (*env)->CallVoidMethod(env, printstream_obj, print_method, trace_string);
}

jobject Java_java_lang_Class_forName(JNIEnv *env, jclass j_class,
				     jobject j_name)
{
    unsigned char *class_name_string;
    waterObject *result;
    unsigned char *p;

    /* XXX */
    /* we can't call java.lang.String.getBytes or anything like that   */
    /* because they in turn call other methods which eventually call   */
    /* Class.forName().  so just pull the characters out of the string */
    /* in this bad, non-Unicode way.  this will break for non-ASCII    */
    /* class names (probably none exist, but they could).              */

#ifdef DEBUG_djm
    fprintf(stderr, "Class.forName() called.\n");
#endif
    class_name_string = water_getStringBytes(env, WATER_OBJECT(j_name));
    if(!class_name_string) {
	/* exception thrown already */
	return 0;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "class name is originally '%s'.\n", class_name_string);
#endif
    /* convert dot-separated name to slash-separated name. */
    p = class_name_string;
    for(p = class_name_string; *p; p++) {
	if(*p == '.') {
	    *p = '/';
	}
    }
#ifdef DEBUG_djm
    fprintf(stderr, "class name is now '%s'.\n", class_name_string);
#endif

    result = water_findClassObjectString(env, class_name_string);
    if(!result) {
	/* exception thrown already */
	return 0;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "result is %p.\n", result);
#endif

    free(class_name_string);
    return result;
}

jobject Java_java_lang_Class_getPrimitiveClass(JNIEnv *env, jclass j_class,
					       jobject j_name)
{
    unsigned char *type_name;
    jobject result;

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "Class.getPrimitiveClass called.\n");
#endif
    type_name = water_getStringBytes(env, (waterObject *)j_name);
    if(!type_name) {
	/* exception thrown already */
	return 0;
    }

    if(strcmp(type_name, "boolean") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_BOOLEAN);
    } else if(strcmp(type_name, "byte") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_BYTE);
    } else if(strcmp(type_name, "char") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_CHAR);
    } else if(strcmp(type_name, "short") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_SHORT);
    } else if(strcmp(type_name, "int") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_INT);
    } else if(strcmp(type_name, "long") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_LONG);
    } else if(strcmp(type_name, "float") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_FLOAT);
    } else if(strcmp(type_name, "double") == 0) {
	result = water_getPrimitiveClassObject(env, JNI_DOUBLE);
    } else {
	result = 0;
	water_throwException(WATER_ENV(env),
			     "java/lang/ClassNotFoundException");
    }
#if defined(DEBUG_djm) && 0
    fprintf(stderr, "Class.getPrimitiveClass result is %p.\n", result);
#endif

    return result;
}

jobject Java_java_lang_Class_newInstance(JNIEnv *env, jobject j_class_object)
{
    waterClass *new_object_class;
    jobject result;

    GET_REFERENCE_MACHINE(new_object_class, 
			  &WATER_OBJECT(j_class_object)->data[0], 0);

    if(new_object_class->class_type != 0) {
	/* we can only create instances of objects and arrays, not     */
	/* primitive types.                                            */

	/* actually, I don't think we're supposed to create arrays     */
	/* here either.  Sun's VM won't.  it's possible for            */
	/* us to do it, though.  have to check this.                   */
	water_throwException(WATER_ENV(env),
			     "java/lang/InstantiationException");
	return 0;
    }

    if(new_object_class->class_type == 0) {
	/* create a new object. */
	jmethodID no_arguments_constructor;
	no_arguments_constructor = 
	    (*env)->GetMethodID(env, new_object_class, "<init>", "()V");
	/* we should always have this method--if nothing else, an empty one */
	/* will be inherited from java.lang.Object.                         */
	assert(no_arguments_constructor);

	result = (*env)->NewObject(env, new_object_class, 
				   no_arguments_constructor);
    }
    /* if the creation failed, we have a null pointer in result and */
    /* an exception has already been thrown.                        */

    return result;
}

jobject Java_java_lang_Object_getClass(JNIEnv *env, jobject j_object)
{
    if(j_object == 0) {
	water_throwException(WATER_ENV(env), 
			     "java/lang/NullPointerException");
	return 0;
    }

    return water_findClassObjectClass(env, WATER_OBJECT(j_object)->class);
}

jobject Java_java_lang_Class_getName(JNIEnv *env, jobject j_class_object)
{
    waterClass *class;
    waterObject *result_string;

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "Class.getName() called.\n");
#endif
    /* could use JNI to throw these exceptions, but we're in */
    /* VM-specific code anyway, and the JNI stuff is a pain. */
    if(j_class_object == 0) {
	water_throwException(WATER_ENV(env), 
			     "java/lang/NullPointerException");
	return 0;
    }

    /* get the waterClass represented by this object */
    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(j_class_object)->data[0], 0);

    if(class->class_type != 0) {
	/* it's a primitive or array. */
	if(class->class_type == JNI_OBJECT_ARRAY) {
	    waterUTF8String *element_class_name = 		
		water_getClassName(class->elements_class, 
				   class->elements_class->this_class);
	    unsigned char *element_class_name_normal = 
		water_UTF8ToNormal(element_class_name);
	    unsigned char *simple_name = 
		malloc(2 + element_class_name->length + 1 + 1);
	    waterUTF8String *simple_name_utf8;

	    if(!element_class_name_normal || !simple_name) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		return 0;
	    }

	    strcpy(simple_name, "[L");
	    strcat(simple_name, element_class_name_normal);
	    strcat(simple_name, ";");

	    simple_name_utf8 = water_NormalToUTF8(simple_name);
	    if(!simple_name_utf8) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		return 0;
	    }

	    result_string = water_newString(env, simple_name_utf8);

	    water_freeUTF8String(simple_name_utf8);
	    free(simple_name);
	    free(element_class_name_normal);
	} else {
	    unsigned char *type_name = water_typeName(class->class_type);
	    waterUTF8String *type_name_utf8 = water_NormalToUTF8(type_name);
	    
#if defined(DEBUG_djm) && 0
	    fprintf(stderr, 
		    "it's a primitive (maybe array) class called '%s'.\n", 
		    type_name);
#endif
	    result_string = water_newString(env, type_name_utf8);

	    water_freeUTF8String(type_name_utf8);
	}
    } else {
	/* it's a regular class. */
	assert(class->minor_version == 3);
	assert(class->constant_pool[class->this_class].tag == 
	       CONSTANT_CLASS_TAG);
	assert(class->constant_pool[class->constant_pool[class->this_class].
				   value.class.class_index].tag == 
	       CONSTANT_UTF8_TAG);

#if defined(DEBUG_djm) && 0
	fprintf(stderr, "it's a regular class.\n");
#endif
	result_string = 
	    water_newString(env,
			    &class->constant_pool[class->constant_pool[
		 class->this_class].value.class.class_index].value.utf8_value);
    }
    
    return result_string;
}

jboolean Java_java_lang_Class_isInstance(JNIEnv *env, jobject class_object,
					 jobject object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);
    
    return 
	water_castIsOK(WATER_ENV(env), class,
		       (*env)->GetObjectClass(env, object)) ? 
	JNI_TRUE : JNI_FALSE;
}

jboolean Java_java_lang_Class_isAssignableFrom(JNIEnv *env, jobject class_object,
					       jclass class)
{
    waterClass *target_class;

    GET_REFERENCE_MACHINE(target_class, 
			  &WATER_OBJECT(class_object)->data[0], 0);
    return water_castIsOK(WATER_ENV(env),
			  target_class, class) ? JNI_TRUE : JNI_FALSE;
}

jboolean Java_java_lang_Class_isInterface(JNIEnv *env, jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    return (class->access_flags & ACC_INTERFACE) ? JNI_TRUE : JNI_FALSE;
}

jboolean Java_java_lang_Class_isArray(JNIEnv *env, jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    return (class->class_type & JNI_IS_ARRAY) ? JNI_TRUE : JNI_FALSE;
}

jboolean Java_java_lang_Class_isPrimitive(JNIEnv *env, jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    return (class->class_type != 0 &&
	    !(class->class_type & JNI_IS_ARRAY)) ? JNI_TRUE : JNI_FALSE;
}

jobject Java_java_lang_Class_getClassLoader(JNIEnv *env, jobject class_object)
{
    /* XXX we will eventually have to make note of the fact that some   */
    /* classes have been loaded by other class loaders, and return them */
    /* properly.                                                        */

    /* classes loaded by the VM itself from the local filesystem return */
    /* null here.                                                       */
    return 0;
}

jobject Java_java_lang_Class_getSuperclass(JNIEnv *env, jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    if(class->resolved_superclass == 0) {
	/* interfaces and java.lang.Object have no superclass. */
	return 0;
    } else {
	return water_findClassObjectClass(env, class->resolved_superclass);
    }
}

jint Java_java_lang_Class_getModifiers(JNIEnv *env, jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    return class->access_flags;
}

jobject Java_java_lang_Class_getComponentType(JNIEnv *env, 
					      jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, &WATER_OBJECT(class_object)->data[0], 0);

    if(class->elements_class) {
	return water_findClassObjectClass(env, class->elements_class);
    } else {
	return 0;
    }
}

void Java_java_lang_ClassLoader_init(JNIEnv *env, jobject class_loader)
{
    /* by the time I write the other functions, this will probably have */
    /* to do something.                                                 */
}

jobject Java_java_lang_ClassLoader_findSystemClass0(JNIEnv *env, 
						    jobject class_loader,
						    jobject class_name_string)
{
    unsigned char *class_name_normal;
    jclass class;

    class_name_normal = water_getStringBytes(env, class_name_string);
    if(!class_name_normal) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "trying to findSystemClass0 on '%s'...\n", 
	    class_name_normal);
#endif
    class = water_jni_FindClass(env, class_name_normal);
    free(class_name_normal);
    if(WATER_ENV(env)->exception_thrown) {
	return 0;
    }
    assert(class);

    return water_findClassObjectClass(env, class);
}

jobject 
Java_java_lang_ClassLoader_getSystemResourceAsStream0(JNIEnv *env,
						      jobject class_loader,
					      jobject resource_name_string)
{
    char **classpath_entries;
    char *resource_name;
    int i;

#ifdef DEBUG_djm
    fprintf(stderr, "in getSystemResourceAsStream0...\n");
#endif
    resource_name = water_getStringBytes(env, resource_name_string);
    classpath_entries = water_getClassPathEntries(env);
    for(i = 0; classpath_entries[i]; i++) {
	PRFileInfo info;

	if(PR_GetFileInfo(classpath_entries[i], &info) != PR_SUCCESS) {
#ifdef DEBUG_djm
	    fprintf(stderr, "warning: couldn't find classpath entry '%s'\n",
		    classpath_entries[i]);
#endif
	    continue;
	}

	if(info.type == PR_FILE_FILE) {
	    /* XXX is it OK to have the filename of the resource directly in */
	    /* the CLASSPATH?                                                */
	} else if(info.type == PR_FILE_DIRECTORY) {
	    waterObject *file_name_string;
	    jclass fileinputstream_class;
	    jmethodID string_constructor;
	    jobject fileinputstream_object;

	    char *file_name =
		(char *)malloc(strlen(classpath_entries[i]) + 
			       1 +                           /* '/' */
			       strlen(resource_name) + 
			       1                             /* '\0' */);
	    if(!file_name) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		return 0;
	    }

	    strcpy(file_name, classpath_entries[i]);
	    strcat(file_name, "/");
	    strcat(file_name, resource_name);

	    if(PR_GetFileInfo(file_name, &info) != PR_SUCCESS) {
		/* not an error, it's just not here. */
		continue;
	    }

	    /* it's here.  make a FileInputStream out of it and return it. */
	    /* if it can't be opened, the FileInputStream constructor will */
	    /* throw an exception.                                         */
	    file_name_string = water_newStringFromBytes(env, file_name);
	    if(!file_name_string) {
		/* exception thrown already */
		return 0;
	    }

	    fileinputstream_class = 
		(*env)->FindClass(env, "java/io/FileInputStream");
	    assert(fileinputstream_class);
	    string_constructor = 
		(*env)->GetMethodID(env, fileinputstream_class,
				    "<init>", "(Ljava/lang/String;)V");
	    assert(string_constructor);
	    fileinputstream_object = 
		(*env)->NewObject(env, fileinputstream_class,
				  string_constructor, file_name_string);
	    free(resource_name);
	    return fileinputstream_object;
	} else {
	    /* type is PR_FILE_OTHER or unknown */
	    fprintf(stderr,
		    "warning: classpath entry '%s' is not a file "
		    "or directory\n",
		    classpath_entries[i]);
	}
    }
    
#ifdef DEBUG_djm
    fprintf(stderr, "getSystemResourceAsStream0 done.\n");
#endif

    free(resource_name);
    /* no exception thrown, just return null */
    return 0;
}

jobject Java_java_lang_ClassLoader_defineClass0(JNIEnv *env,
						jobject class_loader,
						jobject class_name_string,
						jbyteArray class_byte_array,
						jint offset, jint length)
{
    unsigned char *class_name = water_getStringBytes(env, class_name_string);
    unsigned char *class_bytes;
    waterClass *new_class;

    if(!class_name) {
	return 0;
    }

    class_bytes = (*env)->GetByteArrayElements(env, class_byte_array, 0);
    if(!class_bytes) {
	return 0;
    }

    /* don't link yet, just load. */
    new_class =
	water_findClass(env, class_loader, 0, CLASS_LOADED, 
			&class_bytes[offset], length);
    if(!new_class) {
	return 0;
    }

    return water_findClassObjectClass(env, new_class);
}

void Java_java_lang_ClassLoader_resolveClass0(JNIEnv *env,
					      jobject class_loader,
					      jobject class_object)
{
    waterClass *class;

    GET_REFERENCE_MACHINE(class, 
			  &WATER_OBJECT(class_object)->data[0], 0);

    water_raiseState(env, class, CLASS_VERIFIED, 0, 0);
}

jobject Java_java_lang_Thread_currentThread(JNIEnv *env, jclass thread_class)
{
#ifdef DEBUG_djm
    fprintf(stderr, "currentThread() called.\n");
#endif
    return water_getThreadObject(env);
}

void Java_java_lang_Thread_yield(JNIEnv *env, jclass thread_class)
{
    PR_Sleep(PR_INTERVAL_NO_WAIT);
}

void Java_java_lang_Thread_sleep(JNIEnv *env, jclass thread_class, jlong msecs)
{
#ifdef DEBUG_djm
    fprintf(stderr, "thread with env %p sleeping for %Ld milliseconds.\n",
	    env, msecs);
#endif

    PR_Sleep(PR_MillisecondsToInterval((long)msecs));

#ifdef DEBUG_djm
    fprintf(stderr, "thread with env %p done sleeping for %Ld milliseconds.\n",
	    env, msecs);
#endif
}

void Java_java_lang_Thread_start(JNIEnv *env, jobject thread_object)
{
    water_JNIEnv *new_env;
    jfieldID privateinfo_field;

    new_env = (water_JNIEnv *)malloc(sizeof(water_JNIEnv));
    memcpy(new_env, env, sizeof(water_JNIEnv));
    assert(new_env);

    /* need to set the thread object to point to the new env */
    privateinfo_field = 
	(*env)->GetFieldID(env, 
			   (*env)->GetObjectClass(env, thread_object),
			   "PrivateInfo", "I");
    assert(privateinfo_field);
    (*env)->SetIntField(env, thread_object, privateinfo_field, (jint)new_env);

#ifdef DEBUG_djm
    fprintf(stderr, "storing in thread_object %p env pointer %p\n",
	    thread_object, new_env);
#endif

    new_env->thread_object = thread_object;
    water_createStack(&new_env->stack, 
		      &new_env->stack_top, &new_env->stack_end);
    new_env->opstack_top = new_env->stack_top;
    new_env->exception_thrown = 0;
    new_env->call_depth = 0;
    new_env->thread_status = THREAD_ALIVE;

    PR_CreateThread(PR_USER_THREAD, start_thread, new_env, PR_PRIORITY_NORMAL, 
		    PR_LOCAL_THREAD, PR_JOINABLE_THREAD, 0);
}

void Java_java_lang_Thread_setPriority0(JNIEnv *env, jobject thread_object,
					int priority)
{
    /* XXX just ignore it for now. */
}

jboolean Java_java_lang_Thread_isAlive(JNIEnv *env, jobject thread_object)
{
    water_JNIEnv *thread_env;

#ifdef DEBUG_djm
    fprintf(stderr, "isAlive() called.\n");
#endif

    thread_env = getThreadEnv(env, thread_object);
#ifdef DEBUG_djm
    fprintf(stderr, "thread object is %p.  got water_JNIEnv * of %p.\n",
	    thread_object, thread_env);
#endif
    if(thread_env == 0) {
	return JNI_FALSE;
    }

    if(thread_env->thread_status == THREAD_ALIVE) {
	return JNI_TRUE;
    } else {
	return JNI_FALSE;
    }
}

int Java_java_lang_Thread_countStackFrames(JNIEnv *env, jobject thread_object)
{
    water_JNIEnv *thread_env = getThreadEnv(env, thread_object);
    if(!thread_env) {
	return 0;
    }

    return thread_env->call_depth;
}

static water_JNIEnv *getThreadEnv(JNIEnv *env, jobject thread_object)
{
    jfieldID privateinfo_field;

    privateinfo_field = 
	(*env)->GetFieldID(env,
			   (*env)->GetObjectClass(env, thread_object),
			   "PrivateInfo", "I");
    if(!privateinfo_field) {
	return 0;
    }
    return (water_JNIEnv *)(*env)->GetIntField(env, thread_object, 
					       privateinfo_field);
}

static void start_thread(void *arg)
{
    water_JNIEnv *wenv = (water_JNIEnv *)arg;
    JNIEnv *env = (JNIEnv *)wenv;
    jmethodID run_method;

#ifdef DEBUG_djm
    fprintf(stderr, "starting thread with env %p--thread object is %p\n",
	    wenv, wenv->thread_object);
#endif

    run_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, wenv->thread_object),
			    "run", "()V");
    assert(run_method);
    (*env)->CallVoidMethod(env, wenv->thread_object, run_method);

#ifdef DEBUG_djm
    fprintf(stderr, "thread with env %p, thread object %p is done\n",
	    wenv, wenv->thread_object);
#endif

    PR_EnterMonitor(wenv->thread_object->monitor);
    wenv->thread_status = THREAD_COMPLETE;

    /* people trying to join this thread from Java will be waiting on */
    /* the thread object's monitor.                                   */
#ifdef DEBUG_djm
    fprintf(stderr, "notifying everybody on monitor %p\n",
	    wenv->thread_object->monitor);
#endif
    PR_NotifyAll(wenv->thread_object->monitor);
    PR_ExitMonitor(wenv->thread_object->monitor);
}

jobjectArray Java_java_util_ResourceBundle_getClassContext(JNIEnv *env, 
							   jclass bundle_class)
{
    jobjectArray class_array;

    /* I have no idea what is really supposed to be in this array.      */
    /* the only element that's actually accessed is the one at index 2, */
    /* which is a java.lang.Class object that gets getClassLoader()     */
    /* invoked on it.  so, any class loaded by the system class loader  */
    /* will work, I guess.  (we're a static method so we can't tell     */
    /* if we're being called from a class loaded by a different class   */
    /* loader anyway.)                                                  */

    /* actually this is apparently supposed to be a list of the classes */
    /* corresponding to the objects the methods for each stack frame    */
    /* are being called on.  so this is not quite right.                */
    class_array = water_newObjectArray(env, 3, 
				       WATER_ENV(env)->java_lang_class_class);
    if(!class_array) {
	return 0;
    }

    water_findClassObjectClass(env, bundle_class);
    STORE_REFERENCE_MACHINE(WATER_CLASS(bundle_class)->class_object,
			    ARRAY_ELEMENT_PTR(class_array, 2), 0);

    return class_array;
}

void Java_java_lang_Runtime_traceInstructions(JNIEnv *env, 
					      jobject runtime_object,
					      jboolean trace_instructions)
{
    /* we can trace instructions, but I don't want that code compiled */
    /* in in the optimized version, so we'll deny requests to do this */
    /* (we're allowed to).  if you really want to trace instructions, */
    /* compile with debugging and "export WATER_SHOWINST=1".          */
    
    /* does nothing */
}

void Java_java_lang_Runtime_traceMethodCalls(JNIEnv *env, 
					     jobject runtime_object,
					     jboolean trace_calls)
{
    /* see above comment */
    
    /* does nothing */
}

jobject Java_java_lang_Runtime_initializeLinkerInternal(JNIEnv *env,
							jobject runtime_object)
{
    /* as far as I can tell, this wants a copy of our LD_LIBRARY_PATH */
    /* in a string.                                                   */
    unsigned char *library_path = getenv("LD_LIBRARY_PATH");

    if(library_path) {
	return water_newStringFromBytes(env, library_path);
    } else {
	return water_newStringFromBytes(env, "");
    }
}

jint Java_java_lang_Runtime_loadFileInternal(JNIEnv *env,
					     jobject runtime_object,
					     jobject filename_string)
{
    /* it seems we need to return -1 for error, 1 for a successfully */
    /* loaded library, and 0 for library not found.                  */
    unsigned char *filename;
    int result;

    filename = water_getStringBytes(env, filename_string);
    if(!filename) {
	return -1;
    }

    result = water_loadNativeLibrary(WATER_ENV(env)->wvm, filename);
    free(filename);

    return (result < 0) ? 0 : 1;
}

jobject Java_java_lang_Runtime_buildLibName(JNIEnv *env,
					    jobject runtime_object,
					    jobject directory_string,
					    jobject libname_string)
{
    /* we have the directory and the library name.  we just need to */
    /* form "<directory>/lib<libname>.so".                          */
    jclass stringbuffer_class;
    jmethodID stringbuffer_string_constructor, append_method, tostring_method;
    jobject stringbuffer_object, result;
    waterObject *slashlib_string, *dotso_string;

    stringbuffer_class = (*env)->FindClass(env, "java/lang/StringBuffer");
    if(!stringbuffer_class) {
	assert(0);
	return 0;
    }
    stringbuffer_string_constructor =
	(*env)->GetMethodID(env, stringbuffer_class, 
			    "<init>", "(Ljava/lang/String;)V");
    if(!stringbuffer_string_constructor) {
	assert(0);
	return 0;
    }
    stringbuffer_object = 
	(*env)->NewObject(env, stringbuffer_class, 
			  stringbuffer_string_constructor,
			  directory_string);
    if(!stringbuffer_object) {
	assert(0);
	return 0;
    }
    append_method = 
	(*env)->GetMethodID(env, stringbuffer_class,
			    "append", 
			    "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    if(!append_method) {
	assert(0);
	return 0;
    }

    slashlib_string = water_newStringFromBytes(env, "/lib");
    if(!slashlib_string) {
	return 0;
    }
    dotso_string = water_newStringFromBytes(env, ".so");
    if(!dotso_string) {
	return 0;
    }

    (*env)->CallObjectMethod(env, stringbuffer_object, append_method,
			     slashlib_string);
    (*env)->CallObjectMethod(env, stringbuffer_object, append_method,
			     libname_string);
    (*env)->CallObjectMethod(env, stringbuffer_object, append_method,
			     dotso_string);

    tostring_method = (*env)->GetMethodID(env, stringbuffer_class,
					  "toString", "()Ljava/lang/String;");
    if(!tostring_method) {
	assert(0);
	return 0;
    }
    result = 
	(*env)->CallObjectMethod(env, stringbuffer_object, tostring_method);

    return result;
}
