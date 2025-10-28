/* XXX AUTOMATICALLY GENERATED FILE -- DO NOT EDIT */
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

/* this is where the instructions are actually defined.                */
/* this file is preprocessed by a perl script to produce the macroized */
/* versions, if we want them.                                          */

/* warning: don't use RETURN in an unbraced if/else body.    */
/* enclosing in do {} while(0) here would break the 'break'. */
#ifdef USE_SWITCH
#define RETURN(x) instr_result = (x); goto instruction_done;
#else
#define RETURN(x) return (x);
#endif

/* if we're in the switch use runByteCode's local copies of these things */
/* to avoid doing so many pointer dereferences, etc.                     */
#if defined(USE_SWITCH) && defined(USE_LOCAL_COPIES)
#define WENV_PC (pc)
#define WENV_OPSTACK_TOP (opstack_top)
#define WENV_FRAME_TOP (frame_top)
#define WENV_RUNNING_CODE (running_code)
#define WENV_RUNNING_CLASS (running_class)
#else
#define WENV_PC (wenv->pc)
#define WENV_OPSTACK_TOP (wenv->opstack_top)
#define WENV_FRAME_TOP (wenv->frame_top)
#define WENV_RUNNING_CODE (wenv->running_method->code->code)
#define WENV_RUNNING_CLASS (wenv->running_method->clazz)
#endif

/* the one-line comments are used as tags -- they must be present and   */
/* formatted correctly!                                                 */

/* never use plain 'return' in this file!  always use the RETURN macro. */
/* otherwise your code will break when used in the switch.              */

#define instr_nop \
{ \
    /* must optimize this! */ \
    WENV_PC += 1; \
    RETURN(0); \
}


#define instr_aconst_null \
{ \
    jobject o = 0; \
 \
    opstack_push_reference(WENV_OPSTACK_TOP, &o); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_m1 \
{ \
    jint i = -1; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_0 \
{ \
    jint i = 0; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_1 \
{ \
    jint i = 1; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_2 \
{ \
    jint i = 2; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_3 \
{ \
    jint i = 3; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_4 \
{ \
    jint i = 4; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iconst_5 \
{ \
    jint i = 5; \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lconst_0 \
{ \
    jlong i = 0; \
 \
    opstack_push_long(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lconst_1 \
{ \
    jlong i = 1; \
 \
    opstack_push_long(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fconst_0 \
{ \
    jfloat i = 0; \
 \
    opstack_push_float(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fconst_1 \
{ \
    jfloat i = 1; \
 \
    opstack_push_float(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fconst_2 \
{ \
    jfloat i = 2; \
 \
    opstack_push_float(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dconst_0 \
{ \
    jdouble i = 0; \
 \
    opstack_push_double(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dconst_1 \
{ \
    jdouble i = 1; \
 \
    opstack_push_double(WENV_OPSTACK_TOP, &i); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_bipush \
{ \
    jbyte b; \
    jint i; \
 \
    GET_BYTE_NOINC(b, WENV_RUNNING_CODE, WENV_PC+1); \
    i = b; \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
 \
    WENV_PC += 2; \
 \
    RETURN(0); \
}


#define instr_sipush \
{ \
    jshort s; \
    jint i; \
 \
    GET_SHORT_NOINC(s, WENV_RUNNING_CODE, WENV_PC+1); \
    i = s; \
    opstack_push_int(WENV_OPSTACK_TOP, &i); \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_ldc \
{ \
    jubyte index; \
    waterConstantPoolEntry *constant; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    constant = &WENV_RUNNING_CLASS->constant_pool[index]; \
 \
    /* the bytecode verifier should not allow bad constant types to */ \
    /* get through                                                  */ \
    assert(constant->tag == CONSTANT_INTEGER_TAG || \
	   constant->tag == CONSTANT_FLOAT_TAG || \
	   constant->tag == CONSTANT_STRING_TAG); \
 \
    if(constant->tag == CONSTANT_INTEGER_TAG) { \
	opstack_push_int(WENV_OPSTACK_TOP, &constant->value); \
    } else if(constant->tag == CONSTANT_FLOAT_TAG) { \
	opstack_push_float(WENV_OPSTACK_TOP, &constant->value); \
    } else if(constant->tag == CONSTANT_STRING_TAG) { \
	waterObject *new_string; \
 \
	SAVE_LOCAL_STUFF; \
	new_string =  \
	    water_newString((JNIEnv *)wenv, \
	       &WENV_RUNNING_CLASS->constant_pool[ \
                   WENV_RUNNING_CLASS->constant_pool[index]. \
		   value.string.string_index \
               ].value.utf8_value); \
	GRAB_LOCAL_STUFF; \
	if(wenv->exception_thrown) { \
	    RETURN(-1); \
	} \
	 \
	assert(IS_VALID_REFERENCE(new_string)); \
	opstack_push_reference(WENV_OPSTACK_TOP, &new_string); \
    } \
    WENV_PC += 2; \
 \
    RETURN(0); \
}


#define instr_ldc_w \
{ \
    jushort index; \
    waterConstantPoolEntry *constant; \
 \
    /* duplicated mostly from the above...  */ \
    /* should integrate the two somehow.    */ \
 \
    /* the bytecode verifier should not allow bad constant types to */ \
    /* get through                                                  */ \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    constant = &WENV_RUNNING_CLASS->constant_pool[index]; \
 \
    assert(constant->tag == CONSTANT_INTEGER_TAG || \
	   constant->tag == CONSTANT_FLOAT_TAG || \
	   constant->tag == CONSTANT_STRING_TAG); \
 \
    if(constant->tag == CONSTANT_INTEGER_TAG) { \
	opstack_push_int(WENV_OPSTACK_TOP, &constant->value); \
    } else if(constant->tag == CONSTANT_FLOAT_TAG) { \
	opstack_push_float(WENV_OPSTACK_TOP, &constant->value); \
    } else if(constant->tag == CONSTANT_STRING_TAG) { \
	waterObject *new_string; \
 \
	SAVE_LOCAL_STUFF; \
	new_string =  \
	    water_newString((JNIEnv *)wenv, \
	       &WENV_RUNNING_CLASS->constant_pool[ \
                   WENV_RUNNING_CLASS->constant_pool[index]. \
		   value.string.string_index \
               ].value.utf8_value); \
	GRAB_LOCAL_STUFF; \
	if(wenv->exception_thrown) { \
	    RETURN(-1); \
	} \
 \
	assert(IS_VALID_REFERENCE(new_string)); \
	opstack_push_reference(WENV_OPSTACK_TOP, &new_string); \
    } \
    WENV_PC += 3; \
 \
    RETURN(0);     \
}


#define instr_ldc2_w \
{ \
    jushort index; \
    waterConstantPoolEntry *constant; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    constant = &WENV_RUNNING_CLASS->constant_pool[index]; \
 \
    assert(constant->tag == CONSTANT_LONG_TAG || \
	   constant->tag == CONSTANT_DOUBLE_TAG); \
 \
    if(constant->tag == CONSTANT_LONG_TAG) { \
	opstack_push_long(WENV_OPSTACK_TOP, &constant->value); \
    } else if(constant->tag == CONSTANT_DOUBLE_TAG) { \
	opstack_push_double(WENV_OPSTACK_TOP, &constant->value); \
    } \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_iload \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0); \
}


#define instr_lload \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_fload \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_dload \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_aload \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0); \
}


#define instr_iload_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iload_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iload_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iload_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lload_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lload_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lload_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lload_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fload_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fload_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fload_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fload_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dload_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dload_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dload_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dload_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}



#define instr_aload_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_aload_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_aload_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_aload_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iaload \
{ \
    jint index; \
    waterObject *array_ref; \
    jint result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_INT_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_laload \
{ \
    jint index; \
    waterObject *array_ref; \
    jlong result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_LONG_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_faload \
{ \
    jint index; \
    waterObject *array_ref; \
    jfloat result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_FLOAT_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_daload \
{ \
    jint index; \
    waterObject *array_ref; \
    jdouble result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_DOUBLE_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_aaload \
{ \
    jint index; \
    waterObject *array_ref; \
    jobject result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_REFERENCE_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    assert(IS_VALID_REFERENCE(result)); \
    opstack_push_reference(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_baload \
{ \
    jint index; \
    waterObject *array_ref; \
    jbyte elem; \
    jint result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_BYTE_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    result = elem; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_caload \
{ \
    jint index; \
    waterObject *array_ref; \
    jchar elem; \
    jint result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_CHAR_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    result = elem; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_saload \
{ \
    jint index; \
    waterObject *array_ref; \
    jshort elem; \
    jint result; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    GET_SHORT_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
    result = elem; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_istore \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_lstore \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_fstore \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_dstore \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_astore \
{ \
    jubyte index; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    assert(wenv->running_method->code->max_locals > index); \
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
    WENV_PC += 2; \
 \
    RETURN(0);     \
}


#define instr_istore_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_istore_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_istore_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_istore_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lstore_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_lstore_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_lstore_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_lstore_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_fstore_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_fstore_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_fstore_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_fstore_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dstore_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_dstore_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_dstore_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_dstore_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_astore_0 \
{ \
    assert(wenv->running_method->code->max_locals > 0); \
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_astore_1 \
{ \
    assert(wenv->running_method->code->max_locals > 1); \
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_astore_2 \
{ \
    assert(wenv->running_method->code->max_locals > 2); \
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_astore_3 \
{ \
    assert(wenv->running_method->code->max_locals > 3); \
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3); \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_iastore \
{ \
    jint index; \
    jint value; \
    waterObject *array_ref; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_INT_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lastore \
{ \
    jint index; \
    jlong value; \
    waterObject *array_ref; \
    jint array_length; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_LONG_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fastore \
{ \
    jint index; \
    jfloat value; \
    waterObject *array_ref; \
    jint array_length; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_FLOAT_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dastore \
{ \
    jint index; \
    jdouble value; \
    waterObject *array_ref; \
    jint array_length; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_DOUBLE_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_aastore \
{ \
    jint index; \
    jobject value; \
    waterObject *array_ref; \
    jint array_length; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &value); \
    assert(IS_VALID_REFERENCE(value)); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_REFERENCE_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_bastore \
{ \
    jint index; \
    jint value; \
    waterObject *array_ref; \
    jbyte elem; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    elem = value; \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_BYTE_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_castore \
{ \
    jint index; \
    jint value; \
    waterObject *array_ref; \
    jchar elem; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    elem = value; \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_CHAR_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_sastore \
{ \
    jint index; \
    jint value; \
    waterObject *array_ref; \
    jshort elem; \
    jint array_length; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref); \
    assert(IS_VALID_REFERENCE(array_ref)); \
    elem = value; \
    if(array_ref == 0) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0); \
    if(index < 0 || index >= array_length) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv,  \
			     "java/lang/ArrayIndexOutOfBoundsException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1);	 \
    } \
    STORE_SHORT_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_pop \
{ \
    opstack_pop_word_to_nowhere(WENV_OPSTACK_TOP); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_pop2 \
{ \
    opstack_pop_double_word_to_nowhere(WENV_OPSTACK_TOP); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup \
{ \
    opstack_dup_word(WENV_OPSTACK_TOP); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup_x1 \
{ \
    jint word1, word2; \
 \
    opstack_pop_word(WENV_OPSTACK_TOP, &word1); \
    opstack_pop_word(WENV_OPSTACK_TOP, &word2); \
 \
    opstack_push_word(WENV_OPSTACK_TOP, &word1); \
    opstack_push_word(WENV_OPSTACK_TOP, &word2); \
    opstack_push_word(WENV_OPSTACK_TOP, &word1); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup_x2 \
{ \
    jint word1, word2, word3; \
 \
    opstack_pop_word(WENV_OPSTACK_TOP, &word1); \
    opstack_pop_word(WENV_OPSTACK_TOP, &word2); \
    opstack_pop_word(WENV_OPSTACK_TOP, &word3); \
 \
    opstack_push_word(WENV_OPSTACK_TOP, &word1); \
    opstack_push_word(WENV_OPSTACK_TOP, &word3); \
    opstack_push_word(WENV_OPSTACK_TOP, &word2); \
    opstack_push_word(WENV_OPSTACK_TOP, &word1); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup2 \
{ \
    opstack_dup_double_word(WENV_OPSTACK_TOP); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup2_x1 \
{ \
    jlong word1, word2; \
 \
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word1); \
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word2); \
 \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1); \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word2); \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dup2_x2 \
{ \
    jlong word1, word2, word3; \
 \
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word1); \
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word2); \
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word3); \
 \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1); \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word3); \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word2); \
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_swap \
{ \
    jint i1, i2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_push_int(WENV_OPSTACK_TOP, &i1); \
    opstack_push_int(WENV_OPSTACK_TOP, &i2); \
     \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iadd \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 + i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ladd \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 + i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fadd \
{ \
    jfloat i1, i2, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    result = i1 + i2; \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dadd \
{ \
    jdouble i1, i2, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    result = i1 + i2; \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_isub \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 - i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lsub \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 - i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fsub \
{ \
    jfloat i1, i2, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    result = i1 - i2; \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dsub \
{ \
    jdouble i1, i2, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    result = i1 - i2; \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_imul \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 * i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lmul \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 * i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fmul \
{ \
    jfloat i1, i2, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    result = i1 * i2; \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dmul \
{ \
    jdouble i1, i2, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    result = i1 * i2; \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_idiv \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    if(i2 == 0) { \
	/* division by zero */ \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/ArithmeticException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    result = i1 / i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ldiv \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    if(i2 == 0) { \
	/* division by zero */ \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/ArithmeticException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    result = i1 / i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fdiv \
{ \
    jfloat i1, i2, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    result = i1 / i2; \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ddiv \
{ \
    jdouble i1, i2, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    result = i1 / i2; \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_irem \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    if(i2 == 0) { \
	/* division by zero */ \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/ArithmeticException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    result = i1 % i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lrem \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    if(i2 == 0) { \
	/* division by zero */ \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/ArithmeticException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    result = i1 % i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_frem \
{ \
    jfloat i1, i2, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    /* XXX NaN/infinity/zero stuff.  bleh. */ \
    result = i1 - (i2 * i1/i2); \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_drem \
{ \
    jdouble i1, i2, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    /* XXX NaN/infinity/zero stuff.  bleh. */ \
    result = i1 - (i2 * i1/i2); \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ineg \
{ \
    jint i1, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = -i1; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lneg \
{ \
    jlong i1, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = -i1; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fneg \
{ \
    jfloat i1, result; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &i1); \
    result = -i1; \
    opstack_push_float(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_dneg \
{ \
    jdouble i1, result; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &i1); \
    result = -i1; \
    opstack_push_double(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ishl \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    /* only the low 5 bits of the shift amount are used */ \
    result = i1 << (i2 & 0x1f); \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lshl \
{ \
    jint i2; \
    jlong i1, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    /* only the low 6 bits of the shift amount are used */ \
    result = i1 << (i2 & 0x3f); \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ishr \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    /* only the low 5 bits of the shift amount are used */ \
    result = i1 >> (i2 & 0x1f); \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lshr \
{ \
    jint i2; \
    jlong i1, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    /* only the low 6 bits of the shift amount are used */ \
    result = i1 >> (i2 & 0x3f); \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iushr \
{ \
    juint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    /* only the low 5 bits of the shift amount are used */ \
    result = i1 >> (i2 & 0x1f); \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lushr \
{ \
    juint i2; \
    julong i1, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    /* only the low 6 bits of the shift amount are used */ \
    result = i1 >> (i2 & 0x3f); \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iand \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 & i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_land \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 & i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ior \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 | i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lor \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 | i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_ixor \
{ \
    jint i1, i2, result; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &i1); \
    result = i1 ^ i2; \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lxor \
{ \
    jlong i1, i2, result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &i2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &i1); \
    result = i1 ^ i2; \
    opstack_push_long(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_iinc \
{ \
    jubyte index; \
    jbyte increment; \
    jint local; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    GET_BYTE_NOINC(increment, WENV_RUNNING_CODE, WENV_PC+2); \
    opstack_load_local_int(&local, WENV_FRAME_TOP, index); \
    local += increment; \
    opstack_store_local_int(&local, WENV_FRAME_TOP, index); \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_i2l \
{ \
    jint source; \
    jlong target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_long(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_i2f \
{ \
    jint source; \
    jfloat target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_float(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_i2d \
{ \
    jint source; \
    jdouble target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_double(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_l2i \
{ \
    jlong source; \
    jint target; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_int(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_l2f \
{ \
    jlong source; \
    jfloat target; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_float(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_l2d \
{ \
    jlong source; \
    jdouble target; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_double(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_f2i \
{ \
    jfloat source; \
    jint target; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_int(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_f2l \
{ \
    jfloat source; \
    jlong target; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_long(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_f2d \
{ \
    jfloat source; \
    jdouble target; \
 \
    opstack_pop_float(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_double(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_d2i \
{ \
    jdouble source; \
    jint target; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_int(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_d2l \
{ \
    jdouble source; \
    jlong target; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_long(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_d2f \
{ \
    jdouble source; \
    jfloat target; \
 \
    opstack_pop_double(WENV_OPSTACK_TOP, &source); \
    target = source; \
    opstack_push_float(WENV_OPSTACK_TOP, &target); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_i2b \
{ \
    jint source; \
    jbyte target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    source = target; \
    opstack_push_int(WENV_OPSTACK_TOP, &source); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_i2c \
{ \
    jint source; \
    jchar target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    source = target; \
    opstack_push_int(WENV_OPSTACK_TOP, &source); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_i2s \
{ \
    jint source; \
    jshort target; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &source); \
    target = source; \
    source = target; \
    opstack_push_int(WENV_OPSTACK_TOP, &source); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_lcmp \
{ \
    jlong value1, value2; \
    jint result; \
 \
    opstack_pop_long(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_long(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	result = 1; \
    } else if(value1 < value2) { \
	result = -1; \
    } else { \
	result = 0; \
    } \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_fcmpl \
{ \
    jfloat value1, value2; \
    jint result; \
 \
    /* the 'l' version is supposed to always return 'less than' when a NaN */ \
    /* is encountered, and the 'g' version always 'greater than'           */ \
    opstack_pop_float(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	result = 1; \
    } else if(value1 < value2) { \
	result = -1; \
    } else { \
	result = 0; \
    } \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_fcmpg \
{ \
    jfloat value1, value2; \
    jint result; \
 \
    /* the 'l' version is supposed to always return 'less than' when a NaN */ \
    /* is encountered, and the 'g' version always 'greater than'           */ \
    opstack_pop_float(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_float(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	result = 1; \
    } else if(value1 < value2) { \
	result = -1; \
    } else { \
	result = 0; \
    } \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_dcmpl \
{ \
    jdouble value1, value2; \
    jint result; \
 \
    /* the 'l' version is supposed to always return 'less than' when a NaN */ \
    /* is encountered, and the 'g' version always 'greater than'           */ \
    opstack_pop_double(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	result = 1; \
    } else if(value1 < value2) { \
	result = -1; \
    } else { \
	result = 0; \
    } \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_dcmpg \
{ \
    jdouble value1, value2; \
    jint result; \
 \
    /* the 'l' version is supposed to always return 'less than' when a NaN */ \
    /* is encountered, and the 'g' version always 'greater than'           */ \
    opstack_pop_double(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_double(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	result = 1; \
    } else if(value1 < value2) { \
	result = -1; \
    } else { \
	result = 0; \
    } \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
 \
    WENV_PC += 1; \
 \
    RETURN(0);     \
}


#define instr_ifeq \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value == 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_ifne \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value != 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_iflt \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value < 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_ifge \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value >= 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_ifgt \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value > 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_ifle \
{ \
    jint value; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value); \
    if(value <= 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmpeq \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 == value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmpne \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 != value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmplt \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 < value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmpge \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 >= value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmpgt \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 > value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_icmple \
{ \
    jint value1, value2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &value2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &value1); \
    if(value1 <= value2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_acmpeq \
{ \
    jobject ref1, ref2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &ref2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &ref1); \
    if(ref1 == ref2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_if_acmpne \
{ \
    jobject ref1, ref2; \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &ref2); \
    opstack_pop_int(WENV_OPSTACK_TOP, &ref1); \
    if(ref1 != ref2) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_goto \
{ \
    jshort offset; \
 \
    GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
    /* byte code verifier should take care of making sure the offset's OK */ \
    WENV_PC += offset; \
 \
    RETURN(0); \
}


#define instr_jsr \
{ \
    jshort offset; \
 \
    GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
    WENV_PC += 3; \
    opstack_push_int(WENV_OPSTACK_TOP, &WENV_PC); \
    /* byte code verifier should take care of making sure the offset's OK */ \
    WENV_PC += offset - 3; \
 \
    RETURN(0);     \
}


#define instr_ret \
{ \
    jubyte index; \
    int new_pc; \
 \
    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    opstack_load_local_int(&new_pc, WENV_FRAME_TOP, index); \
    WENV_PC = new_pc; \
 \
    RETURN(0); \
}


#define instr_tableswitch \
{ \
    int params_offset; \
    jint default_offset, low, high; \
    jint offset; \
    jint index; \
 \
    /* the parameters to the instruction begin at the first 4-byte aligned */ \
    /* pc after the opcode itself.                                         */ \
    params_offset = (WENV_PC + 1 + 3) & ~0x3; \
    GET_INT_NOINC(default_offset,  \
		  WENV_RUNNING_CODE, params_offset); \
    GET_INT_NOINC(low, WENV_RUNNING_CODE, params_offset + 4); \
    GET_INT_NOINC(high, WENV_RUNNING_CODE, params_offset + 8); \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &index); \
    if(index < low || index > high) { \
	/* not in table -- use default */ \
	WENV_PC += default_offset; \
    } else { \
	/* get offset from table -- it's at position index - low */ \
	GET_INT_NOINC(offset, WENV_RUNNING_CODE, \
		      params_offset + 12 + ((index - low) * 4)); \
	WENV_PC += offset; \
    } \
 \
    RETURN(0); \
}


#define instr_lookupswitch \
{ \
    int params_offset; \
    jint default_offset, npairs; \
    jint key, test_key, offset; \
    int i; \
 \
    /* the parameters to the instruction begin at the first 4-byte aligned */ \
    /* pc after the opcode itself.                                         */ \
    params_offset = (WENV_PC + 1 + 3) & ~0x3; \
    GET_INT_NOINC(default_offset,  \
		  WENV_RUNNING_CODE, params_offset); \
    GET_INT_NOINC(npairs, WENV_RUNNING_CODE, params_offset + 4); \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &key); \
 \
    /* look for the key in the table. */ \
    for(i = 0; i < npairs; i++) { \
	GET_INT_NOINC(test_key, WENV_RUNNING_CODE,  \
		      params_offset + 8 + (8 * i)); \
 	if(key == test_key) { \
	    GET_INT_NOINC(offset, WENV_RUNNING_CODE, \
			  params_offset + 8 + (8 * i) + 4); \
	    WENV_PC += offset; \
	    RETURN(0); \
	} \
    } \
 \
    /* couldn't find the key -- use the default offset. */ \
    WENV_PC += default_offset;     \
    RETURN(0); \
}


#define instr_ireturn \
{ \
    RETURN(1); \
}


#define instr_lreturn \
{ \
    RETURN(1); \
}


#define instr_freturn \
{ \
    RETURN(1); \
}


#define instr_dreturn \
{ \
    RETURN(1); \
}


#define instr_areturn \
{ \
    RETURN(1); \
}


#define instr_return \
{ \
    RETURN(1); \
}


#define instr_getstatic \
{ \
    jushort index; \
    waterConstantReference *ref; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.field) { \
	SAVE_LOCAL_STUFF; \
	water_resolveFieldReference((JNIEnv *)wenv,  \
				    WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.field) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchFieldError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    if(ref->resolved.field->width == 2) { \
	opstack_push_fieldvalue_double_word(WENV_OPSTACK_TOP,  \
					    ref->resolved.field); \
    } else { \
	opstack_push_fieldvalue_word(WENV_OPSTACK_TOP, ref->resolved.field); \
    } \
	 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_putstatic \
{ \
    jushort index; \
    waterConstantReference *ref; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.field) { \
	SAVE_LOCAL_STUFF; \
	water_resolveFieldReference((JNIEnv *)wenv,  \
				    WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.field) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchFieldError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    if(ref->resolved.field->width == 2) { \
	opstack_pop_fieldvalue_double_word(WENV_OPSTACK_TOP,  \
					   ref->resolved.field);     \
    } else { \
	opstack_pop_fieldvalue_word(WENV_OPSTACK_TOP, ref->resolved.field); \
    } \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_getfield \
{ \
    jushort index; \
    waterConstantReference *ref; \
    waterObject *object; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.field) { \
	SAVE_LOCAL_STUFF; \
	water_resolveFieldReference((JNIEnv *)wenv,  \
				    WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.field) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchFieldError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &object); \
    assert(IS_VALID_REFERENCE(object)); \
 \
    if(ref->resolved.field->width == 2) { \
	opstack_push_double_word(WENV_OPSTACK_TOP, \
				 &object->data[ref->resolved.field->value.i]); \
    } else { \
	opstack_push_word(WENV_OPSTACK_TOP, \
			  &object->data[ref->resolved.field->value.i]); \
    } \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_putfield \
{ \
    jushort index; \
    waterConstantReference *ref; \
    waterObject *object; \
    jvalue value; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.field) { \
	SAVE_LOCAL_STUFF; \
	water_resolveFieldReference((JNIEnv *)wenv,  \
				    WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.field) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchFieldError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    /* XXX can unions deal with this? */ \
    /* this is the most unkosher stuff right here */ \
    if(ref->resolved.field->width == 2) { \
	opstack_pop_double_word(WENV_OPSTACK_TOP, &value); \
    } else { \
	opstack_pop_word(WENV_OPSTACK_TOP, &value); \
    } \
    opstack_pop_reference(WENV_OPSTACK_TOP, &object); \
    assert(IS_VALID_REFERENCE(object)); \
 \
    if(ref->resolved.field->width == 2) { \
	*(jlong *)&object->data[ref->resolved.field->value.i] = value.j; \
    } else { \
	*(jint *)&object->data[ref->resolved.field->value.i] = value.i; \
    } \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_invokevirtual \
{ \
    jushort index; \
    waterObject *invoked_object; \
    waterConstantReference *ref; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.method) { \
	SAVE_LOCAL_STUFF; \
	water_resolveMethodReference((JNIEnv *)wenv,  \
				     WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.method) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchMethodError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP, \
			  -ref->resolved.method->args_size); \
 \
    /* we're invoking virtually here--so we don't want to run the code */ \
    /* directly from ref->resolved.method.  instead, we have to look   */ \
    /* at the index in ref->resolved.method, and use it as an index    */ \
    /* into the actual object's virtual method table, so any possibly  */ \
    /* overridden methods are called.                                  */ \
 \
    assert(IS_CALLABLE_REFERENCE(invoked_object)); \
    SAVE_LOCAL_STUFF; \
    water_callMethod(wenv, \
		     invoked_object,  \
		     &invoked_object->class-> \
		     method_table[ref->resolved.method->index],  \
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    WENV_PC += 3; \
    RETURN(0); \
}


#define instr_invokespecial \
{ \
    jushort index; \
    waterObject *invoked_object; \
    waterConstantReference *ref; \
 \
    /* hopefully the bytecode verifier will already have taken care of */ \
    /* figuring out whether all of the strange conditions for validly  */ \
    /* calling this instruction are actually in effect.                */ \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.method) { \
	SAVE_LOCAL_STUFF; \
	water_resolveMethodReference((JNIEnv *)wenv,  \
				     WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.method) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchMethodError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP, \
			  -ref->resolved.method->args_size); \
 \
    /* this time we're _not_ invoking virtually -- just call the exact */ \
    /* method from the exact class specified.                          */ \
    assert(IS_CALLABLE_REFERENCE(invoked_object)); \
    SAVE_LOCAL_STUFF; \
    water_callMethod(wenv, invoked_object,  \
		     ref->resolved.method,  \
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    WENV_PC += 3; \
    RETURN(0); \
}


#define instr_invokestatic \
{ \
    jushort index; \
    waterConstantReference *ref; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    if(!ref->resolved.method) { \
	SAVE_LOCAL_STUFF; \
	water_resolveMethodReference((JNIEnv *)wenv,  \
				     WENV_RUNNING_CLASS, ref); \
	GRAB_LOCAL_STUFF; \
	if(!ref->resolved.method) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/NoSuchMethodError"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    SAVE_LOCAL_STUFF; \
    water_callMethod(wenv, 0,  \
		     ref->resolved.method,  \
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    WENV_PC += 3; \
    RETURN(0); \
}


#define instr_invokeinterface \
{ \
    jushort index; \
    waterConstantReference *ref; \
    waterMethod *method; \
    waterUTF8String *method_name, *method_descriptor; \
    waterObject *invoked_object; \
    jubyte nargs; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference; \
    GET_BYTE_NOINC(nargs, WENV_RUNNING_CODE, WENV_PC+3); \
 \
    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP, -nargs * 4); \
    assert(IS_CALLABLE_REFERENCE(invoked_object)); \
 \
    /* the getUTF8Constant and virtualMethodLookup calls do not throw */ \
    /* exceptions or mess with the stack, so we don't have to         */ \
    /* SAVE_LOCAL_STUFF for them.                                     */ \
    method_name =  \
	water_getUTF8Constant(WENV_RUNNING_CLASS,  \
			      WENV_RUNNING_CLASS-> \
			      constant_pool[ref->name_and_type_index]. \
			      value.name_and_type.name_index); \
    method_descriptor =  \
	water_getUTF8Constant(WENV_RUNNING_CLASS,  \
			      WENV_RUNNING_CLASS-> \
			      constant_pool[ref->name_and_type_index]. \
			      value.name_and_type.descriptor_index); \
 \
    /* this is slow, but I think this is pretty much the only reasonable */ \
    /* way to do this.                                                   */ \
    method = water_virtualMethodLookup(invoked_object->class, \
				       method_name, method_descriptor); \
    if(!method || method->access_flags & ACC_STATIC || \
       !(method->access_flags & ACC_PUBLIC)) { \
	water_throwException(wenv, "java/lang/IncompatibleClassChangeError"); \
	RETURN(-1); \
    } \
 \
    SAVE_LOCAL_STUFF; \
    water_callMethod(wenv, invoked_object,  \
		     method,  \
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    WENV_PC += 5; \
    RETURN(0);     \
}


#define instr_new \
{ \
    jushort index; \
    waterObject *new_object; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    SAVE_LOCAL_STUFF; \
    new_object = water_newObject((JNIEnv *)wenv, \
        water_resolveClassByIndex((JNIEnv *)wenv,  \
				  WENV_RUNNING_CLASS, index, CLASS_VERIFIED)); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    assert(IS_VALID_REFERENCE(new_object)); \
    opstack_push_reference(WENV_OPSTACK_TOP, &new_object); \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_newarray \
{ \
    jubyte type; \
    jint size; \
    waterObject *new_array = 0; \
 \
    GET_BYTE_NOINC(type, WENV_RUNNING_CODE, WENV_PC+1); \
    opstack_pop_int(WENV_OPSTACK_TOP, &size); \
 \
    /* verifier should handle making sure the type is OK--      */ \
    /* if it's not, or if memory allocation fails (which should */ \
    /* set an exception), we'll be returning a null reference.  */ \
    SAVE_LOCAL_STUFF; \
    new_array = water_newPrimitiveArray((JNIEnv *)wenv, size, type); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
    assert(IS_VALID_REFERENCE(new_array)); \
    opstack_push_reference(WENV_OPSTACK_TOP, &new_array); \
 \
    WENV_PC += 2; \
 \
    RETURN(0); \
}


#define instr_anewarray \
{ \
    jint size; \
    waterObject *new_array = 0; \
    jushort index; \
    waterClass *element_class; \
 \
    /* create an array of objects.  this time we need the class           */ \
    /* so we can set the array's class to the array version of the class. */ \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    SAVE_LOCAL_STUFF; \
    element_class =  \
	water_resolveClassByIndex((JNIEnv *)wenv,  \
				  WENV_RUNNING_CLASS, index, CLASS_VERIFIED); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    opstack_pop_int(WENV_OPSTACK_TOP, &size); \
 \
    SAVE_LOCAL_STUFF; \
    new_array = water_newObjectArray((JNIEnv *)wenv, size, element_class); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
 \
    assert(IS_VALID_REFERENCE(new_array)); \
    opstack_push_reference(WENV_OPSTACK_TOP, &new_array); \
 \
    WENV_PC += 3; \
 \
    RETURN(0); \
}


#define instr_arraylength \
{ \
    waterObject *array; \
    int array_length; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &array); \
    assert(IS_VALID_REFERENCE(array)); \
    if(!array) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/NullPointerException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array), 0); \
    opstack_push_int(WENV_OPSTACK_TOP, &array_length); \
    WENV_PC += 1; \
 \
    RETURN(0); \
}


#define instr_athrow \
{ \
    /* just report an exception */ \
    /* XXX make sure the thrown exception object is not null-- */ \
    /* if it is, throw a NullPointerException instead.         */ \
    wenv->exception_thrown = 1; \
    RETURN(-1); \
}


#define instr_checkcast \
{ \
    jushort index; \
    waterConstantClass *class_constant; \
    jobject reference; \
 \
    /* XXX just grab this instead of popping and pushing */ \
    opstack_pop_reference(WENV_OPSTACK_TOP, &reference); \
    opstack_push_reference(WENV_OPSTACK_TOP, &reference); \
    assert(IS_VALID_REFERENCE(reference)); \
 \
    if(reference == 0) { \
	/* null can be cast to anything. */ \
	/* no exception */ \
    } else { \
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
	assert(WENV_RUNNING_CLASS->constant_pool[index].tag ==  \
	       CONSTANT_CLASS_TAG); \
	class_constant =  \
	    &WENV_RUNNING_CLASS->constant_pool[index].value.class; \
	if(!class_constant->resolved_class) { \
	    SAVE_LOCAL_STUFF; \
	    class_constant->resolved_class =  \
		water_resolveClassByIndex((JNIEnv *)wenv, \
					  WENV_RUNNING_CLASS, \
					  index, CLASS_VERIFIED); \
	    GRAB_LOCAL_STUFF; \
	    if(!class_constant->resolved_class) { \
		/* exception already thrown */ \
		RETURN(-1); \
	    } \
	} \
 \
	if(!water_castIsOK(wenv, \
			   class_constant->resolved_class, \
			   WATER_OBJECT(reference)->class)) { \
	    SAVE_LOCAL_STUFF; \
	    water_throwException(wenv, "java/lang/ClassCastException"); \
	    GRAB_LOCAL_STUFF; \
	    RETURN(-1); \
	} \
    } \
 \
    WENV_PC += 3; \
    RETURN(0); \
}


#define instr_instanceof \
{ \
    jushort index; \
    waterConstantClass *class_constant; \
    jobject reference; \
    jint result = 0; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &reference); \
    assert(IS_VALID_REFERENCE(reference)); \
 \
    if(reference == 0) { \
	/* this instruction returns 0 for null. */ \
	result = 0; \
    } else { \
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
	assert(WENV_RUNNING_CLASS->constant_pool[index].tag ==  \
	       CONSTANT_CLASS_TAG); \
	class_constant =  \
	    &WENV_RUNNING_CLASS->constant_pool[index].value.class; \
	if(!class_constant->resolved_class) { \
	    SAVE_LOCAL_STUFF; \
	    class_constant->resolved_class = \
		water_resolveClassByIndex((JNIEnv *)wenv, \
					  WENV_RUNNING_CLASS, \
					  index, CLASS_VERIFIED); \
	    GRAB_LOCAL_STUFF; \
	    if(!class_constant->resolved_class) { \
		/* exception already thrown */ \
		RETURN(-1); \
	    } \
	} \
	 \
	result = water_castIsOK(wenv,  \
				class_constant->resolved_class, \
				WATER_OBJECT(reference)->class) ? 1 : 0; \
    } \
 \
    opstack_push_int(WENV_OPSTACK_TOP, &result); \
    WENV_PC += 3; \
    RETURN(0);     \
}


#define instr_monitorenter \
{ \
    waterObject *object; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &object); \
    assert(IS_VALID_REFERENCE(object)); \
    /* XXX check null pointer exception */ \
    PR_EnterMonitor(object->monitor); \
 \
    WENV_PC += 1; \
    RETURN(0);     \
}


#define instr_monitorexit \
{ \
    waterObject *object; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &object); \
    assert(IS_VALID_REFERENCE(object)); \
    /* XXX check null pointer exception */ \
    /* XXX check if we are not the owner of the monitor */ \
    PR_ExitMonitor(object->monitor); \
 \
    WENV_PC += 1; \
    RETURN(0); \
}


#define instr_wide \
{ \
    /* ugh. */ \
    unsigned char opcode; \
 \
    GET_BYTE_NOINC(opcode, WENV_RUNNING_CODE, WENV_PC+1); \
    if(opcode == 132) {  \
	/* iinc */ \
	/* we have a 16-bit index followed by a 16-bit increment */ \
	jushort index; \
	jshort increment; \
	jint local; \
 \
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+2); \
	GET_SHORT_NOINC(increment,  \
			WENV_RUNNING_CODE, WENV_PC+4); \
	opstack_load_local_int(&local, WENV_FRAME_TOP, index); \
	local += increment; \
	opstack_store_local_int(&local, WENV_FRAME_TOP, index); \
 \
	WENV_PC += 6; \
    } else { \
	/* we have just a 16-bit index */ \
	jushort index; \
	jint new_pc; \
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+2); \
 \
	/* pc will be current pc + 4 unless we hit a ret. */ \
	WENV_PC += 4; \
	switch(opcode) { \
	case(21): /* iload */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(22): /* lload */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(23): /* fload */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(24): /* dload */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_push_local_double(WENV_OPSTACK_TOP,  \
				      WENV_FRAME_TOP, index); \
	    break; \
 \
	case(25): /* aload */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_push_local_reference(WENV_OPSTACK_TOP,  \
					 WENV_FRAME_TOP, index); \
	    break; \
 \
	case(54): /* istore */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(55): /* lstore */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(56): /* fstore */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index); \
	    break; \
 \
	case(57): /* dstore */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_pop_local_double(WENV_OPSTACK_TOP,  \
				     WENV_FRAME_TOP, index); \
	    break; \
 \
	case(58): /* astore */ \
	    assert(wenv->running_method->code->max_locals > index); \
	    opstack_pop_local_reference(WENV_OPSTACK_TOP,  \
					WENV_FRAME_TOP, index); \
	    break; \
 \
	case(169): /* ret */ \
	    opstack_load_local_int(&new_pc, WENV_FRAME_TOP, index); \
	    WENV_PC = new_pc; \
 \
	default: \
	    assert(0); \
	} \
    } \
 \
    RETURN(0); \
}


#define instr_multianewarray \
{ \
    jushort index; \
    jubyte dimensions; \
    waterClass *outer_array_type; \
    jint *dimensions_array; \
    waterObject *array_object; \
    int i; \
 \
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1); \
    GET_BYTE_NOINC(dimensions, WENV_RUNNING_CODE, WENV_PC+3); \
    SAVE_LOCAL_STUFF; \
    outer_array_type =  \
	water_resolveClassByIndex((JNIEnv *)wenv, WENV_RUNNING_CLASS, index, \
				  CLASS_VERIFIED); \
    GRAB_LOCAL_STUFF; \
    if(!outer_array_type) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/ClassNotFoundException"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
 \
    /* make an array of the dimensions */ \
    dimensions_array = (jint *)malloc(dimensions * sizeof(jint)); \
    if(!dimensions_array) { \
	SAVE_LOCAL_STUFF; \
	water_throwException(wenv, "java/lang/OutOfMemoryError"); \
	GRAB_LOCAL_STUFF; \
	RETURN(-1); \
    } \
 \
    /* pop the dimensions, in reverse order */ \
    for(i = 0; i < dimensions; i++) { \
	opstack_pop_int(WENV_OPSTACK_TOP,  \
			&dimensions_array[dimensions - i - 1]); \
    } \
 \
    SAVE_LOCAL_STUFF; \
    array_object = water_newMultiArray((JNIEnv *)wenv, outer_array_type, \
				       dimensions, dimensions_array); \
    GRAB_LOCAL_STUFF; \
    if(wenv->exception_thrown) { \
	RETURN(-1); \
    } \
    opstack_push_reference(WENV_OPSTACK_TOP, &array_object); \
 \
    WENV_PC += 4; \
    RETURN(0); \
}


#define instr_ifnull \
{ \
    jobject ref; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &ref); \
    assert(IS_VALID_REFERENCE(ref)); \
    if(ref == 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_ifnonnull \
{ \
    jobject ref; \
 \
    opstack_pop_reference(WENV_OPSTACK_TOP, &ref); \
    assert(IS_VALID_REFERENCE(ref)); \
    if(ref != 0) { \
	jshort offset; \
 \
	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
	WENV_PC += offset; \
    } else { \
	WENV_PC += 3; \
    } \
 \
    RETURN(0); \
}


#define instr_goto_w \
{ \
    jint offset; \
 \
    GET_INT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
    WENV_PC += offset; \
     \
    RETURN(0); \
}


#define instr_jsr_w \
{ \
    jint offset; \
 \
    GET_INT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1); \
    WENV_PC += 5; \
    opstack_push_int(WENV_OPSTACK_TOP, &WENV_PC); \
 \
    /* byte verifier should take care of making sure the offset's OK */ \
    WENV_PC += offset - 5; \
 \
    RETURN(0); \
}

