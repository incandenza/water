#include <stdlib.h>
#include <string.h>
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

#include "jni.h"
#include "exec.h"
#include "byteops.h"
#include "stack.h"
#include "objects.h"
#include "call.h"
#include "classes.h"
#include "exception.h"
#include <assert.h>

#if !defined(USE_SWITCH) || defined(DEBUG_djm)
#if !defined(USE_SWITCH)
#include "exec-instr-protos.c"
#endif

typedef struct java_instruction {
    unsigned char *name;
    int (*hook)(water_JNIEnv *wenv);
} java_instruction;

static const java_instruction instruction_hooks[] = {
#if !defined(USE_SWITCH)
#include "exec-instr-table.c"
#else /* DEBUG_djm */
#include "exec-instr-table-stubs.c"
#endif
};
#endif

/* GRAB_LOCAL_STUFF/SAVE_LOCAL_STUFF makes the wenv->* stuff that we */
/* cache in locals consistent when exiting the routine.  before      */
/* leaving the runByteCode routine for any reason, SAVE_LOCAL_STUFF  */
/* should be called.  after a called function returns,               */
/* GRAB_LOCAL_STUFF should be called.                                */

/* called routines should only change the opstack_top (when throwing */
/* exceptions), and the only thing we change is the opstack_top and  */
/* the pc.                                                           */
#if defined(USE_SWITCH) && defined(USE_LOCAL_COPIES)
#define GRAB_LOCAL_STUFF \
do { \
    opstack_top = wenv->opstack_top;                    \
} while(0)
#define SAVE_LOCAL_STUFF \
do { \
    wenv->pc = pc;                                      \
    wenv->opstack_top = opstack_top;                    \
    /* the rest should never change--no need to save */ \
} while(0)
#else 
#define GRAB_LOCAL_STUFF do {} while(0)
#define SAVE_LOCAL_STUFF do {} while(0)
#endif

#ifdef USE_SWITCH
#include "exec-instr-macros.c"
#else
#include "exec-instr-pre.c"
#endif

int water_runByteCode(water_JNIEnv *wenv)
{
#ifdef DEBUG_djm
#define COUNT_STUFF
#define SHOW_INSTRUCTIONS
#endif

#ifdef DEBUG_djm
#ifdef SHOW_INSTRUCTIONS
#define COUNT_STUFF
#endif
#endif

#if defined(USE_SWITCH) && defined(USE_LOCAL_COPIES)
    /* keep our own local copies of some of this stuff so we don't have  */
    /* to do so many pointer dereferences and writebacks to memory, etc. */
    /* this way the compiler has the option of putting some of these     */
    /* (especially pc and opstack_top) in registers, which should speed  */
    /* things up a fair amount.                                          */

    /* hmm, I tried specifying 'register' for 'pc' and 'opstack_top',    */
    /* but this actually slowed down caffeinemark by quite a bit.        */
    int pc;
    unsigned char *opstack_top;
    unsigned char *frame_top;
    unsigned char *running_code;
    waterClass *running_class;
#endif
    
#if defined(DEBUG_djm)
#if defined(COUNT_STUFF)
    static int invocation_num = 0;
    static int instructions_executed = 0;
#if defined(SHOW_INSTRUCTIONS)
    static char *should_show_instructions = 0;
    static int should_initialized = 0;

    if(!should_initialized) {
	should_show_instructions = getenv("WATER_SHOWINST");
	should_initialized = 1;
    }
#endif

    invocation_num++;
#endif
#endif

#if defined(USE_SWITCH) && defined(USE_LOCAL_COPIES)
    GRAB_LOCAL_STUFF;
    pc = wenv->pc;
    frame_top = wenv->frame_top;
    running_code = wenv->running_method->code->code;
    running_class = wenv->running_method->clazz;
#endif

    while(1 /* until we hit a return instruction */) {
	int instr_result = 0;

#if defined(DEBUG_djm) && defined(SHOW_INSTRUCTIONS)
	waterUTF8String *class_name = 
	    water_getClassName(wenv->running_method->clazz,
			       wenv->running_method->clazz->this_class);
	char *class_name_normal = water_UTF8ToNormal(class_name);
	char *method_name_normal = 
	    water_UTF8ToNormal(wenv->running_method->name);
	char *descriptor_normal = 
	    water_UTF8ToNormal(wenv->running_method->descriptor);
	int old_pc = wenv->pc;

	if(should_show_instructions) {
	    fprintf(stderr, "i %d %d %d: %s %s %s %s %d\n",
		    wenv->call_depth, invocation_num, instructions_executed,
		    instruction_hooks[wenv->running_method->code->
				     code[wenv->pc]].name,
		    class_name_normal, method_name_normal, descriptor_normal,
		    wenv->pc);
	}
#endif

#ifdef USE_SWITCH
	/* the idea is that we can make these functions conditionally   */
	/* line, giving the effect of having all the code directly in   */
	/* this function, but then also turn the inlining off and have  */
	/* individual functions for profiling, debugging, etc.          */
	switch(wenv->running_method->code->code[wenv->pc]) {

#include "exec-instr-switch.c"

	default:
	    /* unrecognized instruction -- bytecode verifier should've caught */
	    assert(0);
	    abort();
	    break;
	}
#else
	instr_result = 
	    instruction_hooks[wenv->running_method->code->code[wenv->pc]].
	    hook(wenv);
#endif

#ifdef USE_SWITCH
instruction_done:
#endif

#if defined(DEBUG_djm) && defined(SHOW_INSTRUCTIONS)
	if(should_show_instructions) {
	    fprintf(stderr, "i %d %d %d: %s %s %s %s %d end\n",
		    wenv->call_depth, invocation_num, instructions_executed,
		    instruction_hooks[wenv->running_method->code->code[old_pc]].
		    name,
		    class_name_normal, method_name_normal, descriptor_normal,
		    old_pc);
	}

	free(descriptor_normal);
	free(method_name_normal);
	free(class_name_normal);
#endif

#if defined(DEBUG_djm) && defined(COUNT_STUFF)
	instructions_executed++;
#endif

	/* put our local copies of things back in the real environment */
	SAVE_LOCAL_STUFF;

	if(instr_result < 0) {
#ifdef DEBUG_djm
	    fprintf(stderr, 
		    "EXCEPTION: got an exception executing bytecode.\n");
#endif
	    return instr_result;
	} else if(instr_result > 0) {
	    /* ready to return. */
	    return instr_result;
	}
    }
}


