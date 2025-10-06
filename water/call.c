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
#include "call.h"
#include "exec.h"
#include "classes.h"
#include "objects.h"
#include "stack.h"
#include "native-asm.h"
#include <string.h>
#include <assert.h>

/* internal routines */
static void callJavaMethod(water_JNIEnv *wenv, waterObject *object,
			   waterMethod *method, 
			   int args_already_pushed, va_list ap);
static void callNativeMethod(water_JNIEnv *wenv, waterObject *object, 
			     waterMethod *method,
			     int args_need_pushing, va_list ap);
static void pushArguments(water_JNIEnv *wenv, waterObject *object, 
			  waterMethod *method, va_list ap);
static void handleUncaughtException(water_JNIEnv *wenv);


/* the regular virtual method calls */
#define CALL_METHOD_DECL_1(type, name)					\
    type name(JNIEnv *env, jobject j_obj, jmethodID j_method, ...)	\
    {									\
        va_list ap;							\
        type result;							\
									\
        va_start(ap, j_method);						\
        result = name ## V (env, j_obj, j_method, ap);			\
        va_end(ap);							\
									\
        return result;							\
    }									\
    type name ## A (JNIEnv *env, jobject j_obj,				\
		    jmethodID j_method, jvalue *args)			\
    {									\
        /* XXX */							\
        assert(0);							\
	abort();							\
    }

#define CALL_METHOD_DECL_2(type, stacktype, name)			\
    type name ## V (JNIEnv *env, jobject j_obj,				\
		    jmethodID j_method, va_list ap)			\
    {									\
	type result;							\
									\
	/* invoke virtually */						\
	water_callMethod(((water_JNIEnv *)env), WATER_OBJECT(j_obj),	\
			 &WATER_OBJECT(j_obj)->class->			\
			 method_table[(WATER_METHOD(j_method))->index],	\
			 WATER_ARGS_NEED_PUSHING, ap);			\
									\
	if(((water_JNIEnv *)env)->exception_thrown &&			\
	   ((water_JNIEnv *)env)->call_depth == 0)			\
	    handleUncaughtException(((water_JNIEnv *)env));		\
									\
	opstack_pop_ ## stacktype (((water_JNIEnv *)env)->opstack_top,	\
				   &result);				\
	return result;							\
    }

#define CALL_METHOD_DECL_2_EXTEND(type, stacknativetype, stacktype, name) \
    type name ## V (JNIEnv *env, jobject j_obj,				  \
		    jmethodID j_method, va_list ap)			  \
    {									  \
        stacknativetype stack_result;					  \
	type real_result;						  \
									  \
	/* invoke virtually */						  \
	water_callMethod(((water_JNIEnv *)env), WATER_OBJECT(j_obj),	  \
			 &WATER_OBJECT(j_obj)->class->			  \
			 method_table[(WATER_METHOD(j_method))->index],	  \
			 WATER_ARGS_NEED_PUSHING, ap);			  \
									  \
	if(((water_JNIEnv *)env)->exception_thrown &&			  \
	   ((water_JNIEnv *)env)->call_depth == 0)			  \
	    handleUncaughtException(((water_JNIEnv *)env));		  \
									  \
	opstack_pop_ ## stacktype (((water_JNIEnv *)env)->opstack_top,	  \
		 		   &stack_result);			  \
        real_result = stack_result;					  \
	return real_result;						  \
    }

CALL_METHOD_DECL_1(jobject, water_jni_CallObjectMethod)
CALL_METHOD_DECL_2(jobject, reference, water_jni_CallObjectMethod)
CALL_METHOD_DECL_1(jboolean, water_jni_CallBooleanMethod)
CALL_METHOD_DECL_2_EXTEND(jboolean, jint, int, water_jni_CallBooleanMethod)
CALL_METHOD_DECL_1(jbyte, water_jni_CallByteMethod)
CALL_METHOD_DECL_2_EXTEND(jbyte, jint, int, water_jni_CallByteMethod)
CALL_METHOD_DECL_1(jchar, water_jni_CallCharMethod)
CALL_METHOD_DECL_2_EXTEND(jchar, jint, int, water_jni_CallCharMethod)
CALL_METHOD_DECL_1(jshort, water_jni_CallShortMethod)
CALL_METHOD_DECL_2_EXTEND(jshort, jint, int, water_jni_CallShortMethod)
CALL_METHOD_DECL_1(jint, water_jni_CallIntMethod)
CALL_METHOD_DECL_2(jint, int, water_jni_CallIntMethod)
CALL_METHOD_DECL_1(jlong, water_jni_CallLongMethod)
CALL_METHOD_DECL_2(jlong, int, water_jni_CallLongMethod)
CALL_METHOD_DECL_1(jfloat, water_jni_CallFloatMethod)
CALL_METHOD_DECL_2(jfloat, int, water_jni_CallFloatMethod)
CALL_METHOD_DECL_1(jdouble, water_jni_CallDoubleMethod)
CALL_METHOD_DECL_2(jdouble, int, water_jni_CallDoubleMethod)

#undef CALL_METHOD_DECL_1
#undef CALL_METHOD_DECL_2
#undef CALL_METHOD_DECL_2_EXTEND


/* static method calls */
#define CALL_METHOD_DECL_1(type, name)					\
    type name(JNIEnv *env, jclass clazz, jmethodID j_method, ...)	\
    {									\
        va_list ap;							\
        type result;							\
									\
        va_start(ap, j_method);						\
        result = name ## V (env, clazz, j_method, ap);			\
        va_end(ap);							\
									\
        return result;							\
    }									\
    type name ## A (JNIEnv *env, jclass clazz,				\
		    jmethodID j_method, jvalue *args)			\
    {									\
        /* XXX */							\
        assert(0);							\
	abort();							\
    }

#define CALL_METHOD_DECL_2(type, stacktype, name)		\
    type name ## V (JNIEnv *env, jclass clazz,			\
		    jmethodID j_method, va_list ap)		\
    {								\
	type result;						\
								\
	/* static invocation */					\
	water_callMethod(WATER_ENV(env), 0,			\
			 (WATER_METHOD(j_method)),		\
			 WATER_ARGS_NEED_PUSHING, ap);		\
								\
	if(WATER_ENV(env)->exception_thrown &&			\
	   WATER_ENV(env)->call_depth == 0)			\
	    handleUncaughtException(WATER_ENV(env));		\
								\
	opstack_pop_ ## stacktype (WATER_ENV(env)->opstack_top,	\
				   &result);			\
	return result;						\
    }

#define CALL_METHOD_DECL_2_EXTEND(type, stacknativetype, stacktype, name) \
    type name ## V (JNIEnv *env, jclass clazz,				  \
		    jmethodID j_method, va_list ap)			  \
    {									  \
        stacknativetype stack_result;					  \
	type real_result;						  \
									  \
	/* static invocation */						  \
	water_callMethod(WATER_ENV(env), 0,				  \
			 (WATER_METHOD(j_method)),			  \
			 WATER_ARGS_NEED_PUSHING, ap);			  \
									  \
	if(WATER_ENV(env)->exception_thrown &&				  \
	   WATER_ENV(env)->call_depth == 0)				  \
	    handleUncaughtException(WATER_ENV(env));			  \
									  \
	opstack_pop_ ## stacktype (WATER_ENV(env)->opstack_top,		  \
				   &stack_result);			  \
        real_result = stack_result;					  \
	return real_result;						  \
    }

CALL_METHOD_DECL_1(jobject, water_jni_CallStaticObjectMethod)
CALL_METHOD_DECL_2(jobject, reference, water_jni_CallStaticObjectMethod)
CALL_METHOD_DECL_1(jboolean, water_jni_CallStaticBooleanMethod)
CALL_METHOD_DECL_2_EXTEND(jboolean, jint, int, 
			  water_jni_CallStaticBooleanMethod)
CALL_METHOD_DECL_1(jbyte, water_jni_CallStaticByteMethod)
CALL_METHOD_DECL_2_EXTEND(jbyte, jint, int, water_jni_CallStaticByteMethod)
CALL_METHOD_DECL_1(jchar, water_jni_CallStaticCharMethod)
CALL_METHOD_DECL_2_EXTEND(jchar, jint, int, water_jni_CallStaticCharMethod)
CALL_METHOD_DECL_1(jshort, water_jni_CallStaticShortMethod)
CALL_METHOD_DECL_2_EXTEND(jshort, jint, int, water_jni_CallStaticShortMethod)
CALL_METHOD_DECL_1(jint, water_jni_CallStaticIntMethod)
CALL_METHOD_DECL_2(jint, int, water_jni_CallStaticIntMethod)
CALL_METHOD_DECL_1(jlong, water_jni_CallStaticLongMethod)
CALL_METHOD_DECL_2(jlong, int, water_jni_CallStaticLongMethod)
CALL_METHOD_DECL_1(jfloat, water_jni_CallStaticFloatMethod)
CALL_METHOD_DECL_2(jfloat, int, water_jni_CallStaticFloatMethod)
CALL_METHOD_DECL_1(jdouble, water_jni_CallStaticDoubleMethod)
CALL_METHOD_DECL_2(jdouble, int, water_jni_CallStaticDoubleMethod)

#undef CALL_METHOD_DECL_1
#undef CALL_METHOD_DECL_2
#undef CALL_METHOD_DECL_2_EXTEND


/* the nonvirtual 'special' method calls */
#define CALL_METHOD_DECL_1(type, name)					\
    type name(JNIEnv *env, jobject j_obj, jmethodID j_method, ...)	\
    {									\
        va_list ap;							\
        type result;							\
									\
        va_start(ap, j_method);						\
        result = name ## V (env, j_obj, j_method, ap);			\
        va_end(ap);							\
									\
        return result;							\
    }									\
    type name ## A (JNIEnv *env, jobject j_obj,				\
		    jmethodID j_method, jvalue *args)			\
    {									\
        /* XXX */							\
        assert(0);							\
	abort();							\
    }

#define CALL_METHOD_DECL_2(type, stacktype, name)		\
    type name ## V (JNIEnv *env, jobject j_obj,			\
		    jmethodID j_method, va_list ap)		\
    {								\
	type result;						\
								\
	/* invoke nonvirtually--use the exact method given */	\
	water_callMethod(WATER_ENV(env), WATER_OBJECT(j_obj),	\
			 WATER_METHOD(j_method),		\
			 WATER_ARGS_NEED_PUSHING, ap);		\
								\
	if(WATER_ENV(env)->exception_thrown &&			\
	   WATER_ENV(env)->call_depth == 0)			\
	    handleUncaughtException(WATER_ENV(env));		\
								\
	opstack_pop_ ## stacktype(WATER_ENV(env)->opstack_top,	\
				  &result);			\
	return result;						\
    }

#define CALL_METHOD_DECL_2_EXTEND(type, stacknativetype, stacktype, name) \
    type name ## V (JNIEnv *env, jobject j_obj,				  \
		    jmethodID j_method, va_list ap)			  \
    {									  \
        stacknativetype stack_result;					  \
	type real_result;						  \
									  \
	/* invoke nonvirtually--use the exact method given */		  \
	water_callMethod(WATER_ENV(env), WATER_OBJECT(j_obj),		  \
			 WATER_METHOD(j_method),			  \
			 WATER_ARGS_NEED_PUSHING, ap);			  \
									  \
	if(WATER_ENV(env)->exception_thrown &&				  \
	   WATER_ENV(env)->call_depth == 0)				  \
	    handleUncaughtException(WATER_ENV(env));			  \
									  \
	opstack_pop_ ## stacktype (WATER_ENV(env)->opstack_top,		  \
				   &stack_result);			  \
        real_result = stack_result;					  \
	return real_result;						  \
    }

CALL_METHOD_DECL_1(jobject, water_jni_CallNonvirtualObjectMethod)
CALL_METHOD_DECL_2(jobject, reference, water_jni_CallNonvirtualObjectMethod)
CALL_METHOD_DECL_1(jboolean, water_jni_CallNonvirtualBooleanMethod)
CALL_METHOD_DECL_2_EXTEND(jboolean, jint, int, 
			  water_jni_CallNonvirtualBooleanMethod)
CALL_METHOD_DECL_1(jbyte, water_jni_CallNonvirtualByteMethod)
CALL_METHOD_DECL_2_EXTEND(jbyte, jint, int, water_jni_CallNonvirtualByteMethod)
CALL_METHOD_DECL_1(jchar, water_jni_CallNonvirtualCharMethod)
CALL_METHOD_DECL_2_EXTEND(jchar, jint, int, water_jni_CallNonvirtualCharMethod)
CALL_METHOD_DECL_1(jshort, water_jni_CallNonvirtualShortMethod)
CALL_METHOD_DECL_2_EXTEND(jshort, jint, int, 
			  water_jni_CallNonvirtualShortMethod)
CALL_METHOD_DECL_1(jint, water_jni_CallNonvirtualIntMethod)
CALL_METHOD_DECL_2(jint, int, water_jni_CallNonvirtualIntMethod)
CALL_METHOD_DECL_1(jlong, water_jni_CallNonvirtualLongMethod)
CALL_METHOD_DECL_2(jlong, int, water_jni_CallNonvirtualLongMethod)
CALL_METHOD_DECL_1(jfloat, water_jni_CallNonvirtualFloatMethod)
CALL_METHOD_DECL_2(jfloat, int, water_jni_CallNonvirtualFloatMethod)
CALL_METHOD_DECL_1(jdouble, water_jni_CallNonvirtualDoubleMethod)
CALL_METHOD_DECL_2(jdouble, int, water_jni_CallNonvirtualDoubleMethod)

#undef CALL_METHOD_DECL_1
#undef CALL_METHOD_DECL_2
#undef CALL_METHOD_DECL_2_EXTEND


void water_jni_CallVoidMethod(JNIEnv *env, jobject j_obj, 
			      jmethodID j_method, ...)
{
    va_list ap;

    va_start(ap, j_method);
    water_jni_CallVoidMethodV(env, j_obj, j_method, ap);
    va_end(ap);
}

void water_jni_CallVoidMethodA(JNIEnv *env, jobject j_obj,
			       jmethodID j_method, jvalue *args)
{
    /* XXX */
    assert(0);
    abort();
}

void water_jni_CallVoidMethodV(JNIEnv *env, jobject j_obj,
			       jmethodID j_method, va_list ap)
{
    /* invoke virtually */                                            
    water_callMethod(WATER_ENV(env), WATER_OBJECT(j_obj),   
		     &WATER_OBJECT(j_obj)->class->                 
		     method_table[(WATER_METHOD(j_method))->index],
		     WATER_ARGS_NEED_PUSHING, ap);                    
    
    if(WATER_ENV(env)->exception_thrown &&                     
       WATER_ENV(env)->call_depth == 0)                        
	handleUncaughtException(WATER_ENV(env));               
}


void water_jni_CallStaticVoidMethod(JNIEnv *env, jclass j_class,
				    jmethodID j_method, ...)
{
    va_list ap;

    va_start(ap, j_method);
    water_jni_CallStaticVoidMethodV(env, j_class, j_method, ap);
    va_end(ap);
}

void water_jni_CallStaticVoidMethodA(JNIEnv *env, jclass j_class,
				     jmethodID j_method, jvalue *args)
{
    /* XXX */
    assert(0);
    abort();
}

void water_jni_CallStaticVoidMethodV(JNIEnv *env, jclass j_class,
				     jmethodID j_method, va_list ap)
{
    /* static invocation */                                           
    water_callMethod(WATER_ENV(env), 0,                          
		     (WATER_METHOD(j_method)),                       
		     WATER_ARGS_NEED_PUSHING, ap);                    
    
    if(WATER_ENV(env)->exception_thrown &&                     
       WATER_ENV(env)->call_depth == 0)                        
	handleUncaughtException(WATER_ENV(env));               
}


void water_jni_CallNonvirtualVoidMethod(JNIEnv *env, jobject j_obj, 
					jmethodID j_method, ...)
{
    va_list ap;

    va_start(ap, j_method);
    water_jni_CallNonvirtualVoidMethodV(env, j_obj, j_method, ap);
    va_end(ap);
}

void water_jni_CallNonvirtualVoidMethodA(JNIEnv *env, jobject j_obj,
					 jmethodID j_method, jvalue *args)
{
    /* XXX */
    assert(0);
    abort();
}

void water_jni_CallNonvirtualVoidMethodV(JNIEnv *env, jobject j_obj,
					 jmethodID j_method, va_list ap)
{
    /* invoke nonvirtually--use the exact method given */             
    water_callMethod(WATER_ENV(env), WATER_OBJECT(j_obj),   
		     WATER_METHOD(j_method), WATER_ARGS_NEED_PUSHING, ap);
    
    if(WATER_ENV(env)->exception_thrown &&                     
       WATER_ENV(env)->call_depth == 0)                        
	handleUncaughtException(WATER_ENV(env));               
}


/* XXX this should really take the jvalue * version, because  */
/* we can convert the va_list to a jvalue array, but we can't */
/* go the other way.                                          */
void water_callMethod(water_JNIEnv *wenv, waterObject *object,
		      waterMethod *method, 
		      int args_already_pushed, va_list ap)
{
    assert(method != 0);

    /* we should always start a method with any exceptions cleared. */
    wenv->exception_thrown = 0;
    wenv->call_depth++;
    if(method->access_flags & ACC_NATIVE) {
	callNativeMethod(wenv, object, method, args_already_pushed, ap);
    } else {
	callJavaMethod(wenv, object, method, args_already_pushed, ap);
    }
    wenv->call_depth--;
}

static void callJavaMethod(water_JNIEnv *wenv, waterObject *object,
			   waterMethod *method, 
			   int args_already_pushed, va_list ap)
{
#define SHOW_CALLS
    unsigned char *old_frame_top = wenv->frame_top;
    jvalue result;
    int exception_return = 0;
    int i;
    PRMonitor *monitor = 0;
#if defined(DEBUG_djm)
    waterUTF8String *class_name = 
	water_getClassName(method->childmost_class,
			   method->childmost_class->this_class);
    unsigned char *class_name_normal = water_UTF8ToNormal(class_name);
    unsigned char *method_name_normal = water_UTF8ToNormal(method->name);
    unsigned char *descriptor_normal = water_UTF8ToNormal(method->descriptor);
    unsigned char *debug_orig_frame_top = wenv->frame_top;
    waterObject *debug_orig_running_object = wenv->running_object;
    waterMethod *debug_orig_running_method = wenv->running_method;
    int debug_orig_pc = wenv->pc;

    unsigned char *debug_orig_opstack_top = wenv->opstack_top;
#ifdef SHOW_CALLS
    static char *should_show_calls = 0;
    static int should_initialized = 0;

    if(!should_initialized) {
	should_show_calls = getenv("WATER_SHOWCALLS");
	should_initialized = 1;
    }
#endif
#endif

    assert(wenv != 0);
    /* object == 0 is OK for a static method */
    assert(method != 0);
    assert(args_already_pushed == WATER_ARGS_ALREADY_PUSHED ||
	   args_already_pushed == WATER_ARGS_NEED_PUSHING);
    
#if defined(DEBUG_djm) && defined(SHOW_CALLS)
    if(should_show_calls) {
	fprintf(stderr, "c: %d callMethod (%s:%s:%s) called with args "
		"class/object/method: %p/%p/%p\n",
		wenv->call_depth, 
		class_name_normal, method_name_normal, descriptor_normal,
		method->class, object, method);
    }
#endif

    if(method->access_flags & ACC_SYNCHRONIZED) {
	/* for instance methods, use the object's monitor. */
	/* for static methods, use the class's monitor. */
	if(object) {
	    monitor = object->monitor;
	} else {
	    monitor = method->class->monitor;
	}

	PR_EnterMonitor(monitor);
    }

    if(args_already_pushed == WATER_ARGS_ALREADY_PUSHED) {
	wenv->frame_top = wenv->opstack_top - method->args_size;
	assert(wenv->frame_top >= wenv->stack_top);
    } else {
	wenv->frame_top = wenv->opstack_top;

	pushArguments(wenv, object, method, ap);
    }

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "callMethod (%s:%s:%s) frame is at %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->frame_top);
#endif

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "callMethod (%s:%s:%s) args end at %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->opstack_top);
#endif

    /* leave space for locals */
    memset(wenv->opstack_top, 0, method->locals_size);
    wenv->opstack_top += method->locals_size;

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "callMethod (%s:%s:%s) locals end at %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->opstack_top);
#endif

    /* stuff we need to save -- frame pointer, old running class, object, */
    /* method, and pc.  (object+method+pc are all needed to get           */
    /* the same effect as just the pc on a normal machine.)               */

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "callMethod (%s:%s:%s) before call: "
	    "frame/object/method/pc are: %p/%p/%p/%d\n",
	    class_name_normal, method_name_normal, descriptor_normal,
	    old_frame_top, wenv->running_object, 
	    wenv->running_method, wenv->pc);
#endif
    opstack_push_reference(wenv->opstack_top, &old_frame_top);
    opstack_push_reference(wenv->opstack_top, &wenv->running_object);
    opstack_push_reference(wenv->opstack_top, &wenv->running_method);
    opstack_push_int(wenv->opstack_top, &wenv->pc);
#if defined(DEBUG_djm) && 0
    fprintf(stderr,
	    "callMethod (%s:%s:%s) after pushing args stack pointer is %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->opstack_top);
#endif
#if defined(DEBUG_djm) && 0
    fprintf(stderr, 
	    "callMethod (%s:%s:%s) before running frame pointer is %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->frame_top);
#endif

    /* ready to rock and roll. */
    wenv->running_object = object;
    wenv->running_method = method;
    wenv->pc = 0;
    water_runByteCode(wenv);

    /* check for an exception.  if there's a handler in the current method,  */
    /* we want to stay in the current frame and just change the PC.  if not, */
    /* we need to pop the frame, push the exception reference, and return to */
    /* the caller.                                                           */
    while(wenv->exception_thrown) {
	waterObject *exception_object;
	int exception_handled = 0;

	/* clear the exception flag while the handler runs.  only reset */
	/* the flag afterward if we can't find a handler.               */
	/* (the handler itself may throw an exception, too.)            */
	wenv->exception_thrown = 0;
	
	/* XXX don't pop and push */
	opstack_pop_reference(wenv->opstack_top, &exception_object);
	opstack_push_reference(wenv->opstack_top, &exception_object);
	assert(exception_object->class->minor_version == 3);

	for(i = 0; i < method->code->exception_table_length; i++) {
	    waterExceptionInfo *exception_info =
		&method->code->exception_table[i];

	    /* catching a null class means catch everything--for the */
	    /* 'finally' clause.                                     */
	    if(exception_info->catch_type != 0 &&
	       !exception_info->resolved_catch_type) {
		exception_info->resolved_catch_type = 
		    water_resolveClassByIndex((JNIEnv *)wenv,
					      method->class,
					      exception_info->catch_type,
					      CLASS_VERIFIED);
		if(!exception_info->resolved_catch_type) {
		    /* XXX what do to now?  if we throw another exception, */
		    /* we'll enter an infinite loop.                       */
		    
		    /* probably should throw another exception, and then   */
		    /* return directly to the caller without looking at    */
		    /* this method's exception handlers anymore.           */
		    assert(0);
		    abort();
		}
	    }

	    if((exception_info->catch_type == 0 ||
		water_isSuperclass((JNIEnv *)wenv,
				   exception_info->resolved_catch_type,
				   exception_object->class)) &&
	       wenv->pc >= exception_info->start_pc &&
	       wenv->pc < exception_info->end_pc) {
		/* found a handler. */
		exception_handled = 1;
		i = method->code->exception_table_length;

		wenv->pc = exception_info->handler_pc;
		water_runByteCode(wenv);
	    }
	}

	if(!exception_handled) {
	    wenv->exception_thrown = 1;
	    exception_return = 1;
	    break;
	}
    }

    /* get return value */
    if(exception_return) {
	/* the exception will be pushed onto the caller's stack. */
	opstack_pop_reference(wenv->opstack_top, &result);
    } else {
	if(method->return_type != JNI_NONE) {
	    if(method->return_type == JNI_LONG ||
	       method->return_type == JNI_DOUBLE) {
		opstack_pop_double_word(wenv->opstack_top, &result);
	    } else {
		opstack_pop_word(wenv->opstack_top, &result);
	    }
	}
    }

#if defined(DEBUG_djm) && 0
    fprintf(stderr, 
	    "callMethod (%s:%s:%s) after running frame pointer is %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->frame_top);
#endif
    wenv->opstack_top = 
	wenv->frame_top + method->args_size + method->locals_size + 4*4;
#if defined(DEBUG_djm) && 0
    fprintf(stderr,
	    "callMethod (%s:%s:%s) before popping args stack pointer is %p\n", 
	    class_name_normal, method_name_normal, descriptor_normal,
	    wenv->opstack_top);
#endif
    opstack_pop_int(wenv->opstack_top, &wenv->pc);
    opstack_pop_reference(wenv->opstack_top, &wenv->running_method);
    opstack_pop_reference(wenv->opstack_top, &wenv->running_object);
    opstack_pop_reference(wenv->opstack_top, &wenv->frame_top);

    /* unroll the arguments and locals */
    wenv->opstack_top -= (method->args_size + method->locals_size);

#if defined(DEBUG_djm) && 0
    fprintf(stderr,
	    "callMethod (%s:%s:%s) after call: "
	    "frame/object/method/pc are: %p/%p/%p/%d\n",
	    class_name_normal, method_name_normal, descriptor_normal,
	    old_frame_top, wenv->running_object, 
	    wenv->running_method, wenv->pc);
#endif

#ifdef DEBUG_djm
    /* make sure the stack pointer is back in the same place */
    if(args_already_pushed == WATER_ARGS_ALREADY_PUSHED) {
	assert(wenv->opstack_top == 
	       (debug_orig_opstack_top - method->args_size));
    } else {
	assert(wenv->opstack_top == debug_orig_opstack_top);
    }
#endif

    /* push return value */
    if(exception_return) {
	opstack_push_reference(wenv->opstack_top, &result);
    } else {
	if(method->return_type != JNI_NONE) {
	    if(method->return_type == JNI_LONG ||
	       method->return_type == JNI_DOUBLE) {
		opstack_push_double_word(wenv->opstack_top, &result);
	    } else {
		opstack_push_word(wenv->opstack_top, &result);
	    }
	}
    }

    if(method->access_flags & ACC_SYNCHRONIZED) {
	PR_ExitMonitor(monitor);
    }

#ifdef DEBUG_djm    
    /* make sure we restored the old frame properly */
    assert(wenv->frame_top == debug_orig_frame_top);
    assert(wenv->running_object == debug_orig_running_object);
    assert(wenv->running_method == debug_orig_running_method);
    assert(wenv->pc == debug_orig_pc);
#endif

#if defined(DEBUG_djm) && defined(SHOW_CALLS)
    if(should_show_calls) {
	fprintf(stderr, "c: %d callMethod completed (%s:%s:%s) with args "
		"class/object/method: %p/%p/%p\n",
		wenv->call_depth, 
		class_name_normal, method_name_normal, descriptor_normal,
		method->class, object, method);
    }
#endif

#ifdef DEBUG_djm
    free(descriptor_normal);
    free(method_name_normal);
    free(class_name_normal);
#endif

}

static void callNativeMethod(water_JNIEnv *wenv, waterObject *object, 
			     waterMethod *method,
			     int args_already_pushed, va_list ap)
{
    unsigned char *old_frame_top;
    int exception_return = 0;
    waterObject *exception_object;
    jvalue result;
#ifdef DEBUG_djm
    unsigned char *debug_orig_frame_top = wenv->frame_top;
    waterObject *debug_orig_running_object = wenv->running_object;
    waterMethod *debug_orig_running_method = wenv->running_method;
    int debug_orig_pc = wenv->pc;

    unsigned char *debug_orig_opstack_top = wenv->opstack_top;
#endif

    if(!method->native_code) {
	water_linkNativeMethod((JNIEnv *)wenv, method);
	if(!method->native_code) {
	    fprintf(stderr, "can't find code for native method.\n");
	    assert(0);
	    abort();
	}
    }

#if defined(DEBUG_djm) && 0
    fprintf(stderr, "running native method at %p\n", method->native_code);
#endif

    old_frame_top = wenv->frame_top;
    if(args_already_pushed == WATER_ARGS_ALREADY_PUSHED) {
	wenv->frame_top = wenv->opstack_top - method->args_size;
    } else {
	wenv->frame_top = wenv->opstack_top;
	pushArguments(wenv, object, method, ap);
    }

    /* create a stack frame for these babies, too */
    opstack_push_reference(wenv->opstack_top, &old_frame_top);
    opstack_push_reference(wenv->opstack_top, &wenv->running_object);
    opstack_push_reference(wenv->opstack_top, &wenv->running_method);
    opstack_push_int(wenv->opstack_top, &wenv->pc);

    wenv->running_method = method;
    wenv->running_object = object;
    wenv->pc = 0;

    if(object == 0) {
	/* we have to do some strangeness here to handle static methods */
	/* properly.  the JNI routines expect to be passed a pointer    */
	/* to the class instead of a 'this' pointer for static methods. */
	/* (so they can access the static fields in the class, which    */
	/* otherwise they wouldn't be able to easily.)  the problem is, */
	/* the 'this' pointer is already counted as one of the normal   */
	/* arguments for non-static methods.  so we can't just treat    */
	/* the class pointer and the this pointer the same way.         */

	/* what we do is tell water_asmCallNative that the arguments    */
	/* start after the this pointer for non-statics, and then just  */
	/* give it the this pointer ourselves.                          */

	water_asmCallNative((JNIEnv *)wenv, method->class, 
			    method->native_code,
			    wenv->opstack_top - method->args_size - 16, 
			    method->args_size,
			    wenv->opstack_top, method->return_type,
			    &wenv->exception_thrown);
    } else {
	water_asmCallNative((JNIEnv *)wenv, object, 
			    method->native_code,
			    wenv->opstack_top - method->args_size + 4 - 16, 
			    method->args_size - 4,
			    wenv->opstack_top, method->return_type,
			    &wenv->exception_thrown);
    }

    if(wenv->exception_thrown) {
	/* if we still have an exception here, it means the native method */
	/* didn't want to deal with it.  just pop the exception from the  */
	/* stack and push it as the result.                               */
	exception_return = 1;
	opstack_pop_reference(wenv->opstack_top, &exception_object);
    } else {
	/* natives don't increment the stack pointer, they just write the */
	/* results.  so if we want to pop, we have to increment first.    */
	if(method->return_type != JNI_NONE) {
	    if(method->return_type == JNI_LONG ||
	       method->return_type == JNI_DOUBLE) {
		wenv->opstack_top += 8;
		opstack_pop_double_word(wenv->opstack_top, &result);
	    } else {
		wenv->opstack_top += 4;
		opstack_pop_word(wenv->opstack_top, &result);
	    }
	}
    }

    wenv->opstack_top = wenv->frame_top + method->args_size + 4*4;
    opstack_pop_int(wenv->opstack_top, &wenv->pc);
    opstack_pop_reference(wenv->opstack_top, &wenv->running_method);
    opstack_pop_reference(wenv->opstack_top, &wenv->running_object);
    opstack_pop_reference(wenv->opstack_top, &wenv->frame_top);

    /* kill the arguments */
    wenv->opstack_top -= method->args_size;

#ifdef DEBUG_djm
    /* make sure the stack pointer is back in the same place */
    if(args_already_pushed == WATER_ARGS_ALREADY_PUSHED) {
	assert(wenv->opstack_top == 
	       (debug_orig_opstack_top - method->args_size));
    } else {
	assert(wenv->opstack_top == debug_orig_opstack_top);
    }
#endif

    /* push the result if necessary */
    if(exception_return) {
	/* the exception will be pushed onto the caller's stack. */
	opstack_push_reference(wenv->opstack_top, &exception_object);
    } else {
	if(method->return_type == JNI_DOUBLE ||
	   method->return_type == JNI_LONG) {
	    opstack_push_double_word(wenv->opstack_top, &result);
	} else if(method->return_type != JNI_NONE) {
	    opstack_push_word(wenv->opstack_top, &result);
	}
    }

#ifdef DEBUG_djm    
    /* make sure we restored the old frame properly */
    assert(wenv->frame_top == debug_orig_frame_top);
    assert(wenv->running_object == debug_orig_running_object);
    assert(wenv->running_method == debug_orig_running_method);
    assert(wenv->pc == debug_orig_pc);
#endif
}

static void pushArguments(water_JNIEnv *wenv, waterObject *object, 
			  waterMethod *method, va_list ap)
{
    int i;

    /* 'this' ptr + args */
    /* if object is null, must be a static method. */
    if(object) {
	assert(object->class->minor_version == 3);
	opstack_push_reference(wenv->opstack_top, &object);
    }
    
    /* push arguments onto the stack */
    for(i = 0; i < method->num_arguments; i++) { 
	/* XXX more bad stuff */
	jint arg = va_arg(ap, jint);
	
	opstack_push_word(wenv->opstack_top, &arg);
	if(method->argument_types[i] == JNI_LONG ||
	   method->argument_types[i] == JNI_DOUBLE) {
	    arg = va_arg(ap, jint);
	    opstack_push_word(wenv->opstack_top, ((char *)&arg+4));
	}
    }
}

static void handleUncaughtException(water_JNIEnv *wenv)
{
    JNIEnv *env = (JNIEnv *)wenv;
    jclass system_class;
    jfieldID err_field;
    jobject err;
    jobject exception_object;
    jmethodID printstacktrace_method;

    fprintf(stderr, "got an uncaught exception.\n");
    system_class = (*env)->FindClass(env, "java/lang/System");
    err_field = (*env)->GetStaticFieldID(env, system_class, "err",
					 "Ljava/io/PrintStream;");
    err = (*env)->GetStaticObjectField(env, system_class, err_field);

    opstack_pop_reference(wenv->opstack_top, &exception_object);
    printstacktrace_method = 
	(*env)->GetMethodID(env, (*env)->GetObjectClass(env, exception_object),
			    "printStackTrace", "()V");
    (*env)->CallVoidMethod(env, exception_object, printstacktrace_method);
}
