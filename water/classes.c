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

#include "classes.h"
#include "nspr.h"
#include "byteops.h"
#include "utf8.h"
#include "call.h"
#include "objects.h"
#include "exception.h"
#include "verify.h"
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>

/* internal functions */
static int loadClassUsingLoader(JNIEnv *env, waterClass *class, 
				waterObject *class_loader, const char *name);
static int loadClassUsingDefaultLoader(JNIEnv *env, waterClass *class, 
				       const char *in_name);
waterClass *findSlot(JNIEnv *env, waterObject *class_loader, 
		     const char *in_name);
static char *convertDotsToSlashes(const char *in_name);
static int readFieldsOrMethods(JNIEnv *env, waterClass *class,
			       jshort *count, waterFieldOrMethod **array,
			       const unsigned char *buf, int buflen, int *pos);
static int readAttributes(JNIEnv *env, waterClass *class,
			  jshort *attributes_count, 
			  waterAttribute **attributes,
			  const unsigned char *buf, int buflen, int *pos);
static int parseAttributeContents(JNIEnv *env, waterClass *class, 
				  waterAttribute *attribute, int buflen);
static waterClass *newClass(JNIEnv *env, waterObject *class_loader,
			    const char *name);
static unsigned int hashClassName(const char *class_name);
static waterClass *existingClass(JNIEnv *env, const char *name);
static void addToClasses(JNIEnv *env, waterClass *class, const char *name);
static int loadClass(JNIEnv *env, waterClass *class, 
		     const unsigned char *buf, int buflen);
static int prepareClass(JNIEnv *env, waterClass *class);
static int initializeClass(JNIEnv *env, waterClass *class);
static waterCodeAttribute *findCodeAttribute(waterClass *class,
					     int num_attributes,
					     waterAttribute *attributes);
static waterField *findField(JNIEnv *env, waterClass *class, 
			     waterUTF8String *field_name);
static jshort getNextArgumentType(char **descriptor_p);
static char *mangledMethodName(JNIEnv *env, waterClass *class, 
			       waterMethod *method);
static char *mangleString(char *string);
static int isConstructor(waterMethod *method);
static waterConstantValueAttribute *
findConstantValueAttribute(waterClass *class,
			   int num_attributes,
			   waterAttribute *attributes);
static void setConstantValue(waterField *field, 
			     waterConstantValueAttribute *constant_value);


/* -------------------- JNI hooks -------------------- */

jclass water_jni_DefineClass(JNIEnv *env, jobject class_loader,
			     const jbyte *buf, jsize buflen)
{
    waterClass *class, *new_class;
    
    /* this is a little strange.  first we load the class into a new class */
    /* structure.  then we need to find the real class structure, if there */
    /* was one, and overwrite it with the new one (if it's from the same   */
    /* class loader).                                                      */

    new_class = newClass(env, class_loader, 0);
    if(!new_class) {
	return 0;
    }
    if(loadClass(env, class, buf, buflen) < 0) {
	water_freeClass(new_class);
	return 0;
    }

    class = existingClass(env, new_class->name);
    if(class) {
	if(class->class_loader != class_loader) {
	    /* I think this is an error--redefining a class already loaded */ 
	    /* by a different loader.  but the spec doesn't say anything   */
	    /* about this situation.  hmm...                               */
	    return 0;
	}	
    } else {
	class = findSlot(env, class_loader, new_class->name);
	if(!class) {
	    return 0;
	}
    }

    memcpy(&class, &new_class, sizeof(waterClass));

    return class;
}

jclass water_jni_FindClass(JNIEnv *env, const char *name)
{
    return water_findClass(env, 0, name, CLASS_VERIFIED, 0, 0);
}

jclass water_jni_GetSuperclass(JNIEnv *env, jclass clazz)
{
    return WATER_CLASS(clazz)->resolved_superclass;
}

jboolean water_jni_IsAssignableFrom(JNIEnv *env, jclass clazz1, jclass clazz2)
{
    return water_castIsOK(WATER_ENV(env),
			  WATER_CLASS(clazz1), WATER_CLASS(clazz2)) ?
	JNI_TRUE : JNI_FALSE;
}

jmethodID water_jni_GetMethodID(JNIEnv *env, jclass clazz,
				const char *name, const char *sig)
{
    waterMethod *result;
    waterUTF8String *method_name;
    waterUTF8String *method_descriptor;

    method_name = water_NormalToUTF8(name);
    method_descriptor = water_NormalToUTF8(sig);

    result = water_virtualMethodLookup(WATER_CLASS(clazz), 
				       method_name, method_descriptor);

    water_freeUTF8String(method_name);
    water_freeUTF8String(method_descriptor);

    if(result && (result->access_flags & ACC_STATIC)) {
	/* EXCEPTION: should not be static */
	fprintf(stderr, 
		"got a static method when a non-static method was desired.\n");
	return 0;
    }

    return result;    
}


jmethodID water_jni_GetStaticMethodID(JNIEnv *env, jclass clazz, 
				      const char *name, const char *sig)
{
    waterMethod *result;
    waterUTF8String *method_name;
    waterUTF8String *method_descriptor;

    method_name = water_NormalToUTF8(name);
    method_descriptor = water_NormalToUTF8(sig);

    result = water_virtualMethodLookup(WATER_CLASS(clazz), 
				       method_name, method_descriptor);
    water_freeUTF8String(method_name);
    water_freeUTF8String(method_descriptor);

    /* EXCEPTION: make sure it _is_ static */
    if(result && !(result->access_flags & ACC_STATIC)) {
	fprintf(stderr, 
		"got a non-static method when a static method was desired.\n");
	return 0;
    }

    return result;
}

jfieldID water_jni_GetFieldID(JNIEnv *env, jclass clazz,
			      const char *name, const char *sig)
{
    waterUTF8String *utf8_name = water_NormalToUTF8(name);
    jfieldID result;

    assert(clazz);

    result = findField(env, (waterClass *)clazz, utf8_name);

    water_freeUTF8String(utf8_name);

    return result;
}

jfieldID water_jni_GetStaticFieldID(JNIEnv *env, jclass clazz,
				    const char *name, const char *sig)
{
    /* exactly the same as GetFieldID */
    return water_jni_GetFieldID(env, clazz, name, sig);
}

jclass water_jni_GetObjectClass(JNIEnv *env, jobject object)
{
    assert(object);
    return(((waterObject *)object)->class);
}


waterClass *water_findClass(JNIEnv *env, waterObject *class_loader,
			    const char *name, int desired_state,
			    const char *buf, int buflen)
{
    waterClass *class;

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "looking for class '%s'...\n", name);
#endif

    /* handle the weird array classes differently */
    if(name[0] == '[') {
	return water_getArrayClassByName(env, name, 
					 class_loader, desired_state);
    }

    class = findSlot(env, class_loader, name);
    if(!class) {
	return 0;
    }

    if(water_raiseState(env, class, desired_state, buf, buflen) < 0) {
	return 0;
    }

    return class;
}

int water_raiseState(JNIEnv *env, waterClass *class, int desired_state,
		     const char *buf, int buflen)
{
    assert(class);

    /* if the class is in an error state, we can't do anything with it */
    /* anymore.                                                        */
    if(class->state < 0) {
	water_throwException(WATER_ENV(env), 
			     "java/lang/ClassNotFoundException");
	return -1;
    }

    /* do a quick check so we can bail out quickly when the class is already */
    /* in the proper state.                                                  */
    if(class->state >= desired_state) {
	return 0;
    }

    if(desired_state >= CLASS_LOADED && class->state < CLASS_LOADED) {
	if(buf) {
	    /* we already have the data for the class--just load it. */
	    if(loadClass(env, class, buf, buflen) < 0) {
		return -1;
	    }
	} else {
	    /* we need to find the class using the (maybe default) */
	    /* class loader.                                       */
	    /*	    assert(name); */
	    if(!class->name) {
		abort();
	    }
	    if(loadClassUsingLoader(env, class, 
				    class->class_loader, class->name) < 0) {
		return -1;
	    }
	}
	
	assert(class->state == CLASS_LOADED);
    }

    if(desired_state >= CLASS_PREPARED && class->state < CLASS_PREPARED) {
	if(prepareClass(env, class) < 0) {
	    return -1;
	}

	assert(class->state == CLASS_PREPARED);
    }

    if(desired_state >= CLASS_VERIFIED && class->state < CLASS_VERIFIED) {
	if(water_verifyClass(env, class) < 0) {
	    return -1;
	}

	assert(class->state == CLASS_VERIFIED);
    }

    if(desired_state >= CLASS_INITIALIZED && 
       class->state < CLASS_INITIALIZED) {
	if(initializeClass(env, class) < 0) {
	    return -1;
	}

	/* this may have been a recursive call for initialization, so  */
	/* we can't be sure at this point that the class has completed */
	/* initialization.                                             */
    }

    return 0;
}

static int loadClassUsingLoader(JNIEnv *env, waterClass *class, 
				waterObject *class_loader, const char *name)
{
    jmethodID load_class_method;
    waterObject *class_name;

    PR_EnterMonitor(class->monitor);
    if(class_loader) {
	load_class_method = 
	    (*env)->GetMethodID(env, (*env)->GetObjectClass(env, class_loader),
				"loadClass", 
				"(Ljava/lang/String;)Ljava/lang/Class;");
	if(!load_class_method) {
	    PR_ExitMonitor(class->monitor);
	    class->state = CLASS_LOADING_FAILED;
	    return -1;
	}
	class_name = water_newStringFromBytes(env, name);
	if(!class_name) {
	    PR_ExitMonitor(class->monitor);
	    class->state = CLASS_LOADING_FAILED;
	    return -1;
	}
	(*env)->CallObjectMethod(env, class_loader, 
				 load_class_method, class_name);
	if(class->state != CLASS_LOADED) {
	    PR_ExitMonitor(class->monitor);
	    class->state = CLASS_LOADING_FAILED;
	    return -1;
	}
    } else {
	if(loadClassUsingDefaultLoader(env, class, name) < 0) {
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
    }
    assert(class->state == CLASS_LOADED);
    PR_ExitMonitor(class->monitor);
    return 0;
}

static int loadClassUsingDefaultLoader(JNIEnv *env, waterClass *class, 
				       const char *in_name)
{
    char **classpath_entries;
    char *name;
    int i;

    /* this is the default loader--search the CLASSPATH, etc. */
    name = convertDotsToSlashes(in_name);

    classpath_entries = water_getClassPathEntries(env);
    if(!classpath_entries)
	goto fail;
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
	    /* check for .zip or .jar file */
	} else if(info.type == PR_FILE_DIRECTORY) {
	    PRFileDesc *pr_fd;
	    PRInt32 bytes_read = 0;
	    char *buf;
	    
	    char *class_file_name = 
		(char *)malloc(strlen(classpath_entries[i]) + 
			       1 +                           /* '/' */
			       strlen(name) + 
			       6 +                           /* ".class" */ 
			       1                             /* '\0' */);
	    if(!class_file_name) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		free(name);
		return -1;
	    }
	    
	    strcpy(class_file_name, classpath_entries[i]);
	    strcat(class_file_name, "/");
	    strcat(class_file_name, name);
	    strcat(class_file_name, ".class");
	    
	    if(PR_GetFileInfo(class_file_name, &info) != PR_SUCCESS) {
		/* not an error, it's just not here. */
		continue;
	    }
	    
	    if((pr_fd = PR_Open(class_file_name, PR_RDONLY, 0)) == NULL) {
		fprintf(stderr, 
			"warning: saw class file '%s' but couldn't open it\n",
			class_file_name);
		continue;
	    }
	    
#ifdef DEBUG_djm
	    fprintf(stderr, "reading class '%s' from '%s'\n",
		    name, class_file_name);
#endif
	    
	    buf = (char *)malloc(info.size);
	    if(!buf) {
		fprintf(stderr, "warning: couldn't allocate memory\n");
		continue;
	    }
	    
	    while(bytes_read < info.size) {
		int result = PR_Read(pr_fd, buf + bytes_read, 
				     info.size - bytes_read);
		
		if(result <= 0) {
		    fprintf(stderr, 
			    "warning: error reading class file '%s'\n",
			    class_file_name);
		    break;
		}
		
		bytes_read += result;
	    }
	    
	    if(bytes_read != info.size) {
		fprintf(stderr, 
			"warning: couldn't read class file '%s' completely\n",
			class_file_name);
		continue;
	    }
	    
	    PR_Close(pr_fd);
	    
	    /* classloader is null--loaded by system */
	    if(loadClass(env, class, buf, info.size) < 0) {
		free(buf);
		free(name);
		return -1;
	    } else {
		free(buf);
		free(name);
		return 0;
	    }
	} else {
	    /* type is PR_FILE_OTHER or unknown */
	    fprintf(stderr,
		    "warning: classpath entry '%s' is not a file "
		    "or directory\n",
		    classpath_entries[i]);
	}
    }

 fail:
    water_throwException(WATER_ENV(env), "java/lang/ClassNotFoundException");
    class->state = CLASS_LOADING_FAILED;
    free(name);
    return -1;
}

waterClass *findSlot(JNIEnv *env, waterObject *class_loader, 
		     const char *in_name)
{
    unsigned char *name;
    waterClass *class;

    /* convert dots to slashes if necessary */
    name = convertDotsToSlashes(in_name);

    PR_EnterMonitor(WATER_ENV(env)->wvm->global_class_monitor);
    class = existingClass(env, name);
    if(!class) {
	class = newClass(env, class_loader, name);
	if(!class) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    free(name);
	    return 0;
	}
	addToClasses(env, class, name);
    }
    free(name);
    PR_ExitMonitor(WATER_ENV(env)->wvm->global_class_monitor);

    return class;
}

static char *convertDotsToSlashes(const char *in_name)
{
    char *name;
    int i;

    name = strdup(in_name);
    for(i = 0; i < strlen(name); i++) {
	if(name[i] == '.') {
	    name[i] = '/';
	}
    }

    return name;
}

char **water_getClassPathEntries(JNIEnv *env)
{
    WATER_ENV_DECL(wenv);
    int i;

    if(!wenv->classpath_entries) {
	wenv->tokenized_classpath = strdup(wenv->classpath);
	if(!wenv->tokenized_classpath) {
	    fprintf(stderr, "warning: couldn't allocate memory\n");
	    return 0;
	}

	wenv->classpath_entries = 
	    (char **)calloc(255, sizeof(char *) /* XXX don't use fixed size */);

	i = 0;
	wenv->classpath_entries[i] = strsep(&wenv->tokenized_classpath, ":");
	while((wenv->classpath_entries[++i] = 
	       strsep(&wenv->tokenized_classpath, ":")) != 0) {}
    }

    return wenv->classpath_entries;
}


/* -------------------- internal routines -------------------- */

static int loadClass(JNIEnv *env, waterClass *class, 
		     const unsigned char *buf, int buflen)
{
    int pos, i;
    jushort length;

    PR_EnterMonitor(class->monitor);
    if(class->state != CLASS_FETUS) {
	/* we're in a bad state for loading. */
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    class->state = CLASS_PARTIALLY_LOADED;

    /* here we parse out a Java .class file. */
    pos = 0;

    /* check the magic--0xcafebabe */
    if(buflen < 4 ||
       buf[pos++] != 0xca ||
       buf[pos++] != 0xfe ||
       buf[pos++] != 0xba ||
       buf[pos++] != 0xbe) {
	water_throwException(WATER_ENV(env), "java/lang/ClassFormatError");
	class->state = CLASS_LOADING_FAILED;
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    /* read version numbers */
    GET_SHORT(class->minor_version, buf, buflen, pos);
    GET_SHORT(class->major_version, buf, buflen, pos);

    /* now the constant pool. */
    GET_SHORT(class->constant_pool_count, buf, buflen, pos);

    /* the zeroth constant pool entry is "reserved for VM internal use", */
    /* so we need one extra.                                             */
    class->constant_pool = 
	(waterConstantPoolEntry *)calloc(class->constant_pool_count + 1,
					 sizeof(waterConstantPoolEntry));
    if(!class->constant_pool) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	class->state = CLASS_LOADING_FAILED;
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    /* start at 1 here.  the unused zeroth entry is, however, included */
    /* in the count.                                                   */
    for(i = 1; i < class->constant_pool_count; i++) {
	waterConstantPoolEntry *constant = &class->constant_pool[i];

	/* first we have a one-byte tag giving the type. */
	GET_BYTE(constant->tag, buf, buflen, pos);

	/* and now the actual data. */
	switch(constant->tag) {
	case CONSTANT_CLASS_TAG:
	    /* a reference to a class, expressed as the index of the class's */
	    /* name in the constant pool.                                    */
	    GET_SHORT(constant->value.class.class_index, buf, buflen, pos);
	    break;

	case CONSTANT_FIELDREF_TAG:
	case CONSTANT_METHODREF_TAG:
	case CONSTANT_INTERFACE_METHODREF_TAG:
	    /* these all have the same format--first a pool index for the    */
	    /* class name, then a pool index for the name-and-type entry.    */
	    GET_SHORT(constant->value.reference.class_index, buf, buflen, pos);
	    GET_SHORT(constant->value.reference.name_and_type_index, 
		      buf, buflen, pos);
	    break;

	case CONSTANT_STRING_TAG:
	    /* a reference to a string. */
	    GET_SHORT(constant->value.string.string_index, buf, buflen, pos);
	    break;

	case CONSTANT_INTEGER_TAG:
	    /* an integer. */
	    GET_INT(constant->value.int_value, buf, buflen, pos);
	    break;

	case CONSTANT_FLOAT_TAG:
	    /* a float. */
	    GET_FLOAT(constant->value.float_value, buf, buflen, pos);
	    break;

	case CONSTANT_LONG_TAG:
	    /* a long. */
	    GET_LONG(constant->value.long_value, buf, buflen, pos);
	    break;

	case CONSTANT_DOUBLE_TAG:
	    /* a double. */
	    GET_DOUBLE(constant->value.double_value, buf, buflen, pos);
	    break;

	case CONSTANT_NAMEANDTYPE_TAG:
	    /* a pair--a name and a descriptor, both indexes into the */
	    /* constant pool. */
	    GET_SHORT(constant->value.name_and_type.name_index, 
		      buf, buflen, pos);
	    GET_SHORT(constant->value.name_and_type.descriptor_index, 
		      buf, buflen, pos);
	    break;

	case CONSTANT_UTF8_TAG:
	    /* a string, in UTF8 format. */
	    GET_SHORT(length, buf, buflen, pos);
	    if((pos + length) > (buflen + 1)) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/ClassFormatError");
		return -1;
	    }
	    water_UTF8SetValue(&constant->value.utf8_value,
			       length, buf+pos);
	    pos += length;
	    break;

	default:
	    /* unrecognized constant pool entry -- not allowed */
	    water_throwException(WATER_ENV(env), "java/lang/ClassFormatError");
	    class->state = CLASS_LOADING_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}

	/* longs and doubles take two constant pool slots, the second of */
	/* which is not used.  so skip over it.                          */
	if(constant->tag == CONSTANT_DOUBLE_TAG ||
	   constant->tag == CONSTANT_LONG_TAG)
	    i++;
    }

    /* access flags, class name, and superclass */
    GET_SHORT(class->access_flags, buf, buflen, pos);
    GET_SHORT(class->this_class, buf, buflen, pos);
    GET_SHORT(class->super_class, buf, buflen, pos);

    /* make a copy of the class's name as a regular C string for */
    /* convenience/speed.                                        */
    class->name = 
	water_UTF8ToNormal(water_getClassName(class, class->this_class));

    /* now we have interfaces. */
    /* each one is just the index of a "Class" constant in the pool. */
    GET_SHORT(class->interfaces_count, buf, buflen, pos);
    class->interfaces = (jshort *)calloc(class->interfaces_count,
					 sizeof(jshort)); 
    if(!class->interfaces) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	class->state = CLASS_LOADING_FAILED;
	PR_ExitMonitor(class->monitor);
	return -1;
    }
    for(i = 0; i < class->interfaces_count; i++) 
	GET_SHORT(class->interfaces[i], buf, buflen, pos);

    /* now fields and methods. */
    /* they're basically the same, so abstract out. */
    if(readFieldsOrMethods(env, class, &class->fields_count, &class->fields,
			   buf, buflen, &pos) < 0) {
#ifdef DEBUG_djm
	fprintf(stderr, "warning: error reading fields from class file\n");
#endif
	return -1;
    }

    if(readFieldsOrMethods(env, class, &class->methods_count, &class->methods,
			   buf, buflen, &pos) < 0) {
#ifdef DEBUG_djm
	fprintf(stderr, "warning: error reading methods from class file\n");
#endif
	return -1;
    }

    if(readAttributes(env, class, 
		      &class->attributes_count, &class->attributes, 
		      buf, buflen, &pos) < 0) {
#ifdef DEBUG_djm
	fprintf(stderr, "warning: error reading attributes from class file\n");
#endif
	return -1;
    }

    /* load the superclass */
    if(class->super_class) { 
	/* only java.lang.Object has 0 here, i.e. no superclass */
	class->resolved_superclass = 
	    water_resolveClassByIndex(env, class, class->super_class,
				      CLASS_LOADED);
	if(class->resolved_superclass) {
	    if(class->resolved_superclass->state == CLASS_PARTIALLY_LOADED) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/ClassCircularityError");
		class->state = CLASS_LOADING_FAILED;
		PR_ExitMonitor(class->monitor);
		return -1;
	    }
	} else {
	    /* exception thrown already */
	    class->state = CLASS_LOADING_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
    }

    /* make sure we're exactly at the end of the class */
    if(pos != buflen) {
#ifdef DEBUG_djm
	fprintf(stderr, 
		"warning: didn't make it to the end of the class: "
		"pos is %d, buflen is %d\n", pos, buflen);
#endif
	water_throwException(WATER_ENV(env), "java/lang/ClassFormatError");
	class->state = CLASS_LOADING_FAILED;
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    class->state = CLASS_LOADED;
    PR_ExitMonitor(class->monitor);
    return 0;
}

static int readFieldsOrMethods(JNIEnv *env, waterClass *class,
			       jshort *count, waterFieldOrMethod **array,
			       const unsigned char *buf, int buflen, int *pos)
{
    int i;

    GET_SHORT(*count, buf, buflen, *pos);
    *array = 
	(waterFieldOrMethod *)calloc(*count,
				     sizeof(waterFieldOrMethod));
    if(!*array) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }
    for(i = 0; i < *count; i++) {
	waterFieldOrMethod *field = &(*array)[i];
	
	GET_SHORT(field->access_flags, buf, buflen, *pos);
	GET_SHORT(field->name_index, buf, buflen, *pos);
	GET_SHORT(field->descriptor_index, buf, buflen, *pos);
	
	if(readAttributes(env, class, 
			  &field->attributes_count, 
			  &field->attributes, 
			  buf, buflen, pos) < 0) {
#ifdef DEBUG_djm
	    fprintf(stderr, 
		    "warning: error reading attributes from class file\n");
#endif
	    /* exception thrown already */
	    return -1;
	}
    }

    return 0;
}

static int readAttributes(JNIEnv *env, waterClass *class, 
			  jshort *attributes_count, 
			  waterAttribute **attributes,
			  const unsigned char *buf, int buflen, int *pos)
{
    int i;

    GET_SHORT(*attributes_count, buf, buflen, *pos);
    *attributes = (waterAttribute *)calloc(*attributes_count,
					   sizeof(waterAttribute));
    if(!*attributes) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }
    
    for(i = 0; i < *attributes_count; i++) {
	GET_SHORT((*attributes)[i].name_index, buf, buflen, *pos);
	GET_INT((*attributes)[i].length, buf, buflen, *pos);
	(*attributes)[i].contents = 
	    (unsigned char *)malloc((*attributes)[i].length);
	if(!(*attributes)[i].contents) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    return -1;
	} else {
	    memcpy((*attributes)[i].contents, buf + *pos, 
		   (*attributes)[i].length);
	    if(parseAttributeContents(env, class, &(*attributes)[i], 
				      (*attributes)[i].length) < 0) {
		return -1;
	    }
	}
	*pos += (*attributes)[i].length;
    }

    return 0;
}

static int parseAttributeContents(JNIEnv *env, waterClass *class,
				  waterAttribute *attribute, int buflen)
{
    static waterUTF8String *sourcefile_name = 0, *constantvalue_name = 0,
	*code_name = 0, *exceptions_name = 0, *linenumbertable_name = 0,
	*localvariabletable_name = 0;
    waterUTF8String *attribute_name;
    int pos = 0;

    if(!sourcefile_name) {
	/* initialize the constant strings */
	sourcefile_name = water_NormalToUTF8("SourceFile");
	constantvalue_name = water_NormalToUTF8("ConstantValue");
	code_name = water_NormalToUTF8("Code");
	exceptions_name = water_NormalToUTF8("Exceptions");
	linenumbertable_name = water_NormalToUTF8("LineNumberTable");
	localvariabletable_name = water_NormalToUTF8("LocalVariableTable");
    }

    attribute_name = water_getUTF8Constant(class, attribute->name_index);
    if(water_UTF8Compare(sourcefile_name, attribute_name) == 0) {
	GET_SHORT(
            attribute->parsed_contents.source_file.sourcefile_index,
	    attribute->contents, buflen, pos);
    } else if(water_UTF8Compare(constantvalue_name, attribute_name) == 0) {
	GET_SHORT(
            attribute->parsed_contents.constant.constantvalue_index,
	    attribute->contents, buflen, pos);
    } else if(water_UTF8Compare(code_name, attribute_name) == 0) {
	waterCodeAttribute *code = &attribute->parsed_contents.code;
	int i;

	GET_SHORT(code->max_stack, attribute->contents, buflen, pos);
	GET_SHORT(code->max_locals, attribute->contents, buflen, pos);
	GET_INT(code->code_length, attribute->contents, buflen, pos);
	code->code = &attribute->contents[pos];
	pos += code->code_length;
	GET_SHORT(code->exception_table_length, attribute->contents, 
		  buflen, pos);
	code->exception_table = 
	    (waterExceptionInfo *)calloc(code->exception_table_length,
					 sizeof(waterExceptionInfo));
	for(i = 0; i < code->exception_table_length; i++) {
	    GET_SHORT(code->exception_table[i].start_pc, 
		      attribute->contents, buflen, pos);
	    GET_SHORT(code->exception_table[i].end_pc, 
		      attribute->contents, buflen, pos);
	    GET_SHORT(code->exception_table[i].handler_pc, 
		      attribute->contents, buflen, pos);
	    GET_SHORT(code->exception_table[i].catch_type, 
		      attribute->contents, buflen, pos);
	}

	if(readAttributes(env, class, 
			  &code->attributes_count, &code->attributes,
			  attribute->contents, buflen, &pos) < 0) {
	    fprintf(stderr, 
		    "reading attributes of a code attribute failed.\n");
	    return -1;
	}
    } else if(water_UTF8Compare(exceptions_name, attribute_name) == 0) {
	waterExceptionsAttribute *exceptions =
	    &attribute->parsed_contents.exceptions;
	int i;

	GET_SHORT(exceptions->number_of_exceptions, attribute->contents, 
		  buflen, pos);
	exceptions->exception_index_table = 
	    (jshort *)calloc(exceptions->number_of_exceptions, sizeof(jshort));
	for(i = 0; i < exceptions->number_of_exceptions; i++) {
	    GET_SHORT(exceptions->exception_index_table[i],
		      attribute->contents, buflen, pos);
	}
    } else if(water_UTF8Compare(linenumbertable_name, attribute_name) == 0) {
	waterLineNumberTableAttribute *line_number =
	    &attribute->parsed_contents.line_numbers;
	int i;

	GET_SHORT(line_number->line_number_table_length,
		  attribute->contents, buflen, pos);
	line_number->line_number_table = 
	    (waterLineNumberInfo *)
	    calloc(line_number->line_number_table_length, 
		   sizeof(waterLineNumberInfo));
	for(i = 0; i < line_number->line_number_table_length; i++) {
	    GET_SHORT(line_number->line_number_table[i].start_pc,
		      attribute->contents, buflen, pos);
	    GET_SHORT(line_number->line_number_table[i].line_number,
		      attribute->contents, buflen, pos);
	}
    } else if(water_UTF8Compare(localvariabletable_name, 
				attribute_name) == 0) {
	waterLocalVariableTableAttribute *local_variable =
	    (waterLocalVariableTableAttribute *)
	    &attribute->parsed_contents.local_variables;
	int i;

	GET_SHORT(local_variable->local_variable_table_length,
		  attribute->contents, buflen, pos);
	local_variable->local_variable_table =
	    (waterLocalVariableInfo *)
	    calloc(local_variable->local_variable_table_length,
		   sizeof(waterLocalVariableInfo));
	for(i = 0; i < local_variable->local_variable_table_length; i++) {
	    GET_SHORT(local_variable->local_variable_table[i].start_pc,
		      attribute->contents, buflen, pos);
	    GET_SHORT(local_variable->local_variable_table[i].length,
		      attribute->contents, buflen, pos);
	    GET_SHORT(local_variable->local_variable_table[i].name_index,
		      attribute->contents, buflen, pos);
	    GET_SHORT(local_variable->local_variable_table[i].
		      descriptor_index,
		      attribute->contents, buflen, pos);
	    GET_SHORT(local_variable->local_variable_table[i].index,
		      attribute->contents, buflen, pos);
	}
    }
    /* other attributes just don't parse */

    /* if we parsed something, make sure it was the right length. */
    if(pos != 0 && pos != buflen) {
#ifdef DEBUG_djm
	fprintf(stderr, "warning: attribute had the wrong length.\n");
#endif
	water_throwException(WATER_ENV(env), "java/lang/ClassFormatError");
	return -1;
    }

    return 0;
}

static waterClass *newClass(JNIEnv *env, waterObject *class_loader, 
			    const char *name)
{
    waterClass *class = (waterClass *)calloc(1, sizeof(waterClass));

    if(!class) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return 0;
    }

    /* initialize that which must be initialized */
    class->state = CLASS_FETUS;
    class->monitor = PR_NewMonitor();
    class->class_loader = class_loader;
    class->name = strdup(name);

    return class;
}

void water_freeClass(waterClass *class)
{
#if 0
    /* XXX this is pretty screwed up right now.                 */
    /* classes are not freed unless we're shutting down anyway. */

    int i, j;

    if(class->constant_pool)
	for(i = 1; i < class->constant_pool_count; i++)
	    if(class->constant_pool[i].tag == CONSTANT_UTF8_TAG)
		/* XXX free it somehow */;

    if(class->fields) {
	for(i = 0; i < class->fields_count; i++)
	    if(class->fields[i].attributes) {
		for(j = 0; j < class->fields[i].attributes_count; i++)
		    if(class->fields[i].attributes[j].contents)
			free(class->fields[i].attributes[j].contents);
		free(class->fields[i].attributes);
	    }
	free(class->fields);
    }

    if(class->methods) {
	for(i = 0; i < class->methods_count; i++)
	    if(class->methods[i].attributes) {
		for(j = 0; j < class->methods[i].attributes_count; j++)
		    if(class->methods[i].attributes[j].contents)
			free(class->methods[i].attributes[j].contents);
		free(class->methods[i].attributes);
	    }
	free(class->methods);
    }

    if(class->attributes) {
	for(j = 0; j < class->attributes_count; j++)
	    if(class->attributes[j].contents)
		free(class->attributes[j].contents);
	free(class->attributes);
    }

    free(class);
#endif
}

waterClass *water_resolveClassReference(JNIEnv *env,
					waterClass *class, 
					waterConstantReference *ref)
{
    if(ref->resolved.class)
	return ref->resolved.class;

    ref->resolved.class =
	water_resolveClassByIndex(env, class, ref->class_index,
				  CLASS_VERIFIED);

    return ref->resolved.class;
}

waterClass *water_resolveClassByIndex(JNIEnv *env, waterClass *class, 
				      jshort class_index, int desired_state)
{
    char *class_name;
    waterClass *resolved_class;

    assert(env != 0);
    assert(class != 0);
    assert(class->constant_pool[class_index].tag == CONSTANT_CLASS_TAG);

    class_name = water_UTF8ToNormal(water_getClassName(class, class_index));
    resolved_class = water_findClass(env, class->class_loader, class_name,
				     desired_state, 0, 0);
    free(class_name);

    return resolved_class;
}


waterMethod *water_resolveMethodReference(JNIEnv *env,
					  waterClass *class, 
					  waterConstantReference *ref)
{
    waterUTF8String *method_name;
    waterUTF8String *method_descriptor;

    if(ref->resolved.method)
	return ref->resolved.method;

    if(!ref->resolved.class)
	water_resolveClassReference(env, class, ref);
    /* EXCEPTION */
    if(!ref->resolved.class)
	return 0;

    method_name = 
	water_getUTF8Constant(class, 
			      class->constant_pool[ref->name_and_type_index].
			      value.name_and_type.name_index);
    method_descriptor = 
	water_getUTF8Constant(class, 
			      class->constant_pool[ref->name_and_type_index].
			      value.name_and_type.descriptor_index);
    ref->resolved.method = 
	water_virtualMethodLookup(ref->resolved.class, 
				  method_name, method_descriptor);
    if(!ref->resolved.method) {
	/* EXCEPTION: no method found */
	char *method_name_normal = water_UTF8ToNormal(method_name);
	char *descriptor_name_normal = water_UTF8ToNormal(method_descriptor);
	waterUTF8String *class_name = 
	    water_getUTF8Constant(class, class->this_class);
	char *class_name_normal = water_UTF8ToNormal(class_name);

#ifdef DEBUG_djm
	fprintf(stderr,
		"no method '%s' with descriptor '%s' found in class '%s'\n",
		method_name_normal, descriptor_name_normal, class_name_normal);
#endif

	free(method_name_normal);
	free(descriptor_name_normal);
	free(class_name_normal);
	return 0;
    }

    /* initialize the class if necessary */
    /* but only if the method is actually declared in its own class,  */
    /* not inherited, or if it is a constructor.                      */

    /* just use the method's class pointer for the inheritance stuff. */
    /* if the method was inherited unmodified, the class pointer      */
    /* will still point to the defining class, which is what we want. */
    /* if the method was overridden, the class pointer will point     */
    /* to the child class, but initializing it will initialize all    */
    /* of its superclasses anyway.                                    */

    /* the only problem is dealing with inherited, non-overridden     */
    /* constructors.  in this case we do need to find the deepest     */
    /* child and initialize it.                                       */
    if(water_raiseState(env, ref->resolved.method->class, 
			CLASS_INITIALIZED, 0, 0) < 0) {
	return 0;
    }
    
    if(isConstructor(ref->resolved.method)) {
	if(water_raiseState(env, ref->resolved.method->childmost_class, 
			    CLASS_INITIALIZED, 0, 0) < 0) {
	    return 0;
	}
    }

    return ref->resolved.method;
}

static int isConstructor(waterMethod *method)
{
    static waterUTF8String *init_name = 0;

    if(!init_name) {
	init_name = water_NormalToUTF8("<init>");
    }

    if(water_UTF8Compare(method->name, init_name) == 0) {
	return 1;
    } else {
	return 0;
    }
}

waterField *water_resolveFieldReference(JNIEnv *env, waterClass *class,
					waterConstantReference *ref)
{
    if(ref->resolved.field)
	return ref->resolved.field;

    if(!ref->resolved.class)
	water_resolveClassReference(env, class, ref);
    /* EXCEPTION: check for one */
    if(!ref->resolved.class)
	return 0;

    ref->resolved.field = 
	findField(env, ref->resolved.class, 
   	    water_getUTF8Constant(class,
 			 class->constant_pool[ref->name_and_type_index].
				  value.name_and_type.name_index));
    /* EXCEPTION: unknown field */

    /* initialize class if necessary */
    /* necessary for all field accesses except for reading */
    /* constant static final fields.                       */
    /* we initialize the class that declared the field even if we're */
    /* accessing it from a class that inherited it.                  */
    if(ref->resolved.field &&
       !((ref->resolved.field->access_flags & ACC_STATIC) &&
	 (ref->resolved.field->access_flags & ACC_FINAL) &&
	 (ref->resolved.field->constant_value_initialized))) {
	if(water_raiseState(env, ref->resolved.field->class, 
			    CLASS_INITIALIZED, 0, 0) < 0) {
	    return 0;
	}
    }

    return ref->resolved.field;
}

static waterField *findField(JNIEnv *env, waterClass *class, 
			     waterUTF8String *field_name)
{
    int i;

    for(i = 0; i < class->num_resolved_fields; i++)
	if(water_UTF8Compare(class->resolved_fields[i].name, 
			     field_name) == 0)
	    return &class->resolved_fields[i];

    return 0;
}

waterUTF8String *water_getClassName(waterClass *class, jshort pool_index)
{
    return 
	water_getUTF8Constant(class,
            class->constant_pool[pool_index].value.class.class_index);
}


waterUTF8String *water_getUTF8Constant(waterClass *class, jshort pool_index)
{
    assert(class->constant_pool[pool_index].tag == CONSTANT_UTF8_TAG);

    return &class->constant_pool[pool_index].value.utf8_value;
}


static unsigned int hashClassName(const char *class_name)
{
    unsigned int result = 0, i;
    
    /* XXX I just made this up.  replace with a good hash function. */
    for(i = 0; i < strlen(class_name); i++) {
	result *= 217;
	result += class_name[i];
    }

    return result;
}

static waterClass *existingClass(JNIEnv *env, const char *name)
{
    unsigned int hash_bucket = hashClassName(name) % CLASS_HASH_TABLE_SIZE;
    waterClass *class;

    PR_EnterMonitor(WATER_ENV(env)->wvm->global_class_monitor);
    class = WATER_ENV(env)->wvm->loaded_classes[hash_bucket];
    while(class) {
	if(strcmp(class->name, name) == 0) {
	    PR_ExitMonitor(WATER_ENV(env)->wvm->global_class_monitor);
	    return class;
	}
	class = class->next;
    }

    /* not found. */
    PR_ExitMonitor(WATER_ENV(env)->wvm->global_class_monitor);
    return 0;
}

static void addToClasses(JNIEnv *env, waterClass *class, const char *name)
{
    unsigned int hash_bucket = 
	hashClassName(name) % CLASS_HASH_TABLE_SIZE;
    class->next = WATER_ENV(env)->wvm->loaded_classes[hash_bucket];
    WATER_ENV(env)->wvm->loaded_classes[hash_bucket] = class;
}

static int prepareClass(JNIEnv *env, waterClass *class)
{
    int i, j;
    waterUTF8String *class_name;
    char *class_name_normal;

    PR_EnterMonitor(class->monitor);
    if(class->state != CLASS_LOADED) {
#ifdef DEBUG_djm
	fprintf(stderr, 
		"warning: trying to prepare a class that is not in the "
		"CLASS_LOADED state.\n");
#endif
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    /* first prepare the superclass. */
    if(class->resolved_superclass) {
	if(water_raiseState(env, class->resolved_superclass, 
			    CLASS_PREPARED, 0, 0) < 0) {
#ifdef DEBUG_djm
	    fprintf(stderr, "warning: couldn't prepare superclass.\n");
#endif
	    class->state = CLASS_PREPARATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
    }

    /* check out the fields.  for static fields, create some space    */
    /* associated with the class to store the values.  for non-static */
    /* fields, add their size to the total data size for the object,  */
    /* and compute their offsets into the object's data area.         */

    /* first we need to inherit fields from the superclass if there   */
    /* is one.                                                        */
    class->instance_data_size = 0;
    class->num_resolved_fields = class->fields_count;

    /* add the superclass's fields to the count, and start the offset */
    /* to our data after the superclass's data.                       */
    if(class->resolved_superclass) {
	class->instance_data_size += 
	    class->resolved_superclass->instance_data_size;
	class->num_resolved_fields += 
	    class->resolved_superclass->num_resolved_fields;
    }
    class->resolved_fields = 
	(waterField *)calloc(class->num_resolved_fields, 
			     sizeof(waterField));
    if(!class->resolved_fields) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	class->state = CLASS_PREPARATION_FAILED;
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    /* copy the superclass's fields if necessary. */
    if(class->resolved_superclass) {
	memcpy(class->resolved_fields, 
	       class->resolved_superclass->resolved_fields,
	       class->resolved_superclass->num_resolved_fields *
	       sizeof(waterField));
    }

    /* store the information about the fields specific to this class. */
    for(i = 0; i < class->fields_count; i++) {
	waterFieldOrMethod *field = &class->fields[i];
	waterField *resolved_field = 
	    &class->resolved_fields[class->resolved_superclass ? 
	        (class->resolved_superclass->num_resolved_fields + i) : i];
	char *field_descriptor =
	    water_UTF8ToNormal(water_getUTF8Constant(class, 
						     field->descriptor_index));

	field->resolved.field = resolved_field;
	resolved_field->name = water_getUTF8Constant(class, field->name_index);
	resolved_field->class = class;
	resolved_field->access_flags = field->access_flags;
	resolved_field->constant_value_initialized = 0;
	if(field_descriptor[0] == 'D' ||
	   field_descriptor[0] == 'J') {
	    resolved_field->width = 2;
	} else {
	    resolved_field->width = 1;
	}
	
	/* for static fields, we need to get the ConstantValue if there   */
	/* is one.  for instance fields, we have to add to the total data */
	/* size and compute the offset.                                   */
	if(field->access_flags & ACC_STATIC) {
	    waterConstantValueAttribute *constant_value =
		findConstantValueAttribute(class, 
				 field->attributes_count, 
				 field->attributes);

	    if(constant_value) {
		setConstantValue(resolved_field, constant_value);
	    }
	} else {
	    /* alignment shouldn't matter--we copy everything */
	    /* a byte at a time                               */
	    resolved_field->value.i = class->instance_data_size;
	    class->instance_data_size += 4 * resolved_field->width;
	}
    }
    
    /* ugh.  special case for java.lang.Class. */
    /* on its own it creates no fields.  however, we need one to store */
    /* a pointer back to the waterClass structure for the class.       */
    /* so we need to get some space.                                   */
    class_name = water_getClassName(class, class->this_class);
    class_name_normal = water_UTF8ToNormal(class_name);
    if(strcmp(class_name_normal, "java/lang/Class") == 0) {
	class->instance_data_size += 4;
    }

    /* copy the superclass's virtual method table */
    if(class->resolved_superclass) {
	class->method_table = 
	    (waterMethod *)
	    calloc(class->resolved_superclass->method_table_entries,
		   sizeof(waterMethod));
	if(!class->method_table) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    class->state = CLASS_PREPARATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
	memcpy(class->method_table, 
	       class->resolved_superclass->method_table,
	       class->resolved_superclass->method_table_entries *
	       sizeof(waterMethod));
	class->method_table_entries = 
	    class->resolved_superclass->method_table_entries;

	/* wound up looping anyway, to handle one of the 'first active use' */
	/* cases                                                            */
	for(i = 0; i < class->resolved_superclass->method_table_entries; i++) {
	    class->method_table[i].childmost_class = class;
	}
    }

    for(i = 0; i < class->methods_count; i++) {
	waterFieldOrMethod *method = &class->methods[i];
	waterMethod *entry;
	waterCodeAttribute *code;
	waterUTF8String *method_name;
	waterUTF8String *method_descriptor;

	if((method->access_flags & ACC_ABSTRACT) &&
	   !(class->access_flags & ACC_ABSTRACT) &&
	   !(class->access_flags & ACC_INTERFACE)) {
	    /* an abstract method in a non-abstract class is an error. */
	    /* unless it's an interface.                               */
	    water_throwException(WATER_ENV(env), 
				 "java/lang/AbstractMethodError");
	    class->state = CLASS_PREPARATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
	
	method_name = water_getUTF8Constant(class, method->name_index);
	method_descriptor = water_getUTF8Constant(class, 
						  method->descriptor_index);
	entry = water_virtualMethodLookup(class, 
					  method_name, method_descriptor);

	code = findCodeAttribute(class, 
				 method->attributes_count, 
				 method->attributes);
	/* must have code unless the method is abstract or native */
	assert(code != 0 || 
	       ((method->access_flags & ACC_ABSTRACT) ||
		(method->access_flags & ACC_NATIVE)));

	if(!entry) {
	    char *descriptor_p;
	    jshort arg_type;

	    /* the parent didn't have this method.  create an entry. */
	    class->method_table_entries++;
	    class->method_table = realloc(class->method_table,
					  sizeof(waterMethod) * 
					  class->method_table_entries);
	    if(!class->method_table) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		class->state = CLASS_PREPARATION_FAILED;
		PR_ExitMonitor(class->monitor);
		return -1;
	    }
	    entry = &class->method_table[class->method_table_entries - 1];
	    entry->index = class->method_table_entries - 1;
	    entry->class = class;
	    entry->childmost_class = class;
	    entry->name = method_name;
	    entry->descriptor = method_descriptor;
	    entry->code = code;
	    entry->native_code = 0;
	    entry->access_flags = method->access_flags;

	    /* store info about arguments */
	    /* skip the initial '(' */
	    entry->num_arguments = 0;
	    entry->argument_types = 0;
	    entry->args_size = 0;
	    entry->locals_size = 0;

	    descriptor_p = method_descriptor->contents + 1;
	    while(*descriptor_p != ')') {
		arg_type = getNextArgumentType(&descriptor_p);
		if(arg_type == JNI_INVALID) {
		    /* this means we got an invalid descriptor. */
#ifdef DEBUG_djm
		    fprintf(stderr, "invalid descriptor entry in class.\n");
#endif
		    class->state = CLASS_PREPARATION_FAILED;
		    PR_ExitMonitor(class->monitor);
		    return -1;
		}

		entry->num_arguments++;
		entry->argument_types =
		    (jshort *)realloc(entry->argument_types,
				      sizeof(jshort) * entry->num_arguments);
		entry->argument_types[entry->num_arguments-1] = arg_type;
		entry->args_size += water_javaTypeStackSize(arg_type);
	    }
	    if(!(entry->access_flags & ACC_STATIC)) {
		/* 'this' pointer is an extra argument for */
		/* non-static methods                      */
		entry->args_size += sizeof(jobject);
	    }
	    assert((entry->args_size % 4) == 0);

	    /* skip the ')' and get the return type */
	    descriptor_p++;
	    arg_type = getNextArgumentType(&descriptor_p);
	    if(arg_type == JNI_INVALID) {
		class->state = CLASS_PREPARATION_FAILED;
		PR_ExitMonitor(class->monitor);
		return -1;
	    }
	    entry->return_type = arg_type;
	} else {
	    /* the parent had this method, too.  just substitute our */
	    /* different code.                                       */
	    entry->class = class;
	    entry->code = code;

	    /* force the native code to be looked up again for the child */
	    entry->native_code = 0;

	    /* can the access flags be different, too? */
	    /* YES--particularly the ACC_NATIVE flag.  (non-natives can */
	    /* override natives and vice versa)                         */
	    entry->access_flags = method->access_flags;
	}

	if(entry->code) {
	    /* only for non-native methods do we have to know about */
	    /* how many locals there are.                           */

	    /* we don't need to re-parse the argument descriptor      */
	    /* if this is an inherited method--the arguments will     */
	    /* be the same.  however, we do need to update the        */
	    /* locals size whether or not this is inherited.          */
	    /* (inherited method can have different number of locals) */
	    entry->locals_size = 
		entry->code->max_locals * 4 - entry->args_size;
	}
    }

    /* do the interfaces. */
    /* copy the parent's list, then for each one this class implements, */
    /* add it to the list if it's not already there.                    */
    if(class->resolved_superclass) {
	class->resolved_interfaces = 
	    (waterClass **)
	    calloc(class->resolved_superclass->num_resolved_interfaces,
		   sizeof(waterClass *));
	if(!class->resolved_interfaces) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    class->state = CLASS_PREPARATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
	memcpy(class->resolved_interfaces,
	       class->resolved_superclass->resolved_interfaces,
	       class->resolved_superclass->num_resolved_interfaces *
	       sizeof(waterClass *));
	class->num_resolved_interfaces = 
	    class->resolved_superclass->num_resolved_interfaces;
    }

    for(i = 0; i < class->interfaces_count; i++) {
	waterClass *this_interface =
	    water_resolveClassByIndex(env, class, class->interfaces[i],
				      CLASS_PREPARED);
	if(!this_interface) {
	    /* exception should be thrown already */
	    assert(WATER_ENV(env)->exception_thrown);
	    class->state = CLASS_PREPARATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
	for(j = 0; j < class->num_resolved_interfaces; j++) {
	    if(this_interface == class->resolved_interfaces[j]) {
		break;
	    }
	}
	
	if(j == class->num_resolved_interfaces) {
	    /* couldn't find it.  make a new entry. */
	    class->resolved_interfaces = 
		(waterClass **)
		realloc(class->resolved_interfaces,
			sizeof(waterClass *) * 
			(class->num_resolved_interfaces + 1));
	    if(!class->resolved_interfaces) {
		water_throwException(WATER_ENV(env), 
				     "java/lang/OutOfMemoryError");
		class->state = CLASS_PREPARATION_FAILED;
		PR_ExitMonitor(class->monitor);
		return -1;
	    }
	    class->resolved_interfaces[class->num_resolved_interfaces] =
		this_interface;
	    class->num_resolved_interfaces++;
	}
    }

    class->state = CLASS_PREPARED;
    PR_ExitMonitor(class->monitor);

    return 0;
}


static waterConstantValueAttribute *
findConstantValueAttribute(waterClass *class,
			   int num_attributes,
			   waterAttribute *attributes)
{
    int i;
    static waterUTF8String *cv_string = 0;

    if(!cv_string)
	cv_string = water_NormalToUTF8("ConstantValue");

    for(i = 0; i < num_attributes; i++) {
	if(water_UTF8Compare(cv_string,
			     water_getUTF8Constant(class, 
					 attributes[i].name_index)) == 0) {
	    /* WARNING: if it's not a CV attribute */
	    return &attributes[i].parsed_contents.constant;
	}
    }

    return 0;
}

static void setConstantValue(waterField *field, 
			     waterConstantValueAttribute *constant_value)
{
    COPY_BYTES_BIGENDIAN(&field->value, 
			 &field->class->constant_pool
			 [constant_value->constantvalue_index].value,
			 field->width * 4);

    field->constant_value_initialized = 1;	
}

static waterCodeAttribute *findCodeAttribute(waterClass *class,
					     int num_attributes,
					     waterAttribute *attributes)
{
    int i;
    static waterUTF8String *code_string = 0;

    if(!code_string)
	code_string = water_NormalToUTF8("Code");

    for(i = 0; i < num_attributes; i++) {
	if(water_UTF8Compare(code_string,
			     water_getUTF8Constant(class, 
					   attributes[i].name_index)) == 0) {
	    /* WARNING: if it's not a code attribute */
	    return &attributes[i].parsed_contents.code;
	}
    }

    return 0;
}

waterMethod *water_virtualMethodLookup(waterClass *class,
				       waterUTF8String *method_name,
				       waterUTF8String *method_descriptor)
{
    int i;

    for(i = 0; i < class->method_table_entries; i++)
	if(water_UTF8Compare(class->method_table[i].name, method_name) == 0 &&
	   water_UTF8Compare(class->method_table[i].descriptor,
			     method_descriptor) == 0)
	    return &class->method_table[i];

    return 0;
}

static int initializeClass(JNIEnv *env, waterClass *class)
{
    waterUTF8String *clinit_name;
    waterUTF8String *clinit_descriptor;
    waterMethod *clinit_method;
    
#ifdef DEBUG_djm
    waterUTF8String *class_name = water_getClassName(class, class->this_class);
    char *class_name_normal = water_UTF8ToNormal(class_name);

    fprintf(stderr, "initializing class '%s'...\n", class_name_normal);
    free(class_name_normal);
#endif

    /* these steps are from the 'detailed initialization procedure' */
    /* Sun JVM Spec 2.16.5                                          */

    /* step 1 */
    PR_EnterMonitor(class->monitor);

    /* step 2 */
    /* we make sure we have the monitor before reading or writing */
    /* class->initializing_thread.  if we notice someone else is  */
    /* initializing, wait until they are done.  (we should be     */
    /* waiting until the monitor is released by someone, and      */
    /* that shouldn't happen until the initialization is actually */
    /* complete (or errors out)).                                 */
    while(class->initializing_thread &&
	  class->initializing_thread != PR_CurrentThread())
	PR_Wait(class->monitor, PR_INTERVAL_NO_TIMEOUT);

    if(class->initializing_thread) {
	if(class->initializing_thread != PR_CurrentThread()) {
	    /* shouldn't happen */
	    fprintf(stderr,
		    "warning: we've got the monitor, but somebody else "
		    "is initializing!\n");
	    PR_ExitMonitor(class->monitor);
	    return -1;
	} else {
	    /* step 3 */
	    /* a recursive call for initialization.  ignore. */
	    /* at least, I think that's what they mean by "complete */
	    /* normally".                                           */
	    PR_ExitMonitor(class->monitor);
	    return 0;
	}
    }

    /* step 4 */
    if(class->state == CLASS_INITIALIZED) {
	/* the class has already been initialized.  leave. */
	PR_ExitMonitor(class->monitor);
	return 0;
    }

    /* step 5 */
    if(class->state != CLASS_VERIFIED) {
	/* EXCEPTION: NoClassDefFound (bad state for initialization) */
	fprintf(stderr, 
		"warning: tried to initialize a class that's not in "
		"the right state.\n");
	PR_ExitMonitor(class->monitor);
	return -1;
    }

    /* step 6 */
    /* we're in a position to initialize.  note that we're going to */
    /* and release the monitor.                                     */
    class->initializing_thread = PR_CurrentThread();
    PR_ExitMonitor(class->monitor);

    /* step 7 */
    /* initialize superclasses if this is a class */
    if(!(class->access_flags & ACC_INTERFACE)) {
	if(class->resolved_superclass) {
	    if(water_raiseState(env, class->resolved_superclass, 
				CLASS_INITIALIZED, 0, 0) < 0) {
		/* EXCEPTION: whatever exception the recursive call threw */
#ifdef DEBUG_djm
		fprintf(stderr,
			"warning: superclass initialization failed.\n");
#endif
		PR_EnterMonitor(class->monitor);
		class->state = CLASS_INITIALIZATION_FAILED;
		class->initializing_thread = 0;

		PR_Notify(class->monitor);
		PR_ExitMonitor(class->monitor);
		return -1;
	    }
	}
    }

    /* step 8 */
    /* the spec says "execute either the class variable initializers and */
    /* static initializers of the class, or the field variable           */
    /* initializers of the interface".  I think that means just call     */
    /* <clinit> which does all the stuff between the quotes.             */
    clinit_name = water_NormalToUTF8("<clinit>");
    clinit_descriptor = water_NormalToUTF8("()V");
    clinit_method = water_virtualMethodLookup(class, 
					      clinit_name, clinit_descriptor);
    water_freeUTF8String(clinit_name);
    water_freeUTF8String(clinit_descriptor);

    if(clinit_method) {
	water_jni_CallStaticVoidMethod(env, class, clinit_method);
	/* EXCEPTION: check for one */

	if(0) {
	    /* step 10 */
	    /* XXX actually throw the exception */
	    
	    /* step 11 */
	    /* mark class as fucked */
	    PR_EnterMonitor(class->monitor);
	    class->state = CLASS_INITIALIZATION_FAILED;
	    class->initializing_thread = 0;
	    
	    PR_Notify(class->monitor);
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
    }

    /* step 9 */
    /* initialization complete and successful */
    PR_EnterMonitor(class->monitor);
    class->state = CLASS_INITIALIZED;
    class->initializing_thread = 0;

    PR_Notify(class->monitor);
    PR_ExitMonitor(class->monitor);

    return 0;
}

static jshort getNextArgumentType(char **descriptor_p)
{
    int next_is_array = 0;
    jshort arg_type = 0;

    while(1) {
	switch(**descriptor_p) {
	case 'B': arg_type = JNI_BYTE;     (*descriptor_p)++; break;
	case 'C': arg_type = JNI_CHAR;     (*descriptor_p)++; break;
	case 'D': arg_type = JNI_DOUBLE;   (*descriptor_p)++; break;
	case 'F': arg_type = JNI_FLOAT;    (*descriptor_p)++; break;
	case 'I': arg_type = JNI_INT;      (*descriptor_p)++; break;
	case 'J': arg_type = JNI_LONG;     (*descriptor_p)++; break;
	case 'S': arg_type = JNI_SHORT;    (*descriptor_p)++; break;
	case 'Z': arg_type = JNI_BOOLEAN;  (*descriptor_p)++; break;
	case 'V': arg_type = JNI_NONE;     (*descriptor_p)++; break;
	case '[': next_is_array = 1; (*descriptor_p)++; break;
	case 'L':
	    arg_type = JNI_OBJECT;
	    (*descriptor_p)++;
	    while(*((*descriptor_p)++) != ';') /* skip */;
	    break;

	default:
	    fprintf(stderr, "warning: bad argument type '%c'\n",
		    **descriptor_p);
	    arg_type = JNI_INVALID;
	    break;
	}

	if(arg_type != 0) {
	    if(next_is_array) {
		arg_type |= JNI_IS_ARRAY;
	    }
	    return arg_type;
	}
    }
}

void water_linkNativeMethod(JNIEnv *env, waterMethod *method)
{
    WATER_ENV_DECL(wenv);
    char *mangled_method_name;
    waterClass *class;
    int i;

    assert(env != 0);
    assert(method != 0);
    assert(method->class != 0);

    class = method->class;
    while(class) {
	mangled_method_name = mangledMethodName(env, class, method);
	/* XXX check for malloc failure */

	i = 0;
	while(i < wenv->wvm->num_native_libs &&
	      method->native_code == 0) {
	    method->native_code = dlsym(wenv->wvm->native_libs[i],
					mangled_method_name);
#if defined(DEBUG_djm) && 1
	    if(method->native_code) {
		fprintf(stderr, 
			"found native method '%s' at %p "
			"in native library %d...\n", 
			mangled_method_name, method->native_code, i);
	    } else {
		fprintf(stderr, "haven't found method '%s' yet.\n", 
			mangled_method_name);
	    }
#endif
	    i++;
	}

	free(mangled_method_name);
	class = class->resolved_superclass;
    }

#ifdef DEBUG_djm
    if(!method->native_code) {
	fprintf(stderr, "native method not found.\n");
	assert(0);
    }
#endif
}

static char *mangledMethodName(JNIEnv *env, waterClass *class, 
			       waterMethod *method)
{
    waterUTF8String *class_name;
    char *method_name_normal, *class_name_normal, 
	*mangled_class_name, *mangled_method_name, *result;

    assert(env != 0);
    assert(class != 0);
    assert(method != 0);

    method_name_normal = water_UTF8ToNormal(method->name);
    class_name = water_getClassName(class, class->this_class);
    class_name_normal = water_UTF8ToNormal(class_name);
    mangled_class_name = mangleString(class_name_normal);
    mangled_method_name = mangleString(method_name_normal);

    /* XXX does not work for overloaded native methods */
    result = (char *)malloc(5 +                    /* "Java_" */
			    strlen(mangled_class_name) +
			    1 +                    /* "_" */
			    strlen(mangled_method_name) +
			    1);
    if(!result) {
	free(method_name_normal);
	free(class_name_normal);
	free(mangled_class_name);
	free(mangled_method_name);
	return 0;
    }

    strcpy(result, "Java_");
    strcat(result, mangled_class_name);
    strcat(result, "_");
    strcat(result, mangled_method_name);

    free(method_name_normal);
    free(class_name_normal);
    free(mangled_class_name);
    free(mangled_method_name);

    return result;
}

static char *mangleString(char *string)
{
    /* ok, a bit obnoxious, but this is the maximum length to which */
    /* the string will ever expand.                                 */
    char *result = (char *)malloc(strlen(string)*5 + 1);
    char *string_p = string, *result_p = result;

    /* see the JNI spec */
    while(*string_p) {
	if(*string_p == '/') {
	    *result_p++ = '_';
	} else if(*string_p == '_') {
	    *result_p++ = '_';
	    *result_p++ = '1';
	} else if(*string_p == ';') {
	    *result_p++ = '_';
	    *result_p++ = '2';
	} else if(*string_p == '[') {
	    *result_p++ = '_';
	    *result_p++ = '3';
	} else {
	    /* XXX non-printable Unicode characters */
	    *result_p++ = *string_p;
	}

	string_p++;
    }
    *result_p = '\0';

    return result;
}

int water_isSuperclass(JNIEnv *env, waterClass *parent, waterClass *child)
{
    assert(parent != 0);
    assert(child != 0);

    while(child) {
	if(parent == child) {
	    return JNI_TRUE;
	}

	/* if it's a fetus we now have to load it. */
	if(water_raiseState(env, child, CLASS_LOADED, 0, 0) < 0) {
	    return 0;
	}

	child = child->resolved_superclass;
    }

    return JNI_FALSE;
}

int water_implementsInterface(JNIEnv *env, 
			      waterClass *interface, waterClass *class)
{
    int i;

    /* the class has to be prepared. */
    if(water_raiseState(env, class, CLASS_PREPARED, 0, 0) < 0) {
	return 0;
    }
    for(i = 0; i < class->num_resolved_interfaces; i++) {
	if(water_isSuperclass(env, 
			      class->resolved_interfaces[i], interface)) {
	    return 1;
	}
    }

    return 0;
}

struct waterObject *water_findClassObjectString(JNIEnv *env, const char *name)
{
    waterClass *class;

    class = water_jni_FindClass(env, name);
    if(!class) {
	return 0;
    }

    return water_findClassObjectClass(env, class);
}

struct waterObject *water_findClassObjectClass(JNIEnv *env,
					       struct waterClass *class)
{
    assert(class != 0);

    if(!class->class_object) {
	class->class_object = water_newJavaLangClass(env, class);
    }

    return class->class_object;
}


waterClass *water_getPrimitiveClass(JNIEnv *env, int jni_type)
{
    WATER_ENV_DECL(wenv);
    waterClass **class_slot;
    waterClass *result;
    int base_jni_type;

    base_jni_type = (jni_type & ~JNI_IS_ARRAY);

    /* XXX these objects should be in the VM struct, not in the JNIEnv */
    switch(base_jni_type) {
    case(JNI_BOOLEAN):	        class_slot = &wenv->boolean_class; break;
    case(JNI_BYTE):	        class_slot = &wenv->byte_class; break;
    case(JNI_CHAR):	        class_slot = &wenv->char_class; break;
    case(JNI_SHORT):	        class_slot = &wenv->short_class; break;
    case(JNI_INT):	        class_slot = &wenv->int_class; break;
    case(JNI_LONG):	        class_slot = &wenv->long_class; break;
    case(JNI_FLOAT):	        class_slot = &wenv->float_class; break;
    case(JNI_DOUBLE):	        class_slot = &wenv->double_class; break;
    default:
	assert(0);
	return 0;
    }

    if(*class_slot == 0) {
	*class_slot = (waterClass *)calloc(1, sizeof(waterClass));
	if(!*class_slot) {
	    water_throwException((water_JNIEnv *)env, 
				 "java/lang/OutOfMemoryError");
	    return 0;
	}
	(*class_slot)->class_type = base_jni_type;
    }

    if(jni_type & JNI_IS_ARRAY) {
	result = water_getCorrespondingArrayClass(env, *class_slot);
    } else {
	result = *class_slot;
    }

    assert(result);
    return result;
}

waterClass *water_getCorrespondingArrayClass(JNIEnv *env, waterClass *class)
{
    waterClass *new_array_class;

    assert(class);

    /* do we have one already? */
    if(class->array_class) {
	return class->array_class;
    }

    /* nope, create one.  it's a copy of the Object class with         */
    /* the class_type changed and the array_class pointer pointing     */
    /* back to the normal class.                                       */
    
    /* this must be a copy of the Object class because you can call    */
    /* java.lang.Object methods on array references.                   */
    /* actually, this should probably be                               */
    /* a child of the Object class.  (although this is not a normal    */
    /* class, and you shouldn't be looking at stuff inside it except   */
    /* the method table and the three header fields).                  */
    
    new_array_class = (waterClass *)malloc(sizeof(waterClass));
    assert(WATER_ENV(env)->java_lang_object_class != 0);
    memcpy(new_array_class, WATER_ENV(env)->java_lang_object_class, 
	   sizeof(waterClass));
    if(class->class_type == 0 ||
       (class->class_type & JNI_IS_ARRAY)) {
	/* array of a non-primitive type, or */
	/* array of arrays. */
	new_array_class->class_type = JNI_OBJECT_ARRAY;
    } else {
	/* array of a primitive type. */
	new_array_class->class_type = class->class_type | JNI_IS_ARRAY;
    }

    new_array_class->resolved_superclass = 
	WATER_ENV(env)->java_lang_object_class;
    new_array_class->class_object = 0;

    class->array_class = new_array_class;
    new_array_class->elements_class = class;
    new_array_class->array_class = 0;

    return new_array_class;
}

waterClass *water_getArrayClassByName(JNIEnv *env, const char *name,
				      struct waterObject *class_loader,
				      int desired_state)
{
    waterClass *elements_class;
    char *class_name;
    waterClass *found_class;

    /* get a primitive class based on its descriptor. */
    /* this also works for multidimensional arrays, e.g., "[[[D".     */
    /* each time we see an array marker, we recursively grab the      */
    /* inner class, then create/get a corresponding array class to    */
    /* contain it.                                                    */
    switch(name[0]) {
    case 'Z':    return water_getPrimitiveClass(env, JNI_BOOLEAN);
    case 'B':    return water_getPrimitiveClass(env, JNI_BYTE);
    case 'C':    return water_getPrimitiveClass(env, JNI_CHAR);
    case 'S':    return water_getPrimitiveClass(env, JNI_SHORT);
    case 'I':    return water_getPrimitiveClass(env, JNI_INT);
    case 'J':    return water_getPrimitiveClass(env, JNI_LONG);
    case 'F':    return water_getPrimitiveClass(env, JNI_FLOAT);
    case 'D':    return water_getPrimitiveClass(env, JNI_DOUBLE);
    case 'L':
	class_name = strdup(name + 1);
	if(!class_name) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    return 0;
	}

	/* kill the ';' */
	class_name[strlen(class_name)-1] = '\0';

	found_class = water_findClass(env, class_loader, class_name,
				      desired_state, 0, 0);
	free(class_name);

	return found_class;
    case '[':
	elements_class = water_getArrayClassByName(env, name + 1,
						   class_loader, 
						   desired_state);
	if(WATER_ENV(env)->exception_thrown) {
	    return 0;
	}
	return water_getCorrespondingArrayClass(env, elements_class);
    default:
	assert(0);
	abort();
	return 0;
    }
}

int water_arrayDimensions(JNIEnv *env, waterClass *class)
{
    int count;
    waterClass *elements_class;

    assert(class->class_type & JNI_IS_ARRAY);

    elements_class = class->elements_class;
    count = 0;
    while(elements_class) {
	count++;
	elements_class = elements_class->elements_class;
    }

    return count;
}

struct waterObject *water_getPrimitiveClassObject(JNIEnv *env, int jni_type)
{
    waterClass *primitive_class;

    primitive_class = water_getPrimitiveClass(env, jni_type);
    assert(primitive_class);

    return water_findClassObjectClass(env, primitive_class);
}

int water_castIsOK(water_JNIEnv *wenv, waterClass *target, waterClass *source)
{
    /* this implements the definition on pages 257 and 175 of the VM */
    /* spec (checkcast and instanceof).                              */

    assert(wenv != 0);
    assert(target != 0);
    assert(source != 0);

    /* see JLS 5.5 "casting conversions".                               */
    /* look at the run-time conversion rules, not the compile-time ones */
    /* 'R' is source, 'T' is target.                                    */

    /* one shortcut for fetuses--casting any class to itself is OK. */
    if(target == source) {
	return 1;
    }

    /* otherwise, we have to look at the access flags.  therefore the */
    /* class has to be loaded.                                        */
    if(water_raiseState(JNIENV(wenv), target, CLASS_LOADED, 0, 0) < 0) {
	return 0;
    }
    if(water_raiseState(JNIENV(wenv), source, CLASS_LOADED, 0, 0) < 0) {
	return 0;
    }

    if(source->class_type == 0 &&
       !(source->access_flags & ACC_INTERFACE)) {
	/* if R is a class type: */
	if(target->class_type == 0 &&
	   !(target->access_flags & ACC_INTERFACE)) {
	    /* if T is a class type: */
	    return water_isSuperclass((JNIEnv *)wenv, target, source);
	} else if(target->class_type == 0 &&
		  (target->access_flags & ACC_INTERFACE)) {
	    /* if T is an interface type: */
	    return water_implementsInterface((JNIEnv *)wenv, target, source);
	} else {
	    /* T must not be an array type. */
	    return 0;
	}   
    } else if(source->class_type == 0 &&
	      (source->access_flags & ACC_INTERFACE)) {
	/* if R is an interface: */
	if(target->class_type == 0 &&
	   !(target->access_flags & ACC_INTERFACE)) {
	    /* if T is a class type: */
	    /* T must be java.lang.Object. */
	    return target == wenv->java_lang_object_class;
	} else if(target->class_type == 0 &&
		  (target->access_flags & ACC_INTERFACE)) {
	    /* if T is an interface: */
	    /* T must be a superinterface of R. */
	    return water_isSuperclass((JNIEnv *)wenv, target, source);
	} else {
	    /* T must not be an array type. */
	    return 0;
	}
    } else if(source->class_type & JNI_IS_ARRAY) {
	/* if R is a class representing an array type RC[]: */
	if(target->class_type == 0 &&
	   !(target->access_flags & ACC_INTERFACE)) {
	    /* if T is a class type: */
	    /* T must be java.lang.Object. */
	    return target == wenv->java_lang_object_class;
	} else if(target->class_type == 0 &&
		  (target->access_flags & ACC_INTERFACE)) {
	    /* if T is an interface type: */
	    /* T must be java.lang.Cloneable. */
	    return target == wenv->java_lang_cloneable_interface;
	} else if(target->class_type & JNI_IS_ARRAY) {
	    /* if T is a class representing an array type TC[]: */
	    if(target->class_type == source->class_type) {
		if(target->class_type != JNI_OBJECT_ARRAY) {
		    return 1;
		} else {
		    /* compare the classes of the elements. */
		    return water_castIsOK(wenv, 
					  target->elements_class,
					  source->elements_class);
		}
	    } else {
		return 0;
	    }
	} else {
	    /* at this point the target must be a primitive.  we can't */
	    /* allow that.                                             */
	    return 0;
	}
    } else {
	/* the source is a primitive.  the cast is only OK if the target */
	/* is the exact same class representing the primitive.           */
	return source == target;
    }

    /* shouldn't have made it to here. */
    assert(0);
    return 0;
}



void water_loadDefaultNatives(water_JavaVM *vm)
{
    water_loadNativeLibrary(vm, "libwaternative_vmspecific.so");
    water_loadNativeLibrary(vm, "libwaternative_jni.so");
    /*    water_loadNativeLibrary(vm, "libjaphar_net.so.1.0.0");*/
}

int water_loadNativeLibrary(water_JavaVM *vm, const char *name)
{
    void *native_lib;
    void *new_native_libs;

    native_lib = dlopen(name, RTLD_NOW);
    if(!native_lib) {
#ifdef DEBUG_djm
	fprintf(stderr, 
		"warning: couldn't open native methods library '%s': %s.\n", 
		name, dlerror());
#endif
	return -1;
    } else {
	vm->num_native_libs++;
	new_native_libs = 
	    (void **)realloc(vm->native_libs, 
			     vm->num_native_libs * sizeof(void *));
	if(!new_native_libs) {
	    dlclose(native_lib);
	    return -1;
	}
	vm->native_libs = new_native_libs;
	vm->native_libs[vm->num_native_libs - 1] = native_lib;
    }

    return 0;
}

struct waterObject *water_getThreadObject(JNIEnv *env)
{
    jclass thread_class, threadgroup_class;
    jfieldID privateinfo_field, threadgroup_field, priority_field;
    jmethodID threadgroup_constructor;
    jobject thread_object, initial_thread_group;

    if(WATER_ENV(env)->thread_object) {
	return WATER_ENV(env)->thread_object;
    }

    /* if the thread object is not set, that means this is the original,   */
    /* first thread.  we need to make the fields look basically right, but */
    /* we can't call any Thread constructors, because they call            */
    /* getCurrentThread() again, and we get an infinite loop.              */

    thread_class = (*env)->FindClass(env, "java/lang/Thread");
    assert(thread_class);
    thread_object = water_newObject(env, thread_class);

    threadgroup_class = (*env)->FindClass(env, "java/lang/ThreadGroup");
    assert(threadgroup_class);

    threadgroup_constructor = (*env)->GetMethodID(env, threadgroup_class,
						  "<init>", "()V");
    assert(threadgroup_constructor);
    initial_thread_group = (*env)->NewObject(env, threadgroup_class,
					     threadgroup_constructor);
    assert(initial_thread_group);

    threadgroup_field = (*env)->GetFieldID(env, thread_class, "group",
					   "Ljava/lang/ThreadGroup;");
    assert(threadgroup_field);
    (*env)->SetObjectField(env, thread_object, threadgroup_field, 
			   initial_thread_group);

    priority_field = (*env)->GetFieldID(env, thread_class, "priority", "I");
    assert(priority_field);
    (*env)->SetIntField(env, thread_object, priority_field, 5);

    privateinfo_field = (*env)->GetFieldID(env, thread_class, 
					   "PrivateInfo", "I");
    assert(privateinfo_field);

    (*env)->SetIntField(env, thread_object, privateinfo_field, (jint)env);

#ifdef DEBUG_djm
    fprintf(stderr, 
	    "storing in thread_object %p (first thread) env pointer %p\n",
	    thread_object, env);
#endif

    WATER_ENV(env)->thread_object = thread_object;

    return thread_object;
}
