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
#define WENV_RUNNING_CLASS (wenv->running_method->class)
#endif

/* the one-line comments are used as tags -- they must be present and   */
/* formatted correctly!                                                 */

/* never use plain 'return' in this file!  always use the RETURN macro. */
/* otherwise your code will break when used in the switch.              */

/* nop -- #0 */ 
static int instr_nop(water_JNIEnv *wenv)
{
    /* must optimize this! */
    WENV_PC += 1;
    RETURN(0);
}

/* aconst_null -- #1 */ 
static int instr_aconst_null(water_JNIEnv *wenv)
{
    jobject o = 0;

    opstack_push_reference(WENV_OPSTACK_TOP, &o);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_m1 -- #2 */ 
static int instr_iconst_m1(water_JNIEnv *wenv)
{
    jint i = -1;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_0 -- #3 */ 
static int instr_iconst_0(water_JNIEnv *wenv)
{
    jint i = 0;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_1 -- #4 */ 
static int instr_iconst_1(water_JNIEnv *wenv)
{
    jint i = 1;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_2 -- #5 */ 
static int instr_iconst_2(water_JNIEnv *wenv)
{
    jint i = 2;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_3 -- #6 */ 
static int instr_iconst_3(water_JNIEnv *wenv)
{
    jint i = 3;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_4 -- #7 */ 
static int instr_iconst_4(water_JNIEnv *wenv)
{
    jint i = 4;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* iconst_5 -- #8 */ 
static int instr_iconst_5(water_JNIEnv *wenv)
{
    jint i = 5;

    opstack_push_int(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* lconst_0 -- #9 */ 
static int instr_lconst_0(water_JNIEnv *wenv)
{
    jlong i = 0;

    opstack_push_long(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* lconst_1 -- #10 */ 
static int instr_lconst_1(water_JNIEnv *wenv)
{
    jlong i = 1;

    opstack_push_long(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* fconst_0 -- #11 */ 
static int instr_fconst_0(water_JNIEnv *wenv)
{
    jfloat i = 0;

    opstack_push_float(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* fconst_1 -- #12 */ 
static int instr_fconst_1(water_JNIEnv *wenv)
{
    jfloat i = 1;

    opstack_push_float(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* fconst_2 -- #13 */ 
static int instr_fconst_2(water_JNIEnv *wenv)
{
    jfloat i = 2;

    opstack_push_float(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* dconst_0 -- #14 */ 
static int instr_dconst_0(water_JNIEnv *wenv)
{
    jdouble i = 0;

    opstack_push_double(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* dconst_1 -- #15 */ 
static int instr_dconst_1(water_JNIEnv *wenv)
{
    jdouble i = 1;

    opstack_push_double(WENV_OPSTACK_TOP, &i);
    WENV_PC += 1;

    RETURN(0);
}

/* bipush -- #16 */ 
static int instr_bipush(water_JNIEnv *wenv)
{
    jbyte b;
    jint i;

    GET_BYTE_NOINC(b, WENV_RUNNING_CODE, WENV_PC+1);
    i = b;
    opstack_push_int(WENV_OPSTACK_TOP, &i);

    WENV_PC += 2;

    RETURN(0);
}

/* sipush -- #17 */ 
static int instr_sipush(water_JNIEnv *wenv)
{
    jshort s;
    jint i;

    GET_SHORT_NOINC(s, WENV_RUNNING_CODE, WENV_PC+1);
    i = s;
    opstack_push_int(WENV_OPSTACK_TOP, &i);

    WENV_PC += 3;

    RETURN(0);
}

/* ldc -- #18 */ 
static int instr_ldc(water_JNIEnv *wenv)
{
    jubyte index;
    waterConstantPoolEntry *constant;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    constant = &WENV_RUNNING_CLASS->constant_pool[index];

    /* the bytecode verifier should not allow bad constant types to */
    /* get through                                                  */
    assert(constant->tag == CONSTANT_INTEGER_TAG ||
	   constant->tag == CONSTANT_FLOAT_TAG ||
	   constant->tag == CONSTANT_STRING_TAG);

    if(constant->tag == CONSTANT_INTEGER_TAG) {
	opstack_push_int(WENV_OPSTACK_TOP, &constant->value);
    } else if(constant->tag == CONSTANT_FLOAT_TAG) {
	opstack_push_float(WENV_OPSTACK_TOP, &constant->value);
    } else if(constant->tag == CONSTANT_STRING_TAG) {
	waterObject *new_string;

	SAVE_LOCAL_STUFF;
	new_string = 
	    water_newString((JNIEnv *)wenv,
	       &WENV_RUNNING_CLASS->constant_pool[
                   WENV_RUNNING_CLASS->constant_pool[index].
		   value.string.string_index
               ].value.utf8_value);
	GRAB_LOCAL_STUFF;
	if(wenv->exception_thrown) {
	    RETURN(-1);
	}
	
	assert(IS_VALID_REFERENCE(new_string));
	opstack_push_reference(WENV_OPSTACK_TOP, &new_string);
    }
    WENV_PC += 2;

    RETURN(0);
}

/* ldc_w -- #19 */ 
static int instr_ldc_w(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantPoolEntry *constant;

    /* duplicated mostly from the above...  */
    /* should integrate the two somehow.    */

    /* the bytecode verifier should not allow bad constant types to */
    /* get through                                                  */
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    constant = &WENV_RUNNING_CLASS->constant_pool[index];

    assert(constant->tag == CONSTANT_INTEGER_TAG ||
	   constant->tag == CONSTANT_FLOAT_TAG ||
	   constant->tag == CONSTANT_STRING_TAG);

    if(constant->tag == CONSTANT_INTEGER_TAG) {
	opstack_push_int(WENV_OPSTACK_TOP, &constant->value);
    } else if(constant->tag == CONSTANT_FLOAT_TAG) {
	opstack_push_float(WENV_OPSTACK_TOP, &constant->value);
    } else if(constant->tag == CONSTANT_STRING_TAG) {
	waterObject *new_string;

	SAVE_LOCAL_STUFF;
	new_string = 
	    water_newString((JNIEnv *)wenv,
	       &WENV_RUNNING_CLASS->constant_pool[
                   WENV_RUNNING_CLASS->constant_pool[index].
		   value.string.string_index
               ].value.utf8_value);
	GRAB_LOCAL_STUFF;
	if(wenv->exception_thrown) {
	    RETURN(-1);
	}

	assert(IS_VALID_REFERENCE(new_string));
	opstack_push_reference(WENV_OPSTACK_TOP, &new_string);
    }
    WENV_PC += 3;

    RETURN(0);    
}

/* ldc2_w -- #20 */ 
static int instr_ldc2_w(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantPoolEntry *constant;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    constant = &WENV_RUNNING_CLASS->constant_pool[index];

    assert(constant->tag == CONSTANT_LONG_TAG ||
	   constant->tag == CONSTANT_DOUBLE_TAG);

    if(constant->tag == CONSTANT_LONG_TAG) {
	opstack_push_long(WENV_OPSTACK_TOP, &constant->value);
    } else if(constant->tag == CONSTANT_DOUBLE_TAG) {
	opstack_push_double(WENV_OPSTACK_TOP, &constant->value);
    }
    WENV_PC += 3;

    RETURN(0);
}

/* iload -- #21 */ 
static int instr_iload(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);
}

/* lload -- #22 */ 
static int instr_lload(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* fload -- #23 */ 
static int instr_fload(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* dload -- #24 */ 
static int instr_dload(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* aload -- #25 */ 
static int instr_aload(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);
}

/* iload_0 -- #26 */ 
static int instr_iload_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* iload_1 -- #27 */ 
static int instr_iload_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* iload_2 -- #28 */ 
static int instr_iload_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* iload_3 -- #29 */ 
static int instr_iload_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* lload_0 -- #30 */
static int instr_lload_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* lload_1 -- #31 */ 
static int instr_lload_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* lload_2 -- #32 */ 
static int instr_lload_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* lload_3 -- #33 */ 
static int instr_lload_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* fload_0 -- #34 */ 
static int instr_fload_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* fload_1 -- #35 */ 
static int instr_fload_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* fload_2 -- #36 */ 
static int instr_fload_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* fload_3 -- #37 */ 
static int instr_fload_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* dload_0 -- #38 */ 
static int instr_dload_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* dload_1 -- #39 */ 
static int instr_dload_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* dload_2 -- #40 */ 
static int instr_dload_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* dload_3 -- #41 */ 
static int instr_dload_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_push_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}


/* aload_0 -- #42 */ 
static int instr_aload_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* aload_1 -- #43 */ 
static int instr_aload_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* aload_2 -- #44 */ 
static int instr_aload_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* aload_3 -- #45 */ 
static int instr_aload_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_push_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* iaload -- #46 */ 
static int instr_iaload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jint result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_INT_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* laload -- #47 */ 
static int instr_laload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jlong result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_LONG_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* faload -- #48 */ 
static int instr_faload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jfloat result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_FLOAT_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* daload -- #49 */ 
static int instr_daload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jdouble result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_DOUBLE_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* aaload -- #50 */ 
static int instr_aaload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jobject result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_REFERENCE_MACHINE(result, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    assert(IS_VALID_REFERENCE(result));
    opstack_push_reference(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* baload -- #51 */ 
static int instr_baload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jbyte elem;
    jint result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_BYTE_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    result = elem;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* caload -- #52 */ 
static int instr_caload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jchar elem;
    jint result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_CHAR_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    result = elem;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* saload -- #53 */ 
static int instr_saload(water_JNIEnv *wenv)
{
    jint index;
    waterObject *array_ref;
    jshort elem;
    jint result;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    GET_SHORT_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);
    result = elem;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* istore -- #54 */ 
static int instr_istore(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* lstore -- #55 */ 
static int instr_lstore(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* fstore -- #56 */ 
static int instr_fstore(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* dstore -- #57 */ 
static int instr_dstore(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* astore -- #58 */ 
static int instr_astore(water_JNIEnv *wenv)
{
    jubyte index;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    assert(wenv->running_method->code->max_locals > index);
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
    WENV_PC += 2;

    RETURN(0);    
}

/* istore_0 -- #59 */ 
static int instr_istore_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);
}

/* istore_1 -- #60 */ 
static int instr_istore_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);
}

/* istore_2 -- #61 */ 
static int instr_istore_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);
}

/* istore_3 -- #62 */ 
static int instr_istore_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* lstore_0 -- #63 */ 
static int instr_lstore_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);    
}

/* lstore_1 -- #64 */ 
static int instr_lstore_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);    
}

/* lstore_2 -- #65 */ 
static int instr_lstore_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);    
}

/* lstore_3 -- #66 */ 
static int instr_lstore_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_pop_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);    
}

/* fstore_0 -- #67 */ 
static int instr_fstore_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);    
}

/* fstore_1 -- #68 */ 
static int instr_fstore_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);    
}

/* fstore_2 -- #69 */ 
static int instr_fstore_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);    
}

/* fstore_3 -- #70 */ 
static int instr_fstore_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);
}

/* dstore_0 -- #71 */ 
static int instr_dstore_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);    
}

/* dstore_1 -- #72 */ 
static int instr_dstore_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);    
}

/* dstore_2 -- #73 */ 
static int instr_dstore_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);    
}

/* dstore_3 -- #74 */ 
static int instr_dstore_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_pop_local_double(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);    
}

/* astore_0 -- #75 */ 
static int instr_astore_0(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 0);
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 0);
    WENV_PC += 1;

    RETURN(0);    
}

/* astore_1 -- #76 */ 
static int instr_astore_1(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 1);
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 1);
    WENV_PC += 1;

    RETURN(0);    
}

/* astore_2 -- #77 */ 
static int instr_astore_2(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 2);
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 2);
    WENV_PC += 1;

    RETURN(0);    
}

/* astore_3 -- #78 */ 
static int instr_astore_3(water_JNIEnv *wenv)
{
    assert(wenv->running_method->code->max_locals > 3);
    opstack_pop_local_reference(WENV_OPSTACK_TOP, WENV_FRAME_TOP, 3);
    WENV_PC += 1;

    RETURN(0);    
}

/* iastore -- #79 */ 
static int instr_iastore(water_JNIEnv *wenv)
{
    jint index;
    jint value;
    waterObject *array_ref;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_INT_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* lastore -- #80 */ 
static int instr_lastore(water_JNIEnv *wenv)
{
    jint index;
    jlong value;
    waterObject *array_ref;
    jint array_length;

    opstack_pop_long(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_LONG_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* fastore -- #81 */ 
static int instr_fastore(water_JNIEnv *wenv)
{
    jint index;
    jfloat value;
    waterObject *array_ref;
    jint array_length;

    opstack_pop_float(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_FLOAT_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* dastore -- #82 */ 
static int instr_dastore(water_JNIEnv *wenv)
{
    jint index;
    jdouble value;
    waterObject *array_ref;
    jint array_length;

    opstack_pop_double(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_DOUBLE_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* aastore -- #83 */ 
static int instr_aastore(water_JNIEnv *wenv)
{
    jint index;
    jobject value;
    waterObject *array_ref;
    jint array_length;

    opstack_pop_reference(WENV_OPSTACK_TOP, &value);
    assert(IS_VALID_REFERENCE(value));
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_REFERENCE_MACHINE(value, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* bastore -- #84 */ 
static int instr_bastore(water_JNIEnv *wenv)
{
    jint index;
    jint value;
    waterObject *array_ref;
    jbyte elem;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    elem = value;
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_BYTE_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* castore -- #85 */ 
static int instr_castore(water_JNIEnv *wenv)
{
    jint index;
    jint value;
    waterObject *array_ref;
    jchar elem;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    elem = value;
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_CHAR_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* sastore -- #86 */ 
static int instr_sastore(water_JNIEnv *wenv)
{
    jint index;
    jint value;
    waterObject *array_ref;
    jshort elem;
    jint array_length;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    opstack_pop_reference(WENV_OPSTACK_TOP, &array_ref);
    assert(IS_VALID_REFERENCE(array_ref));
    elem = value;
    if(array_ref == 0) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array_ref), 0);
    if(index < 0 || index >= array_length) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, 
			     "java/lang/ArrayIndexOutOfBoundsException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);	
    }
    STORE_SHORT_MACHINE(elem, ARRAY_ELEMENT_PTR(array_ref, index), 0);

    WENV_PC += 1;

    RETURN(0);
}

/* pop -- #87 */ 
static int instr_pop(water_JNIEnv *wenv)
{
    opstack_pop_word_to_nowhere(WENV_OPSTACK_TOP);
    WENV_PC += 1;

    RETURN(0);
}

/* pop2 -- #88 */ 
static int instr_pop2(water_JNIEnv *wenv)
{
    opstack_pop_double_word_to_nowhere(WENV_OPSTACK_TOP);
    WENV_PC += 1;

    RETURN(0);
}

/* dup -- #89 */ 
static int instr_dup(water_JNIEnv *wenv)
{
    opstack_dup_word(WENV_OPSTACK_TOP);
    WENV_PC += 1;

    RETURN(0);
}

/* dup_x1 -- #90 */ 
static int instr_dup_x1(water_JNIEnv *wenv)
{
    jint word1, word2;

    opstack_pop_word(WENV_OPSTACK_TOP, &word1);
    opstack_pop_word(WENV_OPSTACK_TOP, &word2);

    opstack_push_word(WENV_OPSTACK_TOP, &word1);
    opstack_push_word(WENV_OPSTACK_TOP, &word2);
    opstack_push_word(WENV_OPSTACK_TOP, &word1);

    WENV_PC += 1;

    RETURN(0);
}

/* dup_x2 -- #91 */
static int instr_dup_x2(water_JNIEnv *wenv)
{
    jint word1, word2, word3;

    opstack_pop_word(WENV_OPSTACK_TOP, &word1);
    opstack_pop_word(WENV_OPSTACK_TOP, &word2);
    opstack_pop_word(WENV_OPSTACK_TOP, &word3);

    opstack_push_word(WENV_OPSTACK_TOP, &word1);
    opstack_push_word(WENV_OPSTACK_TOP, &word3);
    opstack_push_word(WENV_OPSTACK_TOP, &word2);
    opstack_push_word(WENV_OPSTACK_TOP, &word1);

    WENV_PC += 1;

    RETURN(0);
}

/* dup2 -- #92 */ 
static int instr_dup2(water_JNIEnv *wenv)
{
    opstack_dup_double_word(WENV_OPSTACK_TOP);
    WENV_PC += 1;

    RETURN(0);
}

/* dup2_x1 -- #93 */
static int instr_dup2_x1(water_JNIEnv *wenv)
{
    jlong word1, word2;

    opstack_pop_double_word(WENV_OPSTACK_TOP, &word1);
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word2);

    opstack_push_double_word(WENV_OPSTACK_TOP, &word1);
    opstack_push_double_word(WENV_OPSTACK_TOP, &word2);
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1);

    WENV_PC += 1;

    RETURN(0);
}

/* dup2_x2 -- #94 */
static int instr_dup2_x2(water_JNIEnv *wenv)
{
    jlong word1, word2, word3;

    opstack_pop_double_word(WENV_OPSTACK_TOP, &word1);
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word2);
    opstack_pop_double_word(WENV_OPSTACK_TOP, &word3);

    opstack_push_double_word(WENV_OPSTACK_TOP, &word1);
    opstack_push_double_word(WENV_OPSTACK_TOP, &word3);
    opstack_push_double_word(WENV_OPSTACK_TOP, &word2);
    opstack_push_double_word(WENV_OPSTACK_TOP, &word1);

    WENV_PC += 1;

    RETURN(0);
}

/* swap -- #95 */ 
static int instr_swap(water_JNIEnv *wenv)
{
    jint i1, i2;

    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_push_int(WENV_OPSTACK_TOP, &i1);
    opstack_push_int(WENV_OPSTACK_TOP, &i2);
    
    WENV_PC += 1;

    RETURN(0);
}

/* iadd -- #96 */ 
static int instr_iadd(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 + i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ladd -- #97 */ 
static int instr_ladd(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 + i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fadd -- #98 */ 
static int instr_fadd(water_JNIEnv *wenv)
{
    jfloat i1, i2, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i2);
    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    result = i1 + i2;
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* dadd -- #99 */ 
static int instr_dadd(water_JNIEnv *wenv)
{
    jdouble i1, i2, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i2);
    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    result = i1 + i2;
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* isub -- #100 */ 
static int instr_isub(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 - i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lsub -- #101 */ 
static int instr_lsub(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 - i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fsub -- #102 */ 
static int instr_fsub(water_JNIEnv *wenv)
{
    jfloat i1, i2, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i2);
    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    result = i1 - i2;
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* dsub -- #103 */ 
static int instr_dsub(water_JNIEnv *wenv)
{
    jdouble i1, i2, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i2);
    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    result = i1 - i2;
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* imul -- #104 */ 
static int instr_imul(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 * i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lmul -- #105 */ 
static int instr_lmul(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 * i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fmul -- #106 */ 
static int instr_fmul(water_JNIEnv *wenv)
{
    jfloat i1, i2, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i2);
    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    result = i1 * i2;
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* dmul -- #107 */ 
static int instr_dmul(water_JNIEnv *wenv)
{
    jdouble i1, i2, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i2);
    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    result = i1 * i2;
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* idiv -- #108 */ 
static int instr_idiv(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    if(i2 == 0) {
	/* division by zero */
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/ArithmeticException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    result = i1 / i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ldiv -- #109 */ 
static int instr_ldiv(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    if(i2 == 0) {
	/* division by zero */
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/ArithmeticException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    result = i1 / i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fdiv -- #110 */ 
static int instr_fdiv(water_JNIEnv *wenv)
{
    jfloat i1, i2, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i2);
    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    result = i1 / i2;
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ddiv -- #111 */ 
static int instr_ddiv(water_JNIEnv *wenv)
{
    jdouble i1, i2, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i2);
    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    result = i1 / i2;
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* irem -- #112 */ 
static int instr_irem(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    if(i2 == 0) {
	/* division by zero */
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/ArithmeticException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    result = i1 % i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lrem -- #113 */ 
static int instr_lrem(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    if(i2 == 0) {
	/* division by zero */
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/ArithmeticException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    result = i1 % i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* frem -- #114 */ 
static int instr_frem(water_JNIEnv *wenv)
{
    jfloat i1, i2, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i2);
    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    /* XXX NaN/infinity/zero stuff.  bleh. */
    result = i1 - (i2 * i1/i2);
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* drem -- #115 */ 
static int instr_drem(water_JNIEnv *wenv)
{
    jdouble i1, i2, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i2);
    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    /* XXX NaN/infinity/zero stuff.  bleh. */
    result = i1 - (i2 * i1/i2);
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ineg -- #116 */ 
static int instr_ineg(water_JNIEnv *wenv)
{
    jint i1, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = -i1;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lneg -- #117 */ 
static int instr_lneg(water_JNIEnv *wenv)
{
    jlong i1, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = -i1;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fneg -- #118 */ 
static int instr_fneg(water_JNIEnv *wenv)
{
    jfloat i1, result;

    opstack_pop_float(WENV_OPSTACK_TOP, &i1);
    result = -i1;
    opstack_push_float(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* dneg -- #119 */ 
static int instr_dneg(water_JNIEnv *wenv)
{
    jdouble i1, result;

    opstack_pop_double(WENV_OPSTACK_TOP, &i1);
    result = -i1;
    opstack_push_double(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ishl -- #120 */ 
static int instr_ishl(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    /* only the low 5 bits of the shift amount are used */
    result = i1 << (i2 & 0x1f);
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lshl -- #121 */ 
static int instr_lshl(water_JNIEnv *wenv)
{
    jint i2;
    jlong i1, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    /* only the low 6 bits of the shift amount are used */
    result = i1 << (i2 & 0x3f);
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ishr -- #122 */ 
static int instr_ishr(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    /* only the low 5 bits of the shift amount are used */
    result = i1 >> (i2 & 0x1f);
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lshr -- #123 */ 
static int instr_lshr(water_JNIEnv *wenv)
{
    jint i2;
    jlong i1, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    /* only the low 6 bits of the shift amount are used */
    result = i1 >> (i2 & 0x3f);
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* iushr -- #124 */ 
static int instr_iushr(water_JNIEnv *wenv)
{
    juint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    /* only the low 5 bits of the shift amount are used */
    result = i1 >> (i2 & 0x1f);
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lushr -- #125 */ 
static int instr_lushr(water_JNIEnv *wenv)
{
    juint i2;
    julong i1, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    /* only the low 6 bits of the shift amount are used */
    result = i1 >> (i2 & 0x3f);
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* iand -- #126 */ 
static int instr_iand(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 & i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* land -- #127 */ 
static int instr_land(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 & i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ior -- #128 */ 
static int instr_ior(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 | i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lor -- #129 */ 
static int instr_lor(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 | i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* ixor -- #130 */ 
static int instr_ixor(water_JNIEnv *wenv)
{
    jint i1, i2, result;

    opstack_pop_int(WENV_OPSTACK_TOP, &i2);
    opstack_pop_int(WENV_OPSTACK_TOP, &i1);
    result = i1 ^ i2;
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* lxor -- #131 */ 
static int instr_lxor(water_JNIEnv *wenv)
{
    jlong i1, i2, result;

    opstack_pop_long(WENV_OPSTACK_TOP, &i2);
    opstack_pop_long(WENV_OPSTACK_TOP, &i1);
    result = i1 ^ i2;
    opstack_push_long(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* iinc -- #132 */ 
static int instr_iinc(water_JNIEnv *wenv)
{
    jubyte index;
    jbyte increment;
    jint local;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    GET_BYTE_NOINC(increment, WENV_RUNNING_CODE, WENV_PC+2);
    opstack_load_local_int(&local, WENV_FRAME_TOP, index);
    local += increment;
    opstack_store_local_int(&local, WENV_FRAME_TOP, index);

    WENV_PC += 3;

    RETURN(0);
}

/* i2l -- #133 */ 
static int instr_i2l(water_JNIEnv *wenv)
{
    jint source;
    jlong target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_long(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* i2f -- #134 */ 
static int instr_i2f(water_JNIEnv *wenv)
{
    jint source;
    jfloat target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_float(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* i2d -- #135 */ 
static int instr_i2d(water_JNIEnv *wenv)
{
    jint source;
    jdouble target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_double(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* l2i -- #136 */ 
static int instr_l2i(water_JNIEnv *wenv)
{
    jlong source;
    jint target;

    opstack_pop_long(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_int(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* l2f -- #137 */ 
static int instr_l2f(water_JNIEnv *wenv)
{
    jlong source;
    jfloat target;

    opstack_pop_long(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_float(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* l2d -- #138 */ 
static int instr_l2d(water_JNIEnv *wenv)
{
    jlong source;
    jdouble target;

    opstack_pop_long(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_double(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* f2i -- #139 */ 
static int instr_f2i(water_JNIEnv *wenv)
{
    jfloat source;
    jint target;

    opstack_pop_float(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_int(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* f2l -- #140 */ 
static int instr_f2l(water_JNIEnv *wenv)
{
    jfloat source;
    jlong target;

    opstack_pop_float(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_long(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* f2d -- #141 */ 
static int instr_f2d(water_JNIEnv *wenv)
{
    jfloat source;
    jdouble target;

    opstack_pop_float(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_double(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* d2i -- #142 */ 
static int instr_d2i(water_JNIEnv *wenv)
{
    jdouble source;
    jint target;

    opstack_pop_double(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_int(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* d2l -- #143 */ 
static int instr_d2l(water_JNIEnv *wenv)
{
    jdouble source;
    jlong target;

    opstack_pop_double(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_long(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* d2f -- #144 */ 
static int instr_d2f(water_JNIEnv *wenv)
{
    jdouble source;
    jfloat target;

    opstack_pop_double(WENV_OPSTACK_TOP, &source);
    target = source;
    opstack_push_float(WENV_OPSTACK_TOP, &target);

    WENV_PC += 1;

    RETURN(0);
}

/* i2b -- #145 */ 
static int instr_i2b(water_JNIEnv *wenv)
{
    jint source;
    jbyte target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    source = target;
    opstack_push_int(WENV_OPSTACK_TOP, &source);

    WENV_PC += 1;

    RETURN(0);
}

/* i2c -- #146 */ 
static int instr_i2c(water_JNIEnv *wenv)
{
    jint source;
    jchar target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    source = target;
    opstack_push_int(WENV_OPSTACK_TOP, &source);

    WENV_PC += 1;

    RETURN(0);
}

/* i2s -- #147 */ 
static int instr_i2s(water_JNIEnv *wenv)
{
    jint source;
    jshort target;

    opstack_pop_int(WENV_OPSTACK_TOP, &source);
    target = source;
    source = target;
    opstack_push_int(WENV_OPSTACK_TOP, &source);

    WENV_PC += 1;

    RETURN(0);
}

/* lcmp -- #148 */ 
static int instr_lcmp(water_JNIEnv *wenv)
{
    jlong value1, value2;
    jint result;

    opstack_pop_long(WENV_OPSTACK_TOP, &value2);
    opstack_pop_long(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	result = 1;
    } else if(value1 < value2) {
	result = -1;
    } else {
	result = 0;
    }
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);
}

/* fcmpl -- #149 */ 
static int instr_fcmpl(water_JNIEnv *wenv)
{
    jfloat value1, value2;
    jint result;

    /* the 'l' version is supposed to always return 'less than' when a NaN */
    /* is encountered, and the 'g' version always 'greater than'           */
    opstack_pop_float(WENV_OPSTACK_TOP, &value2);
    opstack_pop_float(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	result = 1;
    } else if(value1 < value2) {
	result = -1;
    } else {
	result = 0;
    }
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);    
}

/* fcmpg -- #150 */ 
static int instr_fcmpg(water_JNIEnv *wenv)
{
    jfloat value1, value2;
    jint result;

    /* the 'l' version is supposed to always return 'less than' when a NaN */
    /* is encountered, and the 'g' version always 'greater than'           */
    opstack_pop_float(WENV_OPSTACK_TOP, &value2);
    opstack_pop_float(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	result = 1;
    } else if(value1 < value2) {
	result = -1;
    } else {
	result = 0;
    }
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);    
}

/* dcmpl -- #151 */ 
static int instr_dcmpl(water_JNIEnv *wenv)
{
    jdouble value1, value2;
    jint result;

    /* the 'l' version is supposed to always return 'less than' when a NaN */
    /* is encountered, and the 'g' version always 'greater than'           */
    opstack_pop_double(WENV_OPSTACK_TOP, &value2);
    opstack_pop_double(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	result = 1;
    } else if(value1 < value2) {
	result = -1;
    } else {
	result = 0;
    }
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);    
}

/* dcmpg -- #152 */ 
static int instr_dcmpg(water_JNIEnv *wenv)
{
    jdouble value1, value2;
    jint result;

    /* the 'l' version is supposed to always return 'less than' when a NaN */
    /* is encountered, and the 'g' version always 'greater than'           */
    opstack_pop_double(WENV_OPSTACK_TOP, &value2);
    opstack_pop_double(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	result = 1;
    } else if(value1 < value2) {
	result = -1;
    } else {
	result = 0;
    }
    opstack_push_int(WENV_OPSTACK_TOP, &result);

    WENV_PC += 1;

    RETURN(0);    
}

/* ifeq -- #153 */ 
static int instr_ifeq(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value == 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* ifne -- #154 */ 
static int instr_ifne(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value != 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* iflt -- #155 */ 
static int instr_iflt(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value < 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* ifge -- #156 */ 
static int instr_ifge(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value >= 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* ifgt -- #157 */ 
static int instr_ifgt(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value > 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* ifle -- #158 */ 
static int instr_ifle(water_JNIEnv *wenv)
{
    jint value;

    opstack_pop_int(WENV_OPSTACK_TOP, &value);
    if(value <= 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmpeq -- #159 */ 
static int instr_if_icmpeq(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 == value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmpne -- #160 */ 
static int instr_if_icmpne(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 != value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmplt -- #161 */ 
static int instr_if_icmplt(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 < value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmpge -- #162 */ 
static int instr_if_icmpge(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 >= value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmpgt -- #163 */ 
static int instr_if_icmpgt(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 > value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_icmple -- #164 */ 
static int instr_if_icmple(water_JNIEnv *wenv)
{
    jint value1, value2;

    opstack_pop_int(WENV_OPSTACK_TOP, &value2);
    opstack_pop_int(WENV_OPSTACK_TOP, &value1);
    if(value1 <= value2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_acmpeq -- #165 */ 
static int instr_if_acmpeq(water_JNIEnv *wenv)
{
    jobject ref1, ref2;

    opstack_pop_int(WENV_OPSTACK_TOP, &ref2);
    opstack_pop_int(WENV_OPSTACK_TOP, &ref1);
    if(ref1 == ref2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* if_acmpne -- #166 */ 
static int instr_if_acmpne(water_JNIEnv *wenv)
{
    jobject ref1, ref2;

    opstack_pop_int(WENV_OPSTACK_TOP, &ref2);
    opstack_pop_int(WENV_OPSTACK_TOP, &ref1);
    if(ref1 != ref2) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* goto -- #167 */ 
static int instr_goto(water_JNIEnv *wenv)
{
    jshort offset;

    GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
    /* byte code verifier should take care of making sure the offset's OK */
    WENV_PC += offset;

    RETURN(0);
}

/* jsr -- #168 */ 
static int instr_jsr(water_JNIEnv *wenv)
{
    jshort offset;

    GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
    WENV_PC += 3;
    opstack_push_int(WENV_OPSTACK_TOP, &WENV_PC);
    /* byte code verifier should take care of making sure the offset's OK */
    WENV_PC += offset - 3;

    RETURN(0);    
}

/* ret -- #169 */ 
static int instr_ret(water_JNIEnv *wenv)
{
    jubyte index;
    int new_pc;

    GET_BYTE_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    opstack_load_local_int(&new_pc, WENV_FRAME_TOP, index);
    WENV_PC = new_pc;

    RETURN(0);
}

/* tableswitch -- #170 */ 
static int instr_tableswitch(water_JNIEnv *wenv)
{
    int params_offset;
    jint default_offset, low, high;
    jint offset;
    jint index;

    /* the parameters to the instruction begin at the first 4-byte aligned */
    /* pc after the opcode itself.                                         */
    params_offset = (WENV_PC + 1 + 3) & ~0x3;
    GET_INT_NOINC(default_offset, 
		  WENV_RUNNING_CODE, params_offset);
    GET_INT_NOINC(low, WENV_RUNNING_CODE, params_offset + 4);
    GET_INT_NOINC(high, WENV_RUNNING_CODE, params_offset + 8);

    opstack_pop_int(WENV_OPSTACK_TOP, &index);
    if(index < low || index > high) {
	/* not in table -- use default */
	WENV_PC += default_offset;
    } else {
	/* get offset from table -- it's at position index - low */
	GET_INT_NOINC(offset, WENV_RUNNING_CODE,
		      params_offset + 12 + ((index - low) * 4));
	WENV_PC += offset;
    }

    RETURN(0);
}

/* lookupswitch -- #171 */ 
static int instr_lookupswitch(water_JNIEnv *wenv)
{
    int params_offset;
    jint default_offset, npairs;
    jint key, test_key, offset;
    int i;

    /* the parameters to the instruction begin at the first 4-byte aligned */
    /* pc after the opcode itself.                                         */
    params_offset = (WENV_PC + 1 + 3) & ~0x3;
    GET_INT_NOINC(default_offset, 
		  WENV_RUNNING_CODE, params_offset);
    GET_INT_NOINC(npairs, WENV_RUNNING_CODE, params_offset + 4);

    opstack_pop_int(WENV_OPSTACK_TOP, &key);

    /* look for the key in the table. */
    for(i = 0; i < npairs; i++) {
	GET_INT_NOINC(test_key, WENV_RUNNING_CODE, 
		      params_offset + 8 + (8 * i));
 	if(key == test_key) {
	    GET_INT_NOINC(offset, WENV_RUNNING_CODE,
			  params_offset + 8 + (8 * i) + 4);
	    WENV_PC += offset;
	    RETURN(0);
	}
    }

    /* couldn't find the key -- use the default offset. */
    WENV_PC += default_offset;    
    RETURN(0);
}

/* ireturn -- #172 */ 
static int instr_ireturn(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* lreturn -- #173 */ 
static int instr_lreturn(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* freturn -- #174 */ 
static int instr_freturn(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* dreturn -- #175 */ 
static int instr_dreturn(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* areturn -- #176 */ 
static int instr_areturn(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* return -- #177 */ 
static int instr_return(water_JNIEnv *wenv)
{
    RETURN(1);
}

/* getstatic -- #178 */
static int instr_getstatic(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.field) {
	SAVE_LOCAL_STUFF;
	water_resolveFieldReference((JNIEnv *)wenv, 
				    WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.field) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchFieldError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    if(ref->resolved.field->width == 2) {
	opstack_push_fieldvalue_double_word(WENV_OPSTACK_TOP, 
					    ref->resolved.field);
    } else {
	opstack_push_fieldvalue_word(WENV_OPSTACK_TOP, ref->resolved.field);
    }
	
    WENV_PC += 3;

    RETURN(0);
}

/* putstatic -- #179 */ 
static int instr_putstatic(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.field) {
	SAVE_LOCAL_STUFF;
	water_resolveFieldReference((JNIEnv *)wenv, 
				    WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.field) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchFieldError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    if(ref->resolved.field->width == 2) {
	opstack_pop_fieldvalue_double_word(WENV_OPSTACK_TOP, 
					   ref->resolved.field);    
    } else {
	opstack_pop_fieldvalue_word(WENV_OPSTACK_TOP, ref->resolved.field);
    }

    WENV_PC += 3;

    RETURN(0);
}

/* getfield -- #180 */ 
static int instr_getfield(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;
    waterObject *object;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.field) {
	SAVE_LOCAL_STUFF;
	water_resolveFieldReference((JNIEnv *)wenv, 
				    WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.field) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchFieldError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    opstack_pop_reference(WENV_OPSTACK_TOP, &object);
    assert(IS_VALID_REFERENCE(object));

    if(ref->resolved.field->width == 2) {
	opstack_push_double_word(WENV_OPSTACK_TOP,
				 &object->data[ref->resolved.field->value.i]);
    } else {
	opstack_push_word(WENV_OPSTACK_TOP,
			  &object->data[ref->resolved.field->value.i]);
    }

    WENV_PC += 3;

    RETURN(0);
}

/* putfield -- #181 */ 
static int instr_putfield(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;
    waterObject *object;
    jvalue value;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.field) {
	SAVE_LOCAL_STUFF;
	water_resolveFieldReference((JNIEnv *)wenv, 
				    WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.field) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchFieldError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    /* XXX can unions deal with this? */
    /* this is the most unkosher stuff right here */
    if(ref->resolved.field->width == 2) {
	opstack_pop_double_word(WENV_OPSTACK_TOP, &value);
    } else {
	opstack_pop_word(WENV_OPSTACK_TOP, &value);
    }
    opstack_pop_reference(WENV_OPSTACK_TOP, &object);
    assert(IS_VALID_REFERENCE(object));

    if(ref->resolved.field->width == 2) {
	*(jlong *)&object->data[ref->resolved.field->value.i] = value.j;
    } else {
	*(jint *)&object->data[ref->resolved.field->value.i] = value.i;
    }

    WENV_PC += 3;

    RETURN(0);
}

/* invokevirtual -- #182 */ 
static int instr_invokevirtual(water_JNIEnv *wenv)
{
    jushort index;
    waterObject *invoked_object;
    waterConstantReference *ref;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.method) {
	SAVE_LOCAL_STUFF;
	water_resolveMethodReference((JNIEnv *)wenv, 
				     WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.method) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchMethodError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP,
			  -ref->resolved.method->args_size);

    /* we're invoking virtually here--so we don't want to run the code */
    /* directly from ref->resolved.method.  instead, we have to look   */
    /* at the index in ref->resolved.method, and use it as an index    */
    /* into the actual object's virtual method table, so any possibly  */
    /* overridden methods are called.                                  */

    assert(IS_CALLABLE_REFERENCE(invoked_object));
    SAVE_LOCAL_STUFF;
    water_callMethod(wenv,
		     invoked_object, 
		     &invoked_object->class->
		     method_table[ref->resolved.method->index], 
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    WENV_PC += 3;
    RETURN(0);
}

/* invokespecial -- #183 */ 
/* this instruction handles non-virtual method calls--either calls */
/* to a private method (which hence can't be overridden), or       */
/* deliberate calls using "super.", or constructor calls.          */
static int instr_invokespecial(water_JNIEnv *wenv)
{
    jushort index;
    waterObject *invoked_object;
    waterConstantReference *ref;

    /* hopefully the bytecode verifier will already have taken care of */
    /* figuring out whether all of the strange conditions for validly  */
    /* calling this instruction are actually in effect.                */
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.method) {
	SAVE_LOCAL_STUFF;
	water_resolveMethodReference((JNIEnv *)wenv, 
				     WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.method) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchMethodError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP,
			  -ref->resolved.method->args_size);

    /* this time we're _not_ invoking virtually -- just call the exact */
    /* method from the exact class specified.                          */
    assert(IS_CALLABLE_REFERENCE(invoked_object));
    SAVE_LOCAL_STUFF;
    water_callMethod(wenv, invoked_object, 
		     ref->resolved.method, 
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    WENV_PC += 3;
    RETURN(0);
}

/* invokestatic -- #184 */ 
static int instr_invokestatic(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    if(!ref->resolved.method) {
	SAVE_LOCAL_STUFF;
	water_resolveMethodReference((JNIEnv *)wenv, 
				     WENV_RUNNING_CLASS, ref);
	GRAB_LOCAL_STUFF;
	if(!ref->resolved.method) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/NoSuchMethodError");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    SAVE_LOCAL_STUFF;
    water_callMethod(wenv, 0, 
		     ref->resolved.method, 
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    WENV_PC += 3;
    RETURN(0);
}

/* invokeinterface -- #185 */ 
static int instr_invokeinterface(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantReference *ref;
    waterMethod *method;
    waterUTF8String *method_name, *method_descriptor;
    waterObject *invoked_object;
    jubyte nargs;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    ref = &WENV_RUNNING_CLASS->constant_pool[index].value.reference;
    GET_BYTE_NOINC(nargs, WENV_RUNNING_CODE, WENV_PC+3);

    GET_REFERENCE_MACHINE(invoked_object, WENV_OPSTACK_TOP, -nargs * 4);
    assert(IS_CALLABLE_REFERENCE(invoked_object));

    /* the getUTF8Constant and virtualMethodLookup calls do not throw */
    /* exceptions or mess with the stack, so we don't have to         */
    /* SAVE_LOCAL_STUFF for them.                                     */
    method_name = 
	water_getUTF8Constant(WENV_RUNNING_CLASS, 
			      WENV_RUNNING_CLASS->
			      constant_pool[ref->name_and_type_index].
			      value.name_and_type.name_index);
    method_descriptor = 
	water_getUTF8Constant(WENV_RUNNING_CLASS, 
			      WENV_RUNNING_CLASS->
			      constant_pool[ref->name_and_type_index].
			      value.name_and_type.descriptor_index);

    /* this is slow, but I think this is pretty much the only reasonable */
    /* way to do this.                                                   */
    method = water_virtualMethodLookup(invoked_object->class,
				       method_name, method_descriptor);
    if(!method || method->access_flags & ACC_STATIC ||
       !(method->access_flags & ACC_PUBLIC)) {
	water_throwException(wenv, "java/lang/IncompatibleClassChangeError");
	RETURN(-1);
    }

    SAVE_LOCAL_STUFF;
    water_callMethod(wenv, invoked_object, 
		     method, 
		     WATER_ARGS_ALREADY_PUSHED, WATER_NO_ARGS);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    WENV_PC += 5;
    RETURN(0);    
}

/* new -- #187 */ 
static int instr_new(water_JNIEnv *wenv)
{
    jushort index;
    waterObject *new_object;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    SAVE_LOCAL_STUFF;
    new_object = water_newObject((JNIEnv *)wenv,
        water_resolveClassByIndex((JNIEnv *)wenv, 
				  WENV_RUNNING_CLASS, index, CLASS_VERIFIED));
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    assert(IS_VALID_REFERENCE(new_object));
    opstack_push_reference(WENV_OPSTACK_TOP, &new_object);
    WENV_PC += 3;

    RETURN(0);
}

/* newarray -- #188 */ 
static int instr_newarray(water_JNIEnv *wenv)
{
    jubyte type;
    jint size;
    waterObject *new_array = 0;

    GET_BYTE_NOINC(type, WENV_RUNNING_CODE, WENV_PC+1);
    opstack_pop_int(WENV_OPSTACK_TOP, &size);

    /* verifier should handle making sure the type is OK--      */
    /* if it's not, or if memory allocation fails (which should */
    /* set an exception), we'll be returning a null reference.  */
    SAVE_LOCAL_STUFF;
    new_array = water_newPrimitiveArray((JNIEnv *)wenv, size, type);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }
    assert(IS_VALID_REFERENCE(new_array));
    opstack_push_reference(WENV_OPSTACK_TOP, &new_array);

    WENV_PC += 2;

    RETURN(0);
}

/* anewarray -- #189 */ 
static int instr_anewarray(water_JNIEnv *wenv)
{
    jint size;
    waterObject *new_array = 0;
    jushort index;
    waterClass *element_class;

    /* create an array of objects.  this time we need the class           */
    /* so we can set the array's class to the array version of the class. */
    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    SAVE_LOCAL_STUFF;
    element_class = 
	water_resolveClassByIndex((JNIEnv *)wenv, 
				  WENV_RUNNING_CLASS, index, CLASS_VERIFIED);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    opstack_pop_int(WENV_OPSTACK_TOP, &size);

    SAVE_LOCAL_STUFF;
    new_array = water_newObjectArray((JNIEnv *)wenv, size, element_class);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }

    assert(IS_VALID_REFERENCE(new_array));
    opstack_push_reference(WENV_OPSTACK_TOP, &new_array);

    WENV_PC += 3;

    RETURN(0);
}

/* arraylength -- #190 */ 
static int instr_arraylength(water_JNIEnv *wenv)
{
    waterObject *array;
    int array_length;

    opstack_pop_reference(WENV_OPSTACK_TOP, &array);
    assert(IS_VALID_REFERENCE(array));
    if(!array) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/NullPointerException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(array), 0);
    opstack_push_int(WENV_OPSTACK_TOP, &array_length);
    WENV_PC += 1;

    RETURN(0);
}

/* athrow -- #191 */ 
static int instr_athrow(water_JNIEnv *wenv)
{
    /* just report an exception */
    /* XXX make sure the thrown exception object is not null-- */
    /* if it is, throw a NullPointerException instead.         */
    wenv->exception_thrown = 1;
    RETURN(-1);
}

/* checkcast -- #192 */ 
static int instr_checkcast(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantClass *class_constant;
    jobject reference;

    /* XXX just grab this instead of popping and pushing */
    opstack_pop_reference(WENV_OPSTACK_TOP, &reference);
    opstack_push_reference(WENV_OPSTACK_TOP, &reference);
    assert(IS_VALID_REFERENCE(reference));

    if(reference == 0) {
	/* null can be cast to anything. */
	/* no exception */
    } else {
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
	assert(WENV_RUNNING_CLASS->constant_pool[index].tag == 
	       CONSTANT_CLASS_TAG);
	class_constant = 
	    &WENV_RUNNING_CLASS->constant_pool[index].value.class;
	if(!class_constant->resolved_class) {
	    SAVE_LOCAL_STUFF;
	    class_constant->resolved_class = 
		water_resolveClassByIndex((JNIEnv *)wenv,
					  WENV_RUNNING_CLASS,
					  index, CLASS_VERIFIED);
	    GRAB_LOCAL_STUFF;
	    if(!class_constant->resolved_class) {
		/* exception already thrown */
		RETURN(-1);
	    }
	}

	if(!water_castIsOK(wenv,
			   class_constant->resolved_class,
			   WATER_OBJECT(reference)->class)) {
	    SAVE_LOCAL_STUFF;
	    water_throwException(wenv, "java/lang/ClassCastException");
	    GRAB_LOCAL_STUFF;
	    RETURN(-1);
	}
    }

    WENV_PC += 3;
    RETURN(0);
}

/* instanceof -- #193 */ 
static int instr_instanceof(water_JNIEnv *wenv)
{
    jushort index;
    waterConstantClass *class_constant;
    jobject reference;
    jint result = 0;

    opstack_pop_reference(WENV_OPSTACK_TOP, &reference);
    assert(IS_VALID_REFERENCE(reference));

    if(reference == 0) {
	/* this instruction returns 0 for null. */
	result = 0;
    } else {
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
	assert(WENV_RUNNING_CLASS->constant_pool[index].tag == 
	       CONSTANT_CLASS_TAG);
	class_constant = 
	    &WENV_RUNNING_CLASS->constant_pool[index].value.class;
	if(!class_constant->resolved_class) {
	    SAVE_LOCAL_STUFF;
	    class_constant->resolved_class =
		water_resolveClassByIndex((JNIEnv *)wenv,
					  WENV_RUNNING_CLASS,
					  index, CLASS_VERIFIED);
	    GRAB_LOCAL_STUFF;
	    if(!class_constant->resolved_class) {
		/* exception already thrown */
		RETURN(-1);
	    }
	}
	
	result = water_castIsOK(wenv, 
				class_constant->resolved_class,
				WATER_OBJECT(reference)->class) ? 1 : 0;
    }

    opstack_push_int(WENV_OPSTACK_TOP, &result);
    WENV_PC += 3;
    RETURN(0);    
}

/* monitorenter -- #194 */ 
static int instr_monitorenter(water_JNIEnv *wenv)
{
    waterObject *object;

    opstack_pop_reference(WENV_OPSTACK_TOP, &object);
    assert(IS_VALID_REFERENCE(object));
    /* XXX check null pointer exception */
    PR_EnterMonitor(object->monitor);

    WENV_PC += 1;
    RETURN(0);    
}

/* monitorexit -- #195 */ 
static int instr_monitorexit(water_JNIEnv *wenv)
{
    waterObject *object;

    opstack_pop_reference(WENV_OPSTACK_TOP, &object);
    assert(IS_VALID_REFERENCE(object));
    /* XXX check null pointer exception */
    /* XXX check if we are not the owner of the monitor */
    PR_ExitMonitor(object->monitor);

    WENV_PC += 1;
    RETURN(0);
}

/* wide -- #196 */ 
static int instr_wide(water_JNIEnv *wenv)
{
    /* ugh. */
    unsigned char opcode;

    GET_BYTE_NOINC(opcode, WENV_RUNNING_CODE, WENV_PC+1);
    if(opcode == 132) { 
	/* iinc */
	/* we have a 16-bit index followed by a 16-bit increment */
	jushort index;
	jshort increment;
	jint local;

	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+2);
	GET_SHORT_NOINC(increment, 
			WENV_RUNNING_CODE, WENV_PC+4);
	opstack_load_local_int(&local, WENV_FRAME_TOP, index);
	local += increment;
	opstack_store_local_int(&local, WENV_FRAME_TOP, index);

	WENV_PC += 6;
    } else {
	/* we have just a 16-bit index */
	jushort index;
	jint new_pc;
	GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+2);

	/* pc will be current pc + 4 unless we hit a ret. */
	WENV_PC += 4;
	switch(opcode) {
	case(21): /* iload */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_push_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(22): /* lload */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_push_local_long(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(23): /* fload */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_push_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(24): /* dload */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_push_local_double(WENV_OPSTACK_TOP, 
				      WENV_FRAME_TOP, index);
	    break;

	case(25): /* aload */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_push_local_reference(WENV_OPSTACK_TOP, 
					 WENV_FRAME_TOP, index);
	    break;

	case(54): /* istore */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(55): /* lstore */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_pop_local_int(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(56): /* fstore */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_pop_local_float(WENV_OPSTACK_TOP, WENV_FRAME_TOP, index);
	    break;

	case(57): /* dstore */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_pop_local_double(WENV_OPSTACK_TOP, 
				     WENV_FRAME_TOP, index);
	    break;

	case(58): /* astore */
	    assert(wenv->running_method->code->max_locals > index);
	    opstack_pop_local_reference(WENV_OPSTACK_TOP, 
					WENV_FRAME_TOP, index);
	    break;

	case(169): /* ret */
	    opstack_load_local_int(&new_pc, WENV_FRAME_TOP, index);
	    WENV_PC = new_pc;

	default:
	    assert(0);
	}
    }

    RETURN(0);
}

/* multianewarray -- #197 */ 
static int instr_multianewarray(water_JNIEnv *wenv)
{
    jushort index;
    jubyte dimensions;
    waterClass *outer_array_type;
    jint *dimensions_array;
    waterObject *array_object;
    int i;

    GET_SHORT_NOINC(index, WENV_RUNNING_CODE, WENV_PC+1);
    GET_BYTE_NOINC(dimensions, WENV_RUNNING_CODE, WENV_PC+3);
    SAVE_LOCAL_STUFF;
    outer_array_type = 
	water_resolveClassByIndex((JNIEnv *)wenv, WENV_RUNNING_CLASS, index,
				  CLASS_VERIFIED);
    GRAB_LOCAL_STUFF;
    if(!outer_array_type) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/ClassNotFoundException");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }

    /* make an array of the dimensions */
    dimensions_array = (jint *)malloc(dimensions * sizeof(jint));
    if(!dimensions_array) {
	SAVE_LOCAL_STUFF;
	water_throwException(wenv, "java/lang/OutOfMemoryError");
	GRAB_LOCAL_STUFF;
	RETURN(-1);
    }

    /* pop the dimensions, in reverse order */
    for(i = 0; i < dimensions; i++) {
	opstack_pop_int(WENV_OPSTACK_TOP, 
			&dimensions_array[dimensions - i - 1]);
    }

    SAVE_LOCAL_STUFF;
    array_object = water_newMultiArray((JNIEnv *)wenv, outer_array_type,
				       dimensions, dimensions_array);
    GRAB_LOCAL_STUFF;
    if(wenv->exception_thrown) {
	RETURN(-1);
    }
    opstack_push_reference(WENV_OPSTACK_TOP, &array_object);

    WENV_PC += 4;
    RETURN(0);
}

/* ifnull -- #198 */ 
static int instr_ifnull(water_JNIEnv *wenv)
{
    jobject ref;

    opstack_pop_reference(WENV_OPSTACK_TOP, &ref);
    assert(IS_VALID_REFERENCE(ref));
    if(ref == 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* ifnonnull -- #199 */ 
static int instr_ifnonnull(water_JNIEnv *wenv)
{
    jobject ref;

    opstack_pop_reference(WENV_OPSTACK_TOP, &ref);
    assert(IS_VALID_REFERENCE(ref));
    if(ref != 0) {
	jshort offset;

	GET_SHORT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
	WENV_PC += offset;
    } else {
	WENV_PC += 3;
    }

    RETURN(0);
}

/* goto_w -- #200 */ 
static int instr_goto_w(water_JNIEnv *wenv)
{
    jint offset;

    GET_INT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
    WENV_PC += offset;
    
    RETURN(0);
}

/* jsr_w -- #201 */ 
static int instr_jsr_w(water_JNIEnv *wenv)
{
    jint offset;

    GET_INT_NOINC(offset, WENV_RUNNING_CODE, WENV_PC+1);
    WENV_PC += 5;
    opstack_push_int(WENV_OPSTACK_TOP, &WENV_PC);

    /* byte verifier should take care of making sure the offset's OK */
    WENV_PC += offset - 5;

    RETURN(0);
}
