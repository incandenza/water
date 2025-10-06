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
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/* XXX I thought this was in <limits.h>--where is it? */
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static unsigned char *getFilenameFromFile(JNIEnv *env, jobject file_object);
static void openFile(JNIEnv *env, jobject is_object, jobject filename_string,
		     int flags);
static void storeFD(JNIEnv *env, jobject stream_object, int fd);
static int getFD(JNIEnv *env, jobject stream_object);


jlong Java_java_lang_System_currentTimeMillis(JNIEnv *env, jclass system_class)
{
    struct timeval time;
    jlong result;

    gettimeofday(&time, 0);
    /* tv_usec is _micro_seconds */
    result = (jlong)time.tv_sec * 1000 + (jlong)time.tv_usec / 1000;

#ifdef DEBUG_djm
    fprintf(stderr, "currentTimeMillis() called--time is %Lu (a.k.a %Lx)\n", result, result);
#endif
    return result;
}

void Java_java_lang_System_setIn0(JNIEnv *env, jclass system_class,
				  jobject stream_object)
{
    /* XXX */
}

void Java_java_lang_System_setOut0(JNIEnv *env, jclass system_class,
				   jobject stream_object)
{
    /* XXX */
}

void Java_java_lang_System_setErr0(JNIEnv *env, jclass system_class,
				   jobject stream_object)
{
    /* XXX */
}

jobject Java_java_io_FileDescriptor_initSystemFD(JNIEnv *env, jclass j_class,
						 jobject fd_object, int fd)
{
    /* just save the file descriptor into the object.         */
    /* we don't have to do any special remapping or anything. */

    /* XXX use JNI instead */
    STORE_INT_MACHINE(fd, &((waterObject *)fd_object)->data[0], 0);

    return fd_object;
}

jboolean Java_java_io_FileDescriptor_valid(JNIEnv *env, jobject fd_object)
{
    int fd;

    /* XXX use JNI instead */
    GET_INT_MACHINE(fd, &((waterObject *)fd_object)->data[0], 0);

    /* what other tests can be performed here? */
    /* need to figure out if the fd is open. */
    /* there must be a system call that does this, but I can't figure out */
    /* what it is at the moment... XXX                                    */
    if(fd >= 0) {
	return JNI_TRUE;
    } else {
	return JNI_FALSE;
    }
}

void Java_java_io_FileDescriptor_sync(JNIEnv *env, jobject fd_object)
{
    int fd;

    /* XXX use JNI instead */
    GET_INT_MACHINE(fd, &((waterObject *)fd_object)->data[0], 0);

    fsync(fd);
}

void Java_java_lang_Runtime_exitInternal(JNIEnv *env, jobject runtime, 
					 jint status)
{
    /* this really is supposed to exit the entire VM.  wacky.           */
    /* this can't be called without a SecurityManager (if there is one) */
    /* permitting it, of course.                                        */
    exit(status);
}

jlong Java_java_lang_Double_doubleToLongBits(JNIEnv *env, jclass class, 
					     jdouble d)
{
    jlong result;

    STORE_DOUBLE_MACHINE(d, &result, 0);
#ifdef DEBUG_djm
    fprintf(stderr, "doubleToLongBits: double is %f, long bits are %Ld\n",
	    d, result);
#endif
    return result;
}

jdouble Java_java_lang_Double_longBitsToDouble(JNIEnv *env, jclass class, 
					       jlong l)
{
    jdouble result;

    STORE_LONG_MACHINE(l, &result, 0);
#ifdef DEBUG_djm
    fprintf(stderr, "longBitsToDouble: long bits are %Ld, double is %f\n",
	    l, result);
#endif
    return result;
}

jint Java_java_lang_Float_floatToIntBits(JNIEnv *env, jclass class, jfloat f)
{
    jint result;

    STORE_FLOAT_MACHINE(f, &result, 0);
    return result;
}

jfloat Java_java_lang_Float_intBitsToFloat(JNIEnv *env, jclass class, jint i)
{
    jfloat result;

    STORE_INT_MACHINE(i, &result, 0);
    return result;
}

jdouble Java_java_lang_Math_sin(JNIEnv *env, jclass class, jdouble arg)
{
    return sin(arg);
}

jdouble Java_java_lang_Math_cos(JNIEnv *env, jclass class, jdouble arg)
{
    return cos(arg);
}

jdouble Java_java_lang_Math_tan(JNIEnv *env, jclass class, jdouble arg)
{
    return tan(arg);
}

jdouble Java_java_lang_Math_asin(JNIEnv *env, jclass class, jdouble arg)
{
    return asin(arg);
}

jdouble Java_java_lang_Math_acos(JNIEnv *env, jclass class, jdouble arg)
{
    return acos(arg);
}

jdouble Java_java_lang_Math_atan(JNIEnv *env, jclass class, jdouble arg)
{
    return atan(arg);
}

jdouble Java_java_lang_Math_exp(JNIEnv *env, jclass class, jdouble arg)
{
    return exp(arg);
}

jdouble Java_java_lang_Math_log(JNIEnv *env, jclass class, jdouble arg)
{
    return log(arg);
}

jdouble Java_java_lang_Math_sqrt(JNIEnv *env, jclass class, jdouble arg)
{
    return sqrt(arg);
}

jdouble Java_java_lang_Math_ceil(JNIEnv *env, jclass class, jdouble arg)
{
    return ceil(arg);
}

jdouble Java_java_lang_Math_floor(JNIEnv *env, jclass class, jdouble arg)
{
    return floor(arg);
}

jdouble Java_java_lang_Math_rint(JNIEnv *env, jclass class, jdouble arg)
{
    return rint(arg);
}

jdouble Java_java_lang_Math_atan2(JNIEnv *env, jclass class, 
				  jdouble arg1, jdouble arg2)
{
    return atan2(arg1, arg2);
}

jdouble Java_java_lang_Math_pow(JNIEnv *env, jclass class, 
				jdouble arg1, jdouble arg2)
{
    return pow(arg1, arg2);
}

jboolean Java_java_io_File_isFile0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    struct stat st;

#ifdef DEBUG_djm
    fprintf(stderr, "got filename '%s'\n", filename);
#endif

    if(stat(filename, &st) < 0) {
	free(filename);
	return JNI_FALSE;
    }
    free(filename);

    if(S_ISREG(st.st_mode)) {
#ifdef DEBUG_djm
	fprintf(stderr, "it's a file.\n");
#endif
	return JNI_TRUE;
    } else {
#ifdef DEBUG_djm
	fprintf(stderr, "it's not a file.\n");
#endif
	return JNI_FALSE;
    }
}

jboolean Java_java_io_File_isDirectory0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    struct stat st;

#ifdef DEBUG_djm
    fprintf(stderr, "got filename '%s'\n", filename);
#endif

    if(stat(filename, &st) < 0) {
	free(filename);
	return JNI_FALSE;
    }
    free(filename);

    if(S_ISDIR(st.st_mode)) {
#ifdef DEBUG_djm
	fprintf(stderr, "it's a directory.\n");
#endif
	return JNI_TRUE;
    } else {
#ifdef DEBUG_djm
	fprintf(stderr, "it's not a directory.\n");
#endif
	return JNI_FALSE;
    }
}

jboolean Java_java_io_File_exists0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    struct stat st;

    if(stat(filename, &st) < 0) {
	free(filename);
	return JNI_FALSE;
    }
    free(filename);
    return JNI_TRUE;
}

jlong Java_java_io_File_lastModified0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    struct stat st;

    if(stat(filename, &st) < 0) {
	free(filename);
	/* XXX the API spec doesn't say that this throws any exception */
	/* except a SecurityException...  so I guess we don't throw    */
	/* one if the file doesn't exist.                              */
	return 0;
    }

    return (jlong)st.st_mtime;
}

jobjectArray Java_java_io_File_list0(JNIEnv *env, jobject file_object)
{
    unsigned char *dirname = getFilenameFromFile(env, file_object);
    struct dirent **filelist;
    int num_files, i, j;
    waterObject *filenames_array;
    jclass string_class;
    waterObject *string;

    num_files = scandir(dirname, &filelist, 0, alphasort);
    free(dirname);
    if(num_files < 0) {
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return 0;
    }

    string_class = (*env)->FindClass(env, "java/lang/String");
    assert(string_class);
    filenames_array = water_newObjectArray(env, 
					   /* skip "." and ".." */
					   num_files - 2, 
					   string_class);
    if(!filenames_array) {
	return 0;
    }

    for(i = 0, j = 0; i < num_files; i++) {
	if(!(strcmp(filelist[i]->d_name, ".") == 0 ||
	     strcmp(filelist[i]->d_name, "..") == 0)) {
	    string = water_newStringFromBytes(env, filelist[i]->d_name);
	    if(!string) {
		return 0;
	    }
	    STORE_REFERENCE_MACHINE(string, 
				    ARRAY_ELEMENT_PTR(filenames_array, j), 0);
	    j++;
	}
    }

    return filenames_array;
}

jboolean Java_java_io_File_isAbsolute(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    jboolean result;

    if(!filename) {
	return JNI_FALSE;
    }
    result = (filename[0] == '/') ? JNI_TRUE : JNI_FALSE;
    free(filename);

    return result;    
}

jboolean Java_java_io_File_delete0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);

    if(!filename) {
	return JNI_FALSE;
    }
    if(unlink(filename) < 0) {
	return JNI_FALSE;
    } else {
	return JNI_TRUE;
    }
}

jboolean Java_java_io_File_rmdir0(JNIEnv *env, jobject file_object)
{
    /* rmdir is the same as delete */
    return Java_java_io_File_delete0(env, file_object);
}

jboolean Java_java_io_File_mkdir0(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);

    if(!filename) {
	return JNI_FALSE;
    }
    if(mkdir(filename, 0777) < 0) {
	return JNI_FALSE;
    } else {
	return JNI_TRUE;
    }
}

jboolean Java_java_io_File_renameTo0(JNIEnv *env, 
				     jobject source_file_object,
				     jobject target_file_object)
{
    unsigned char *source_filename, *target_filename;

    source_filename = getFilenameFromFile(env, source_file_object);
    if(!source_filename) {
	return JNI_FALSE;
    }

    target_filename = getFilenameFromFile(env, target_file_object);
    if(!target_filename) {
	free(source_filename);
	return JNI_FALSE;
    }

    if(rename(source_filename, target_filename) < 0) {
	return JNI_FALSE;
    } else {
	return JNI_TRUE;
    }
}

jlong Java_java_io_File_length(JNIEnv *env, jobject file_object)
{
    unsigned char *filename = getFilenameFromFile(env, file_object);
    struct stat st;

    if(filename < 0) {
	return 0;
    }

    if(stat(filename, &st) < 0) {
	free(filename);
	/* XXX the API spec doesn't say that this throws any exception */
	/* except a SecurityException...  so I guess we don't throw    */
	/* one if the file doesn't exist.                              */
	return 0;
    }

    return (jlong)st.st_size;
}

void Java_java_io_FileInputStream_open(JNIEnv *env, jobject is_object, 
				       jobject filename_string)
{
    openFile(env, is_object, filename_string, O_RDONLY);
}

jint Java_java_io_FileInputStream_read(JNIEnv *env, jobject is_object)
{
    int fd = getFD(env, is_object);
    int bytes_read;
    jbyte byte;

    bytes_read = read(fd, &byte, 1);
    if(bytes_read == -1) {
	water_throwException(WATER_ENV(env), "java/lang/IOException");
	return -1;
    } else if(bytes_read == 0) {
	/* EOF */
	return -1;
    } else {
	return (jint)byte;
    }
}

jint Java_java_io_FileInputStream_readBytes(JNIEnv *env, jobject is_object,
					    jbyteArray byte_array, 
					    jint start_byte, jint num_bytes)
{
    int fd = getFD(env, is_object);
    jint result;

#if defined(DEBUG_djm)
    int array_length;
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(byte_array), 0);
    assert(start_byte + num_bytes <= array_length);
#endif

#ifdef DEBUG_djm
    fprintf(stderr, "in readBytes(), fd is %d, reading %d bytes...\n", fd, num_bytes);
#endif

    /* XXX should check the result and throw exceptions if necessary here */
    /* XXX should keep reading if we don't get everything the first time  */
    result = read(fd, ARRAY_ELEMENT_PTR(byte_array, start_byte), num_bytes);
#ifdef DEBUG_djm
    fprintf(stderr, "readBytes() done, read %d bytes.\n", result);
#endif
    if(result < 0) {
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return -1;
    } else if(result == 0) {
	return -1;
    } 
    
    return result;
}

jlong Java_java_io_FileInputStream_skip(JNIEnv *env, jobject is_object, 
					jlong bytes)
{
    int fd;

#ifdef DEBUG_djm
    fprintf(stderr, 
	    "in FileInputStream.skip(), object is %p, bytes are %Ld...\n", 
	    is_object, bytes);
#endif
    fd = getFD(env, is_object);
#ifdef DEBUG_djm
    fprintf(stderr, "got fd %d in FileInputStream.skip()...\n", fd);
#endif
    if(lseek(fd, (off_t)bytes, SEEK_CUR) < 0) {
	water_throwException(WATER_ENV(env), "java/lang/IOException");
	return -1;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "done with FileInputStream.skip().\n");
#endif
    return bytes;
}

jint Java_java_io_FileInputStream_available(JNIEnv *env, jobject is_object)
{
    /* XXX */
    return 0;
}

void Java_java_io_FileInputStream_close(JNIEnv *env, jobject is_object)
{
    int fd = getFD(env, is_object);
    close(fd);
}

void Java_java_io_FileOutputStream_open(JNIEnv *env, jobject is_object, 
					jobject filename_string)
{    
    openFile(env, is_object, filename_string, O_WRONLY | O_CREAT);
}

void Java_java_io_FileOutputStream_openAppend(JNIEnv *env, jobject is_object, 
					      jobject filename_string)
{    
    openFile(env, is_object, filename_string, O_WRONLY | O_CREAT | O_APPEND);
}

void Java_java_io_FileOutputStream_write(JNIEnv *env, jobject is_object, 
					 jint ch)
{
    int fd = getFD(env, is_object);
    jbyte byte = ch;

    write(fd, &byte, 1);
}

void Java_java_io_FileOutputStream_writeBytes(JNIEnv *env, jobject os_object,
					      jobject byte_array,
					      jint start_byte,
					      jint num_bytes)
{
    int fd = getFD(env, os_object);

#if defined(DEBUG_djm)
    int array_length;
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(byte_array), 0);
    assert(start_byte + num_bytes <= array_length);
#endif

    /* XXX should check the result and throw exceptions if necessary here */
    write(fd, ARRAY_ELEMENT_PTR(byte_array, start_byte), num_bytes);
}

void Java_java_io_FileOutputStream_close(JNIEnv *env, jobject is_object)
{
    int fd = getFD(env, is_object);
    close(fd);
}

static unsigned char *getFilenameFromFile(JNIEnv *env, jobject file_object)
{
    jfieldID path_field;
    jobject path_string;

    path_field = (*env)->GetFieldID(env, 
				    (*env)->GetObjectClass(env, file_object),
				    "path",
				    "Ljava/lang/String;");
    if(!path_field) {
	assert("path field not found" && 0);
	return 0;
    }
    path_string = (*env)->GetObjectField(env, file_object, path_field);
    if(!path_string) {
	assert("no path string" && 0);
	return 0;
    }

    return water_getStringBytes(env, path_string);
}

static void openFile(JNIEnv *env, jobject is_object, jobject filename_string,
		     int flags)
{
    unsigned char *filename;
    int fd;

    filename = water_getStringBytes(env, filename_string);
    if(!filename) {
	return;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "openFile() is opening '%s'...\n", filename);
#endif
    fd = open(filename, flags, 0666);
#ifdef DEBUG_djm
    fprintf(stderr, "openFile() opened '%s' as fd %d.\n", filename, fd);
#endif
    free(filename);
    if(fd < 0) {
	water_throwException(WATER_ENV(env), "java/io/FileNotFoundException");
	return;
    }

    storeFD(env, is_object, fd);
}

static void storeFD(JNIEnv *env, jobject stream_object, int fd)
{
    jfieldID fd_object_field, fd_field;
    jobject fd_object;

    fd_object_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, stream_object),
			   "fd", "Ljava/io/FileDescriptor;");
    assert(fd_object_field);
    fd_field =
	(*env)->GetFieldID(env, 
			   (*env)->FindClass(env, "java/io/FileDescriptor"),
			   "fd", "I");
    assert(fd_field);
    fd_object = (*env)->GetObjectField(env, stream_object, fd_object_field);
    assert(fd_object);

    (*env)->SetIntField(env, fd_object, fd_field, fd);
}

static int getFD(JNIEnv *env, jobject stream_object)
{
    jfieldID fd_object_field, fd_field;
    jobject fd_object;
   
    fd_object_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, stream_object),
			   "fd", "Ljava/io/FileDescriptor;");
    assert(fd_object_field);
    fd_field =
	(*env)->GetFieldID(env, 
			   (*env)->FindClass(env, "java/io/FileDescriptor"),
			   "fd", "I");
    assert(fd_field);
    fd_object = (*env)->GetObjectField(env, stream_object, fd_object_field);
    assert(fd_object);

    return (*env)->GetIntField(env, fd_object, fd_field);
}

void Java_java_io_RandomAccessFile_open(JNIEnv *env, jobject file_object,
					jobject filename_string,
					jboolean allow_writes)
{
    openFile(env, file_object, filename_string,
	     (allow_writes == JNI_TRUE) ? (O_RDWR | O_CREAT) : O_RDONLY);
}

jint Java_java_io_RandomAccessFile_read(JNIEnv *env, jobject file_object)
{
    /* exactly the same as for a FileInputStream */
    return Java_java_io_FileInputStream_read(env, file_object);
}

jint Java_java_io_RandomAccessFile_readBytes(JNIEnv *env, jobject file_object,
					     jbyteArray bytes, 
					     jint start_byte, jint num_bytes)
{
    /* exactly the same as for a FileInputStream */
    return Java_java_io_FileInputStream_readBytes(env, file_object,
						  bytes, 
						  start_byte, num_bytes);
}

void Java_java_io_RandomAccessFile_write(JNIEnv *env, jobject file_object, 
					 jint ch)
{
    /* more of the same */
    return Java_java_io_FileOutputStream_write(env, file_object, ch);
}

void Java_java_io_RandomAccessFile_writeBytes(JNIEnv *env, jobject file_object,
					      jobject byte_array,
					      jint start_byte,
					      jint num_bytes)
{
    return Java_java_io_FileOutputStream_writeBytes(env, file_object,
						    byte_array,
						    start_byte, num_bytes);
}

void Java_java_io_RandomAccessFile_close(JNIEnv *env, jobject file_object)
{
    int fd = getFD(env, file_object);
    close(fd);
}

jlong Java_java_io_RandomAccessFile_getFilePointer(JNIEnv *env, 
						   jobject file_object)
{
    int fd = getFD(env, file_object);
    jlong result;

    result = lseek(fd, 0, SEEK_CUR);
    if(result < 0) {
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return 0;
    }

    return result;
}

void Java_java_io_RandomAccessFile_seek(JNIEnv *env, jobject file_object,
					jlong offset)
{
    int fd = getFD(env, file_object);

    if(lseek(fd, (off_t)offset, SEEK_SET) < 0) { 
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return;
    }
}

jlong Java_java_io_RandomAccessFile_length(JNIEnv *env, jobject file_object)
{
    int fd = getFD(env, file_object);
    off_t orig_pos, length;

    orig_pos = lseek(fd, 0, SEEK_CUR);
    if(orig_pos < 0) {
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return 0;
    }
      
    length = lseek(fd, 0, SEEK_END);
    
    if(length < 0) {
	/* at least try to put the correct position back */
	lseek(fd, orig_pos, SEEK_SET);
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return 0;
    }

    if(lseek(fd, orig_pos, SEEK_SET) < 0) {
	water_throwException(WATER_ENV(env), "java/io/IOException");
	return 0;
    }

    return (jlong)length;
}
