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
#include "classes.h"
#include "jni_types.h"
#include "verify.h"
#include "exception.h"
#include "byteops.h"
#include <assert.h>
#include <string.h>

/* the only way you get nested jsr's in the first place is by having  */
/* a finally clause inside another finally clause.  so this will      */
/* allow that up to a nesting depth of 3.                             */
#define MAX_NESTED_JSRS    3

typedef struct waterSlot {
    int type;
    waterClass *class;
} waterSlot;

typedef struct waterInstruction {
    int index;

    int pc;
    int length;

    int changed;

    int branch_targets_computed;
    int num_branch_targets;
    int *branch_targets;

    int previously_reached;
    int stack_size;
    waterSlot *stack_contents;

    waterSlot *locals_contents;

    int num_jsr_targets_reachable_from;
    int jsr_targets_reachable_from[MAX_NESTED_JSRS];

    int jsr_returning_from_set;
    int jsr_returning_from;

    int *jsr_locals_touched[MAX_NESTED_JSRS];
} waterInstruction;

#define INITIAL_INSTRUCTION_ALLOC_CHUNK 100
int instruction_alloc_chunk = INITIAL_INSTRUCTION_ALLOC_CHUNK;

int instruction_length_table[] = {
/*          0   1   2   3   4   5   6   7   8   9  */
/*   0 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
/*  10 */   1,  1,  1,  1,  1,  1,  2,  3,  2,  3,  
/*  20 */   3,  2,  2,  2,  2,  2,  1,  1,  1,  1,  
/*  30 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
/*  40 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
/*  50 */   1,  1,  1,  1,  2,  2,  2,  2,  2,  1,  
/*  60 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  70 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  80 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/*  90 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 100 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 110 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 120 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 130 */   1,  1,  3,  1,  1,  1,  1,  1,  1,  1,
/* 140 */   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
/* 150 */   1,  1,  1,  3,  3,  3,  3,  3,  3,  3,
/* 160 */   3,  3,  3,  3,  3,  3,  3,  3,  3,  2,
/* 170 */   0,  0,  1,  1,  1,  1,  1,  1,  3,  3,
/* 180 */   3,  3,  3,  3,  3,  5, -1,  3,  2,  3,
/* 190 */   1,  1,  3,  3,  1,  1,  0,  4,  3,  3,
/* 200 */   5,  5
};

#define ARGUMENT_NO_EXPAND  0
#define ARGUMENT_EXPAND     1

static int verify_method(JNIEnv *env, waterClass *class, waterMethod *method);
static int check_instructions(JNIEnv *env, waterMethod *method,
			      waterInstruction **out_instruction_table,
			      int *out_instruction_count,
			      unsigned char **block_o_memory_p);
static int compute_jsr_stuff(JNIEnv *env, waterMethod *method,
			     waterInstruction *instruction_table,
			     int instruction_count);
static int 
merge_jsr_targets_reachable_from(waterMethod *method,
				 waterInstruction *source_instruction,
				 waterInstruction *target_instruction,
				 int *any_changed);
static int mark_as_jsr_target(waterMethod *method,
			      waterInstruction *instruction, int *any_changed);
static int trace_execution_flow(JNIEnv *env, waterMethod *method,
				waterInstruction *instruction_table,
				int instruction_count);
static int verify_instruction(JNIEnv *env, waterMethod *method,
			      waterInstruction *instruction_table,
			      int instruction_count,
			      int instruction_index,
			      waterSlot *new_stack_contents,
			      waterSlot *new_locals_contents,
			      int *out_any_changed);
static int merge_ret(JNIEnv *env,
		     waterMethod *method, waterInstruction *this_instruction,
		     waterInstruction *instruction_table, 
		     int instruction_count,
		     waterSlot *new_stack_contents, int new_stack_size,
		     waterSlot *new_locals_contents,
		     int *any_changed);
static int expand_instruction_table(JNIEnv *env, waterMethod *method,
				    waterInstruction **instruction_table_p,
				    int *instructions_allocated_p,
				    unsigned char **block_o_memory_p,
				    int max_index);
static int is_valid_opcode(unsigned char opcode);
static int instruction_length(unsigned char *code, int pc);
static int compute_instruction_effects(JNIEnv *env,
				       waterMethod *method,
				       waterInstruction *instruction,
				       waterSlot *new_stack_contents,
				       int *new_stack_size,
				       waterSlot *new_locals_contents);
static int compute_branch_targets(waterMethod *method,
				  waterInstruction *instruction, 
				  waterInstruction *instruction_table,
				  int instruction_count);
static int find_instruction(int pc, 
			    waterInstruction *instruction_table,
			    int instruction_count);
static int get_local_index(unsigned char *code);
static int type_slots(int type);
static int get_type(unsigned char *code);
static int get_promoted_type(int type);
static int get_pool_index(unsigned char *code);
static int parse_method_descriptor(JNIEnv *env, waterMethod *method,
				   waterUTF8String *descriptor,
				   waterSlot *arguments, 
				   int *arguments_count,
				   int max_arguments,
				   waterSlot *return_type,
				   int do_expansion);
static int next_argument_type(JNIEnv *env, waterMethod *method,
			      waterUTF8String *descriptor, int *offset_p,
			      waterSlot *results_array, int *results_offset_p,
			      int max_arguments, int do_expansion);
static int is_unconditional_branch_or_return(unsigned char *code);
static int merge_stacks_and_locals(JNIEnv *env,
				   waterMethod *method,
				   waterInstruction *instruction,
				   waterInstruction *source_instruction,
				   waterSlot *stack_contents,
				   int stack_size,
				   waterSlot *locals_contents,
				   int *changed);
static waterClass *find_common_parent(JNIEnv *env,
				      waterClass *class1, waterClass *class2);
static int merge_stacks_exception(JNIEnv *env,
				  waterMethod *method,
				  waterInstruction *instruction,
				  waterInstruction *source_instruction,
				  waterExceptionInfo *exception_info,
				  waterSlot *new_stack_contents,
				  waterSlot *locals_contents,
				  int *changed);
static int verifier_resolveFieldReference(JNIEnv *env,
					  waterMethod *method, int pool_index,
					  waterClass **containing_class,
					  waterSlot *field_slot);
static int verifier_resolveMethodReference(JNIEnv *env,
					   waterMethod *method, int pool_index,
					   waterClass **method_class,
					   waterSlot *method_arguments,
					   int *method_arguments_count,
					   int max_arguments,
					   waterSlot *method_return_type);
static int get_source_type(unsigned char *code);
static int get_target_type(unsigned char *code);
static int get_array_type(unsigned char *code);
static int get_dimensions(unsigned char *code);
static int verifier_castIsOK(water_JNIEnv *wenv, 
			     waterClass *target, waterClass *source);
static int is_jsr(unsigned char opcode);
static void free_instruction_table(waterInstruction *instruction_table,
				   int instruction_count);

int water_verifyClass(JNIEnv *env, waterClass *class)
{
    int i;

    PR_EnterMonitor(class->monitor);

    if(class->state != CLASS_PREPARED) {
#ifdef DEBUG_djm
	fprintf(stderr, 
		"warning: trying to verify a class that's not in the "
		"CLASS_PREPARED state.\n");
#endif
	PR_ExitMonitor(class->monitor);
	return -1;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "verifying class '%s'...\n", class->name);
#endif
    /* first verify the superclass */
    if(class->resolved_superclass) {
	if(water_raiseState(env, class->resolved_superclass, 
			    CLASS_VERIFIED, 0, 0) < 0) {
#ifdef DEBUG_djm
	    fprintf(stderr, "warning: couldn't verify superclass.\n");
#endif
	    class->state = CLASS_VERIFICATION_FAILED;
	    PR_ExitMonitor(class->monitor);
	    return -1;
	}
    }

#ifdef TURN_ON_VERIFIER
    /* don't verify interfaces--they have no code. */
    if(!(class->access_flags & ACC_INTERFACE)) {
	for(i = 0; i < class->method_table_entries; i++) {
	    /* only verify methods that actually have code: */
	    /* i.e., not natives or abstract methods.       */
	    if(!(class->method_table[i].access_flags & ACC_NATIVE ||
		 class->method_table[i].access_flags & ACC_ABSTRACT)) {
		if(verify_method(env, class, &class->method_table[i]) < 0) {
		    if(WATER_ENV(env)->exception_thrown) {
			/* there was already some exception thrown during   */
			/* verification, e.g. we ran out of memory or       */
			/* something. so leave that exception.              */
		    } else {
			water_throwException(WATER_ENV(env), 
					     "java/lang/VerifyError");
		    }
		    class->state = CLASS_VERIFICATION_FAILED;
		    PR_ExitMonitor(class->monitor);
		    return -1;
		}
	    }
	}
    }
#endif
	
    /* made it. */
    class->state = CLASS_VERIFIED;
    PR_ExitMonitor(class->monitor);

    return 0;
}

static int verify_method(JNIEnv *env, waterClass *class, waterMethod *method)
{
    waterInstruction *instruction_table;
    int instruction_count;
    unsigned char *block_o_memory = 0;

    /* verify a method.  see section 4.8 of the JVM spec to understand    */
    /* what we're trying to implement here.                               */

    /* only verify methods that we define, not inherited ones. */
    if(method->clazz != class) {
	return 0;
    }

    /* first do the static pass--make sure all instructions are valid, */
    /* calculate their lengths and initialize the instruction table.   */

    /* these two passes are both part of what Sun defines as 'Pass 3'. */
    if(check_instructions(env, method, 
			  &instruction_table, &instruction_count,
			  &block_o_memory) < 0) {
	/* first pass failed. */
	return -1;
    }

    /* now we do the second pass, where we actually look at the        */
    /* instructions and see how they affect the stack and locals, etc. */
    if(trace_execution_flow(env, method, 
			    instruction_table, instruction_count) < 0) {
	/* failure. */
	free_instruction_table(instruction_table, instruction_count);
	free(block_o_memory);
	return -1;
    }

    /* that's it, it's verified. */
    free_instruction_table(instruction_table, instruction_count);
    free(block_o_memory);
    return 0;
}

static int check_instructions(JNIEnv *env, waterMethod *method,
			      waterInstruction **out_instruction_table,
			      int *out_instruction_count,
			      unsigned char **block_o_memory_p)
{
    int instruction_count = 0;
    waterInstruction *instruction_table = 0;
    int instructions_allocated = 0;
    int length;
    int pc;
    int any_jsrs = 0;

    if(method->code->code_length == 0) {
	/* zero-length code is not allowed. */
	return -1;
    }

    /* reset the instruction alloc chunk. */
    /* we'll keep doubling it in expand_instruction_table so we don't do   */
    /* a zillion mallocs of 100 instructions or whatever for huge methods. */
    instruction_alloc_chunk = INITIAL_INSTRUCTION_ALLOC_CHUNK;

    /* scan once to find the number of instructions and make sure */
    /* they're at least valid.                                    */
    pc = 0;
    while(pc < method->code->code_length) {
	if(!is_valid_opcode(method->code->code[pc])) {
	    return -1;
	}

	if(is_jsr(method->code->code[pc])) {
	    any_jsrs = 1;
	}

	if(instruction_count >= instructions_allocated) {
	    if(expand_instruction_table(env, method,
					&instruction_table,
					&instructions_allocated,
					block_o_memory_p,
					instruction_count) < 0) {
		/* out of memory--exception already thrown. */
		free_instruction_table(instruction_table, instruction_count);
		free(*block_o_memory_p);
		return -1;
	    }
	}

	/* need to see the instruction itself to get the length for */
	/* some instructions.  (notably tableswitch/lookupswitch)   */
	/* not only that, but we need to have the pc.               */

	length = instruction_length(method->code->code, pc);

	instruction_table[instruction_count].index = instruction_count;
	instruction_table[instruction_count].pc = pc;
	instruction_table[instruction_count].length = length;

	pc += length;
	
	/* it's OK to be _just_ past the end of the code, but not   */
	/* any farther than that.                                   */
	if(pc > method->code->code_length) {
	    free_instruction_table(instruction_table, instruction_count);
	    return -1;
	}

	instruction_count++;
    }

    /* we should have just guaranteed that the last byte of the last */
    /* instruction is at the very end of the code.                   */
    assert(pc == method->code->code_length);

    /* now go through the instructions again, and for each            */
    /* instruction, mark the jsr targets that it's reachable from.    */ 
    /* (see VM spec 4.9.6)                                            */
    /* we only need to do this if there are some jsr's in the method. */
    if(any_jsrs) {
	if(compute_jsr_stuff(env, method, 
			     instruction_table, instruction_count) < 0) {
	    return -1;
	}
    }

    *out_instruction_table = instruction_table;
    *out_instruction_count = instruction_count;

    /* yeah, we happy. */
    return 0;
}

static int compute_jsr_stuff(JNIEnv *env, waterMethod *method,
			     waterInstruction *instruction_table,
			     int instruction_count)
{
    int any_changed;
    int i, j;

    any_changed = 1;
    while(any_changed) {
	any_changed = 0;

	for(i = 0; i < instruction_count; i++) {
	    waterInstruction *this_instruction = &instruction_table[i];
	    
	    /* if this instruction is a jsr, mark its first target. */
	    if(is_jsr(method->code->code[this_instruction->pc])) {
		if(compute_branch_targets(method, this_instruction,
					  instruction_table,
					  instruction_count) < 0) {
		    return -1;
		}
		
		assert(this_instruction->num_branch_targets == 1);
		
		if(mark_as_jsr_target(method,
				      &instruction_table[
			              this_instruction->branch_targets[0]],
				      &any_changed) < 0) {
		    return -1;
		}
	    }

	    /* now propagate any jsr's the current instruction is reachable */
	    /* from.                                                        */
	    if(this_instruction->num_jsr_targets_reachable_from > 0) {
		/* all of the possible successors to this instruction */
		/* are reachable from the jsr's that this instruction */
		/* is reachable from.                                 */
		if(!is_unconditional_branch_or_return(&method->code->code[
						      this_instruction->pc])) {
		    if(i == (instruction_count - 1)) {
			/* we just fell off the end of the code. */
			return -1;
		    }
		    
		    if(merge_jsr_targets_reachable_from(method,
							this_instruction,
					      	&instruction_table[i+1],
						&any_changed) < 0) {
			return -1;
		    }
		}

		/* do branches */
		if(compute_branch_targets(method, this_instruction,
					  instruction_table,
					  instruction_count) < 0) {
		    return -1;
		}
		
		for(j = 0; j < this_instruction->num_branch_targets; j++) {
		    if(merge_jsr_targets_reachable_from(method,
							this_instruction,
							&instruction_table[
			 this_instruction->branch_targets[j]],
							&any_changed) < 0) {
			return -1;
		    }
		}
		
		/* do exception handlers */
		for(j = 0; j < method->code->exception_table_length; j++) {
		    waterExceptionInfo *exception_info = 
			&method->code->exception_table[j];
		    
		    if(this_instruction->pc >= exception_info->start_pc &&
		       this_instruction->pc < exception_info->end_pc) {
			int handler_index =
			    find_instruction(exception_info->handler_pc,
					     instruction_table, 
					     instruction_count);
			if(handler_index < 0) {
			    /* invalid exception handler address. */
			    return -1;
			}
			
			if(merge_jsr_targets_reachable_from(method,
							    this_instruction,
					    &instruction_table[handler_index],
						    &any_changed) < 0) {
			    return -1;
			}
		    }
		}
	    }
	}
    }

    return 0;
}

static int 
merge_jsr_targets_reachable_from(waterMethod *method,
				 waterInstruction *source_instruction,
				 waterInstruction *target_instruction,
				 int *any_changed)
{
    int i, j;

    for(i = 0; i < source_instruction->num_jsr_targets_reachable_from; i++) {
	for(j = 0; 
	    j < target_instruction->num_jsr_targets_reachable_from; j++) {
	    if(target_instruction->jsr_targets_reachable_from[j] ==
	       source_instruction->jsr_targets_reachable_from[i]) {
		/* already there */
		break;
	    }
	}

	if(j == target_instruction->num_jsr_targets_reachable_from) {
	    /* not found. */
	    if(j == MAX_NESTED_JSRS) {
		/* too many nested jsr's. */
		return -1;
	    }
	    
	    target_instruction->jsr_targets_reachable_from[j] = 
		source_instruction->jsr_targets_reachable_from[i];
	    target_instruction->num_jsr_targets_reachable_from++;
	    *any_changed = 1;
	}
    }

    if(target_instruction->jsr_returning_from_set) {
	if(method->code->code[target_instruction->pc] == 169 /* ret */ &&
	   target_instruction->jsr_returning_from != 
	   source_instruction->jsr_returning_from) {
	    /* if it's a ret and it's returning from jsr has already been */
	    /* marked, it can't change.                                   */
	    return -1;
	}
	if(target_instruction->jsr_returning_from != 
	   source_instruction->jsr_returning_from) {
	    target_instruction->jsr_returning_from = 
		source_instruction->jsr_returning_from;
	    *any_changed = 1;
	}
    } else {
	target_instruction->jsr_returning_from = 
	    source_instruction->jsr_returning_from;
	target_instruction->jsr_returning_from_set = 1;
	*any_changed = 1;
    }

    return 0;
}

static int mark_as_jsr_target(waterMethod *method,
			      waterInstruction *instruction, int *any_changed)
{
    int i;

    /* mark this instruction as a jsr target--that is, one of the jsr */
    /* targets it is reachable from is itself.                        */
    for(i = 0; i < instruction->num_jsr_targets_reachable_from; i++) {
	if(instruction->jsr_targets_reachable_from[i] == instruction->index) {
	    break;
	}
    }

    if(i == instruction->num_jsr_targets_reachable_from) {
	/* not found. */
	if(i == MAX_NESTED_JSRS) {
	    /* too many nested jsr's--die. */
	    return -1;
	}

	instruction->jsr_targets_reachable_from[i] = instruction->index;
	instruction->num_jsr_targets_reachable_from++;
	*any_changed = 1;
    }

    if(instruction->jsr_returning_from_set) {
	if(method->code->code[instruction->pc] == 169 /* ret */ &&
	   instruction->jsr_returning_from != instruction->index) {
	    /* if it's a ret and it's returning from jsr has already been */
	    /* marked, it can't change.                                   */
	    return -1;
	}
	if(instruction->jsr_returning_from != instruction->index) {
	    instruction->jsr_returning_from = instruction->index;
	    *any_changed = 1;
	}
    } else {
	instruction->jsr_returning_from = instruction->index;
	instruction->jsr_returning_from_set = 1;
	*any_changed = 1;
    }

    return 0;
}

static int trace_execution_flow(JNIEnv *env, waterMethod *method,
				waterInstruction *instruction_table,
				int instruction_count)
{
    int any_changed, i;
    int ignore;
    waterSlot *new_stack_contents;
    waterSlot *new_locals_contents;

    /* we have to have at least one instruction.                 */
    /* (length 0 code and bad instructions have already failed.) */
    assert(instruction_count >= 1);

    /* only allocate these once for the entire method since they're */
    /* always the same size.  pass them into verify_instruction so  */
    /* it won't have to allocate them itself.                       */
    new_stack_contents = 
	(waterSlot *)calloc(method->code->max_stack, sizeof(waterSlot));
    if(!new_stack_contents) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }
    new_locals_contents = 
	(waterSlot *)calloc(method->code->max_locals, sizeof(waterSlot));
    if(!new_locals_contents) {
	free(new_stack_contents);
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }

    /* see JVM spec page 129 */
    instruction_table[0].changed = 1;

    for(i = 0; i < method->code->max_locals; i++) {
	/* start with invalid type for all locals */
	instruction_table[0].locals_contents[i].type = JNI_NONE;
	instruction_table[0].locals_contents[i].class = 0;
    }
    if(method->access_flags & ACC_STATIC) {
	if(parse_method_descriptor(env, method, method->descriptor, 
				   &instruction_table[0].locals_contents[0], 
				   &ignore, method->code->max_locals, 0,
				   ARGUMENT_EXPAND) < 0) {
	    free(new_stack_contents);
	    free(new_locals_contents);
	    return -1;
	}
    } else {
	/* for non-static methods include a 'this' pointer. */
	instruction_table[0].locals_contents[0].type = 0;
	instruction_table[0].locals_contents[0].class = method->clazz;
	if(parse_method_descriptor(env, method, method->descriptor, 
				   &instruction_table[0].locals_contents[1], 
				   &ignore, method->code->max_locals - 1, 
				   0, ARGUMENT_EXPAND) < 0) {
	    free(new_stack_contents);
	    free(new_locals_contents);
	    return -1;
	}
    }
    instruction_table[0].stack_size = 0;
    instruction_table[0].previously_reached = 1;

    any_changed = 1;
    while(any_changed) {
	any_changed = 0;	    
	
	/* look for any changed instructions. */
	for(i = 0; i < instruction_count; i++) {
	    if(instruction_table[i].changed) {
		if(verify_instruction(env, method, 
				      instruction_table, instruction_count, i,
				      new_stack_contents, new_locals_contents,
				      &any_changed) < 0) {
		    free(new_stack_contents);
		    free(new_locals_contents);
		    return -1;
		}
	    }
	}
    }

    /* made it. */
    free(new_stack_contents);
    free(new_locals_contents);
    return 0;
}

static int verify_instruction(JNIEnv *env, waterMethod *method,
			      waterInstruction *instruction_table,
			      int instruction_count,
			      int instruction_index,
			      waterSlot *new_stack_contents,
			      waterSlot *new_locals_contents,
			      int *out_any_changed)
{
    int new_stack_size;
    int any_changed = 0;
    waterInstruction *this_instruction = &instruction_table[instruction_index];
    int i;

    /* the stack and locals have to be set up */
    assert(this_instruction->previously_reached);

    new_stack_contents = 
	(waterSlot *)calloc(method->code->max_stack, sizeof(waterSlot));
    if(!new_stack_contents) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }
    new_locals_contents = 
	(waterSlot *)calloc(method->code->max_locals, sizeof(waterSlot));
    if(!new_locals_contents) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }

    /* mark this instruction as unchanged. */
    this_instruction->changed = 0;

    /* figure out what this instruction does to the stack and locals. */
    if(compute_instruction_effects(env, method, 
				   this_instruction, 
				   new_stack_contents, &new_stack_size,
				   new_locals_contents) < 0) {
	return -1;
    }

    /* if this is a branch, figure out possible branch destinations. */
    if(compute_branch_targets(method, this_instruction,
			      instruction_table,
			      instruction_count) < 0) {
	return -1;
    }

    /* now we have to merge the new stack and locals into all of the */
    /* possible subsequent instructions.                             */

    if(method->code->code[this_instruction->pc] == 169 /* ret */) {
	/* for 'ret' we need to do something completely different. */
	if(merge_ret(env, method, this_instruction, 
		     instruction_table, instruction_count,
		     new_stack_contents, new_stack_size, new_locals_contents,
		     &any_changed) < 0) {
	    return -1;
	}
    } else {
	/* first do the branch targets. */
	for(i = 0; i < this_instruction->num_branch_targets; i++) {
	    waterInstruction *target_instruction =
		&instruction_table[this_instruction->branch_targets[i]];
	    
	    if(merge_stacks_and_locals(env, method,
				       target_instruction, 
				       this_instruction,
				       new_stack_contents, new_stack_size,
				       new_locals_contents,
				       &any_changed) < 0) {
		/* merge failed--the stack was different last time this */
		/* instruction was encountered.                         */
		return -1;
	    }
	}
    
	/* if this is not an unconditional branch, we also have */
	/* to check the next instruction.                       */
	if(!is_unconditional_branch_or_return(&method->code->code[
						  this_instruction->pc])) {
	    if(instruction_index == (instruction_count - 1)) {
		/* we just fell off the end of the code--not allowed. */
		return -1;
	    }
	    
	    if(merge_stacks_and_locals(env, method,
			       &instruction_table[instruction_index + 1],
				       this_instruction,
				       new_stack_contents, new_stack_size,
				       new_locals_contents,
				       &any_changed) < 0) {
		return -1;
	    }
	}
	
	/* also check all of the exception handlers that cover */
	/* this instruction.                                   */
	for(i = 0; i < method->code->exception_table_length; i++) {
	    waterExceptionInfo *exception_info = 
		&method->code->exception_table[i];
	    
	    if(this_instruction->pc >= exception_info->start_pc &&
	       this_instruction->pc < exception_info->end_pc) {
		int handler_index =
		    find_instruction(exception_info->handler_pc,
				     instruction_table, instruction_count);
		if(handler_index < 0) {
		    /* invalid exception handler address. */
		    return -1;
		}
		
		/* merging into an exception handler is slightly different-- */
		/* we don't care about the stack, because it's unrolled when */
		/* the exception occurs.                                     */
		if(merge_stacks_exception(env, method,
					  &instruction_table[handler_index],
					  this_instruction,
					  exception_info,
					  new_stack_contents,
					  new_locals_contents,
					  &any_changed) < 0) {
		    return -1;
		}
	    }
	}
    }

    /* it worked. */
    if(any_changed) {
	/* only set this to 1--never set it to zero. */
	*out_any_changed = 1;
    }
    return 0;
}

static int merge_ret(JNIEnv *env,
		     waterMethod *method, waterInstruction *this_instruction,
		     waterInstruction *instruction_table, 
		     int instruction_count,
		     waterSlot *new_stack_contents, int new_stack_size,
		     waterSlot *new_locals_contents,
		     int *any_changed)
{
    int i, j;
    waterSlot *subroutine_merged_locals;
    int which_locals_flags;

    /* we need to find all the jsr's that called the subroutine that */
    /* this ret is returning from.  for the instructions after each  */
    /* of those jsr's, we merge the stacks and locals, except we     */
    /* merge the locals differently--  if they were never touched    */
    /* by the subroutine, then we use the values from _before_       */
    /* the subroutine was called.  otherwise we use the normal       */
    /* values.  see VM spec 4.9.6.                                   */

    if(!this_instruction->jsr_returning_from_set) {
	/* if we never figured out what subroutine this 'ret' is    */
	/* returning from (which must mean we hit the 'ret' without */
	/* hitting a 'jsr' first), that's an error.                 */
	return -1;
    }

    subroutine_merged_locals = 
	(waterSlot *)calloc(method->code->max_locals, sizeof(waterSlot));
    if(!subroutine_merged_locals) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }

    for(i = 0; i < instruction_count; i++) {
	if(is_jsr(method->code->code[instruction_table[i].pc]) &&
	   instruction_table[i].branch_targets[0] == 
	   this_instruction->jsr_returning_from) {
	    /* found one. */
	    if(i == (instruction_count - 1)) {
		free(subroutine_merged_locals);
		return -1;
	    }

	    /* start with the locals from before the jsr. */
	    memcpy(subroutine_merged_locals, 
		   instruction_table[i].locals_contents,
		   sizeof(waterSlot) * method->code->max_locals);

	    /* now, for each local accessed or modified in the subroutine, */
	    /* use the subroutine's new type.                              */
	    
	    /* find the right set */
	    for(j = 0; this_instruction->num_jsr_targets_reachable_from; j++) {
		if(this_instruction->jsr_targets_reachable_from[j] ==
		   this_instruction->jsr_returning_from) {
		    break;
		}
	    }
	    assert(j != this_instruction->num_jsr_targets_reachable_from);
	    which_locals_flags = j;

	    for(j = 0; j < method->code->max_locals; j++) {
		if(this_instruction->
		   jsr_locals_touched[which_locals_flags][j]) {
		    subroutine_merged_locals[j] = new_locals_contents[j];
		}
	    }

	    if(merge_stacks_and_locals(env, method,
				       &instruction_table[i + 1],
				       this_instruction,
				       new_stack_contents,
				       new_stack_size,
				       subroutine_merged_locals,
				       any_changed) < 0) {
		free(subroutine_merged_locals);
		return -1;
	    }				       
	}
    }

    free(subroutine_merged_locals);
    return 0;
}

static int expand_instruction_table(JNIEnv *env, waterMethod *method,
				    waterInstruction **instruction_table_p,
				    int *instructions_allocated_p,
				    unsigned char **block_o_memory_p,
				    int max_index)
{
    waterInstruction *new_instruction_table;
    int new_length;
    unsigned char *new_block_o_memory;
    int new_block_o_memory_size;
    int one_instruction_chunk;
    int i, j;

    if(max_index < *instructions_allocated_p) {
	/* already have enough space. */
	return 0;
    }

    new_length = max_index + 1 + instruction_alloc_chunk;
    
    /* if we make it back here for the same method, allocate twice */
    /* as many the next time.                                      */
    instruction_alloc_chunk *= 2;

    new_instruction_table = 
	(waterInstruction *)
	realloc(*instruction_table_p, sizeof(waterInstruction) * new_length);
    if(!new_instruction_table) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }

    memset(&new_instruction_table[*instructions_allocated_p], 0,
	   sizeof(waterInstruction) * 
	   (new_length - *instructions_allocated_p));

    /* the block_o_memory holds all of the stacks and locals         */
    /* so we don't have to malloc them individually for every single */
    /* instruction.                                                  */
    new_block_o_memory_size = 
	new_length * 
	((method->code->max_locals + method->code->max_stack) *
	 sizeof(waterSlot) +
	 /* room for jsr local flags */
	 method->code->max_locals * MAX_NESTED_JSRS * sizeof(int));
    new_block_o_memory = 
	(unsigned char *)realloc(*block_o_memory_p, new_block_o_memory_size);
    if(!new_block_o_memory) {
	water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	return -1;
    }

    /* the size of the block_o_memory piece that each instruction gets */
    one_instruction_chunk =
	(method->code->max_locals + method->code->max_stack) * 
	 sizeof(waterSlot) +
	 /* room for jsr local flags */
	 method->code->max_locals * MAX_NESTED_JSRS * sizeof(int);

#if defined(DEBUG_djm) && 0
    printf("the block_o_memory is now at %p and is of size %d\n",
	   new_block_o_memory, new_block_o_memory_size);
    if(new_block_o_memory != *block_o_memory_p && *block_o_memory_p != 0) {
	printf("it moved!  it used to be at %p\n", *block_o_memory_p);
    }
#endif

    /* if the block moved, adjust all the old pointers, too; */
    /* else just do the new ones.                            */
    if(new_block_o_memory != *block_o_memory_p) {
	i = 0;
    } else {
	i = *instructions_allocated_p;
    }
    for(; i < new_length; i++) {
	/* set the pointers into the block_o_memory. */
	/* XXX make sure there are no alignment problems here. */
	unsigned char *our_chunk = 
	    &new_block_o_memory[i * one_instruction_chunk];

	new_instruction_table[i].stack_contents = (waterSlot *)&our_chunk[0];
	new_instruction_table[i].locals_contents = 
	    (waterSlot *)&our_chunk[method->code->max_stack * 
				   sizeof(waterSlot)];
	for(j = 0; j < MAX_NESTED_JSRS; j++) {
	    new_instruction_table[i].jsr_locals_touched[j] =
		(int *)&our_chunk[((method->code->max_stack +
				    method->code->max_locals) * 
				   sizeof(waterSlot)) +
				 method->code->max_locals * j * sizeof(int)];
	}
    }

    *instructions_allocated_p = new_length;
    *instruction_table_p = new_instruction_table;
    *block_o_memory_p = new_block_o_memory;
    return 0;
}

static int is_valid_opcode(unsigned char opcode)
{
    /* just check if this opcode represents a real instruction. */
    return (opcode <= 185) ||
	(opcode >= 187 && opcode <= 201);
}

static int instruction_length(unsigned char *code, int pc)
{
    int length;
    unsigned char opcode;

    opcode = code[pc];
    length = instruction_length_table[opcode];
    if(length == 0) {
	/* variable length instruction--'tableswitch', 'lookupswitch', or */
	/* 'wide'                                                         */

	/* XXX make sure we never read past end of code even if the       */
	/* instruction is invalid                                         */
	if(opcode == 171) {
	    /* lookupswitch */
	    /* the number of pairs is at the first 4-byte aligned byte after */
	    /* the opcode + 4.                                               */
	    int npairs_pc = ((pc + 1 + 3) & ~0x3) + 4;
	    jint npairs;

	    GET_INT_NOINC(npairs, code, npairs_pc);
	    length = (npairs_pc + 4 + (npairs * 8)) - pc;
	} else if(opcode == 170) {
	    /* tableswitch */
	    /* after the default value there is a low and high value, */
	    /* followed by one 32-bit offset for each value between   */
	    /* the low and the high value, inclusive.                 */
	    int lowvalue_pc = ((pc + 1 + 3) & ~0x3) + 4;
	    jint lowvalue, highvalue;

	    GET_INT_NOINC(lowvalue, code, lowvalue_pc);
	    GET_INT_NOINC(highvalue, code, lowvalue_pc + 4);
	    
	    length = (lowvalue_pc + 8 + ((highvalue - lowvalue + 1) * 4)) - pc;
	} else if(opcode == 196) {
	    /* wide */
	    /* the 'wide' instruction is usually 4 bytes long.  it's only  */
	    /* 6 bytes long if the instruction being widened is an 'iinc'. */
	    if(code[pc + 1] == 132 /* iinc */) {
		length = 6;
	    } else {
		length = 4;
	    }
	} else {
	    /* there are no other variable-length instructions. */
	    assert(0 && "bad variable-length instruction");
	}
    }
    /* some reasonable maximum length-- the maximum length of a method */
    assert(length > 0 && length <= 65535);

    return length;
}

/* leave these outside the body of the below routine to help emacs */
#define PUSH_STACK(pushed_type, pushed_class)				\
do {									\
    int slots = type_slots((pushed_type));				\
    if(((*new_stack_size) + slots - 1) >= method->code->max_stack) {	\
	/* stack overflow */						\
	return -1;							\
    }									\
    if(slots == 2) {							\
	new_stack_contents[*new_stack_size].type =			\
	    (pushed_type) | JNI_IS_FIRST_HALF;				\
	new_stack_contents[*new_stack_size].class = (pushed_class);	\
	(*new_stack_size)++;						\
	new_stack_contents[*new_stack_size].type =			\
	    (pushed_type) | JNI_IS_SECOND_HALF;				\
	new_stack_contents[*new_stack_size].class = (pushed_class);	\
	(*new_stack_size)++;						\
    } else {								\
	new_stack_contents[*new_stack_size].type = (pushed_type);	\
	new_stack_contents[*new_stack_size].class = (pushed_class);	\
	(*new_stack_size)++;						\
    }									\
} while(0)
    
#define SET_LOCAL_TYPE(local_index, set_type, set_class)		     \
do {									     \
    int slots = type_slots((set_type));					     \
    if(((local_index) + slots - 1) >= method->code->max_locals) {	     \
	/* invalid index */						     \
	return -1;							     \
    }									     \
									     \
    if(slots == 2) {							     \
	new_locals_contents[(local_index)].type =			     \
	    (set_type) | JNI_IS_FIRST_HALF;				     \
	new_locals_contents[(local_index)].class = (set_class);		     \
	new_locals_contents[(local_index) + 1].type =			     \
	    (set_type) | JNI_IS_SECOND_HALF;				     \
	new_locals_contents[(local_index) + 1].class = (set_class);	     \
    } else {								     \
	new_locals_contents[(local_index)].type = (set_type);		     \
	new_locals_contents[(local_index)].class = (set_class);		     \
    }									     \
									     \
    if(which_locals_flags >= 0) {					     \
	instruction->							     \
	    jsr_locals_touched[which_locals_flags][(local_index)] = 1;	     \
	if(slots == 2) {						     \
	    instruction->						     \
		jsr_locals_touched[which_locals_flags][(local_index)+1] = 1; \
	}								     \
    }									     \
} while(0)
       
#define CHECK_LOCAL_TYPE(local_index, desired_type)			     \
do {									     \
    int slots = type_slots((desired_type));				     \
    if(((local_index) + slots - 1) >= method->code->max_locals) {	     \
	/* invalid index */						     \
	return -1;							     \
    }									     \
									     \
    if(slots == 2) {							     \
	if((new_locals_contents[(local_index)].type !=			     \
	    ((desired_type) | JNI_IS_FIRST_HALF)) ||			     \
	   (new_locals_contents[(local_index) + 1].type !=		     \
	    ((desired_type) | JNI_IS_SECOND_HALF))) {			     \
	    /* bad type */						     \
	    return -1;							     \
	}								     \
    } else {								     \
	if(new_locals_contents[(local_index)].type != (desired_type)) {	     \
	    /* bad type */						     \
	    return -1;							     \
	}								     \
    }									     \
									     \
    if(which_locals_flags >= 0) {					     \
	instruction->							     \
	    jsr_locals_touched[which_locals_flags][(local_index)] = 1;	     \
	if(slots == 2) {						     \
	    instruction->						     \
		jsr_locals_touched[which_locals_flags][(local_index)+1] = 1; \
	}								     \
    }									     \
} while(0)
       
#define CHECK_STACK_SIZE(minimum_size)			\
do {							\
    if((*new_stack_size) < (minimum_size)) {		\
	/* stack too small */				\
	return -1;					\
    }							\
} while(0)
     
#define CHECK_STACK_TYPE(stack_index, desired_type)			      \
do {									      \
    int slots = type_slots((desired_type));				      \
    if((*new_stack_size) < (stack_index)) {				      \
	/* stack too small */						      \
	return -1;							      \
    }									      \
									      \
    if(slots == 2) {							      \
	if((new_stack_contents[(*new_stack_size) - (stack_index)].type !=     \
	    ((desired_type) | JNI_IS_FIRST_HALF)) ||			      \
	   (new_stack_contents[(*new_stack_size) - (stack_index) + 1].type != \
	    ((desired_type) | JNI_IS_SECOND_HALF))) {			      \
	    /* bad type */						      \
	    return -1;							      \
	}								      \
    } else {								      \
	if(new_stack_contents[(*new_stack_size) - (stack_index)].type !=      \
	   (desired_type)) {						      \
	    /* bad type */						      \
	    return -1;							      \
	}								      \
    }									      \
} while(0)
       
#define POP_STACK(popped_type)						\
do {									\
    int slots = type_slots((popped_type));				\
    if((*new_stack_size) < slots) {					\
	/* stack too small */						\
	return -1;							\
    }									\
    									\
    (*new_stack_size) -= slots;                             		\
} while(0)
       
#define GET_STACK_CLASS(slot_number, lvalue_class)			\
do {									\
    CHECK_STACK_SIZE((slot_number));					\
    (lvalue_class) =							\
	new_stack_contents[(*new_stack_size) - (slot_number)].class;	\
} while(0)
       
#define GET_STACK_TYPE(slot_number, lvalue_type)			\
do {									\
    CHECK_STACK_SIZE((slot_number));					\
    (lvalue_type) =							\
	new_stack_contents[(*new_stack_size) - (slot_number)].type;	\
} while(0)
       
#define GET_STACK_SLOT(slot_number, lvalue_type, lvalue_class)		\
do {									\
    CHECK_STACK_SIZE((slot_number));					\
    (lvalue_type) =							\
	new_stack_contents[(*new_stack_size) - (slot_number)].type;	\
									\
    (lvalue_class) =							\
	new_stack_contents[(*new_stack_size) - (slot_number)].class;	\
} while(0)

#define IS_SINGLE_WORD(type)						\
     (!(((type) & JNI_IS_FIRST_HALF) || ((type) & JNI_IS_SECOND_HALF)))

#define IS_DOUBLE_WORD(type1, type2)					\
     (((type1) & JNI_IS_FIRST_HALF) && ((type2) & JNI_IS_SECOND_HALF))

static int compute_instruction_effects(JNIEnv *env,
				       waterMethod *method,
				       waterInstruction *instruction,
				       waterSlot *new_stack_contents,
				       int *new_stack_size,
				       waterSlot *new_locals_contents)
{
    int local_index, pool_index, type, type2, type3, type4, promoted_type,
	source_type, target_type, dimensions, i, tag, stack_slot;
    waterClass *class, *class2, *class3, *class4, 
	*object_class, *method_class;
    waterSlot *method_arguments;
    int method_arguments_count;
    waterSlot method_return_type;
    waterSlot return_type;
    waterSlot field_slot;
    unsigned char *instruction_code;
    int which_locals_flags;

    /* the stack and locals have to be set up */
    assert(instruction->previously_reached);

    instruction_code = &method->code->code[instruction->pc];

    /* we start with the new stack and locals being the same as the old. */
    memcpy(new_stack_contents, instruction->stack_contents,
	   method->code->max_stack * sizeof(waterSlot));
    *new_stack_size = instruction->stack_size;
    memcpy(new_locals_contents, instruction->locals_contents,
	   method->code->max_locals * sizeof(waterSlot));

    /* if we're in a jsr subroutine, see which set of locals we're */
    /* modifying.                                                  */
    which_locals_flags = -1;
    if(instruction->jsr_returning_from_set) {
	for(i = 0; instruction->num_jsr_targets_reachable_from; i++) {
	    if(instruction->jsr_targets_reachable_from[i] ==
	       instruction->jsr_returning_from) {
		break;
	    }
	}
	assert(i != instruction->num_jsr_targets_reachable_from);
	which_locals_flags = i;
    }

    switch(instruction_code[0]) {
    case(0): /* nop */
	break;

    case(1): /* aconst_null */
	/* null's are special--they count as instances of any class. */
	/* so we set the class to 0.  however, there can also be     */
	/* null's that are instances of a certain class, and those   */
	/* have to be checkcast'ed if you're going to assign them to */
	/* something else.                                           */
	PUSH_STACK(0, 0);
	break;

    case(2): /* iconst_m1 */
    case(3): /* iconst_0 */
    case(4): /* iconst_1 */
    case(5): /* iconst_2 */
    case(6): /* iconst_3 */
    case(7): /* iconst_4 */
    case(8): /* iconst_5 */
    case(9): /* lconst_0 */
    case(10): /* lconst_1 */
    case(11): /* fconst_0 */
    case(12): /* fconst_1 */
    case(13): /* fconst_2 */
    case(14): /* dconst_0 */
    case(15): /* dconst_1 */
    case(16): /* bipush */
    case(17): /* sipush */
	type = get_type(instruction_code);
	PUSH_STACK(type, 0);
	break;

    case(18): /* ldc */
    case(19): /* ldc_w */
    case(20): /* ldc2_w */
	pool_index = get_pool_index(instruction_code);
	tag = method->clazz->constant_pool[pool_index].tag;
	if(instruction_code[0] == 20) {
	    /* for ldc2_w, the constant can only be a long or double. */
	    if(tag == CONSTANT_LONG_TAG) {
		type = JNI_LONG;
	    } else if(tag == CONSTANT_DOUBLE_TAG) {
		type = JNI_DOUBLE;
	    } else {
		/* bad constant type */
		return -1;
	    }
	} else {
	    /* for ldc and ldc_w, the constant can be float, int, or string. */
	    if(tag == CONSTANT_FLOAT_TAG) {
		type = JNI_FLOAT;
	    } else if(tag == CONSTANT_INTEGER_TAG) {
		type = JNI_INT;
	    } else if(tag == CONSTANT_STRING_TAG) {
		type = 0;
	    } else {
		/* bad constant type */
		return -1;
	    }
	}

	/* these push one value--either int, long, float, double, or */
	/* java/lang/String.                                         */
	if(type == 0) {
	    PUSH_STACK(0, WATER_ENV(env)->java_lang_string_class);
	} else {
	    PUSH_STACK(type, 0);
	}
	break;
 
    case(21): /* iload */
    case(22): /* lload */
    case(23): /* fload */
    case(24): /* dload */
    case(25): /* aload */
    case(26): /* iload_0 */
    case(27): /* iload_1 */
    case(28): /* iload_2 */
    case(29): /* iload_3 */
    case(30): /* lload_0 */
    case(31): /* lload_1 */
    case(32): /* lload_2 */
    case(33): /* lload_3 */
    case(34): /* fload_0 */
    case(35): /* fload_1 */
    case(36): /* fload_2 */
    case(37): /* fload_3 */
    case(38): /* dload_0 */
    case(39): /* dload_1 */
    case(40): /* dload_2 */
    case(41): /* dload_3 */
    case(42): /* aload_0 */
    case(43): /* aload_1 */
    case(44): /* aload_2 */
    case(45): /* aload_3 */
	local_index = get_local_index(instruction_code);
	type = get_type(instruction_code);

	if(type == 0 && 
	   (local_index + type_slots(JNI_RETURN_ADDRESS)) <=
	   method->code->max_locals &&
	   new_locals_contents[local_index].type == JNI_RETURN_ADDRESS) {
	    /* special case-- returnAddress's can be operated on */
	    /* by the astore/aload instructions.                 */
	    type = JNI_RETURN_ADDRESS;
	}	
	CHECK_LOCAL_TYPE(local_index, type);
	PUSH_STACK(type, new_locals_contents[local_index].class);
	break;

    case(46): /* iaload */
    case(47): /* laload */
    case(48): /* faload */
    case(49): /* daload */
    case(50): /* aaload */
    case(51): /* baload */
    case(52): /* caload */
    case(53): /* saload */
	type = get_type(instruction_code);
	promoted_type = get_promoted_type(type);

	/* reference followed by int */
	CHECK_STACK_TYPE(2, 0);
	CHECK_STACK_TYPE(1, JNI_INT);

	/* make sure the reference is really to an array, and an array of */
	/* the proper type.                                               */
	GET_STACK_CLASS(2, class);

	/* null here is OK--just throws a NullPointerException at runtime. */
	if(class &&
	   (!(class->class_type & JNI_IS_ARRAY) ||
	    !(class->elements_class->class_type == type ||
	      (type == 0 && 
	       (class->elements_class->class_type & JNI_IS_ARRAY))))) {
	    /* bad reference type */
	    return -1;
	}

	POP_STACK(JNI_INT);
	POP_STACK(0);

	PUSH_STACK(promoted_type, 
		   (promoted_type == 0) ? class->elements_class : 0);
	break;

    case(54): /* istore */
    case(55): /* lstore */
    case(56): /* fstore */
    case(57): /* dstore */
    case(58): /* astore */
    case(59): /* istore_0 */
    case(60): /* istore_1 */
    case(61): /* istore_2 */
    case(62): /* istore_3 */
    case(63): /* lstore_0 */
    case(64): /* lstore_1 */
    case(65): /* lstore_2 */
    case(66): /* lstore_3 */
    case(67): /* fstore_0 */
    case(68): /* fstore_1 */
    case(69): /* fstore_2 */
    case(70): /* fstore_3 */
    case(71): /* dstore_0 */
    case(72): /* dstore_1 */
    case(73): /* dstore_2 */
    case(74): /* dstore_3 */
    case(75): /* astore_0 */
    case(76): /* astore_1 */
    case(77): /* astore_2 */
    case(78): /* astore_3 */
	local_index = get_local_index(instruction_code);
	type = get_type(instruction_code);

	if(type == 0 && 
	   *new_stack_size >= type_slots(JNI_RETURN_ADDRESS) &&
	   new_stack_contents[(*new_stack_size) - 
			     type_slots(JNI_RETURN_ADDRESS)].type ==
	   JNI_RETURN_ADDRESS) {
	    /* special case-- returnAddress's can be operated on */
	    /* by the astore/aload instructions.                 */
	    type = JNI_RETURN_ADDRESS;
	}
	CHECK_STACK_TYPE(type_slots(type), type);
	GET_STACK_CLASS(type_slots(type), class);
	SET_LOCAL_TYPE(local_index, type, class);
	POP_STACK(type);
	break;

    case(79): /* iastore */
    case(80): /* lastore */
    case(81): /* fastore */
    case(82): /* dastore */
    case(83): /* aastore */
    case(84): /* bastore */
    case(85): /* castore */
    case(86): /* sastore */
	type = get_type(instruction_code);
	promoted_type = get_promoted_type(type);

	/* reference, then index (int), then value */
	CHECK_STACK_TYPE(2 + type_slots(promoted_type), 0);
	CHECK_STACK_TYPE(1 + type_slots(promoted_type), JNI_INT);
	CHECK_STACK_TYPE(type_slots(promoted_type),     promoted_type);

	/* make sure the reference is really to an array, and an array of */
	/* the proper type.                                               */
	GET_STACK_CLASS(2 + type_slots(promoted_type), class);
	if(class &&
	   (!(class->class_type & JNI_IS_ARRAY) ||
	    !(class->elements_class->class_type == type ||
	      (type == 0 && 
	       (class->elements_class->class_type & JNI_IS_ARRAY))))) {
	    /* bad reference type */
	    return -1;
	}

	if(type == 0) {
	    /* make sure the given reference type is OK to store */
	    /* in the array.                                     */
	    if(!verifier_castIsOK(WATER_ENV(env), class->elements_class,
				  new_stack_contents[(*new_stack_size) -
					   type_slots(promoted_type)].class)) {
		return -1;
	    }
	}

	POP_STACK(promoted_type);
	POP_STACK(JNI_INT);
	POP_STACK(0);
	break;

    case(87): /* pop */
	POP_STACK(JNI_ANY_WORD);
	break;

    case(88): /* pop2 */
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	break;

    case(89): /* dup */
	GET_STACK_SLOT(1, type, class);
	if(!(IS_SINGLE_WORD(type))) {
	    /* it's not allowed to be half of something. */
	    return -1;
	}
	PUSH_STACK(type, class);
	break;

    case(90): /* dup_x1 */
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	if(!(IS_SINGLE_WORD(type) &&
	     IS_SINGLE_WORD(type2))) {
	    /* no halves allowed. */
	    return -1;
	}
	   
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type, class);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	break;

    case(91): /* dup_x2 */
	GET_STACK_SLOT(3, type3, class3);
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	/* word1 must be 32-bit.  word3 and word2 together can be a 64-bit */
	/* value.                                                          */
	if(!(IS_SINGLE_WORD(type) &&
	     ((IS_SINGLE_WORD(type2) && IS_SINGLE_WORD(type3)) ||
	      (IS_DOUBLE_WORD(type3, type2))))) {
	    return -1;
	}

	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type, class);
	PUSH_STACK(type3, class3);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	break;

    case(92): /* dup2 */
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	if(!((IS_SINGLE_WORD(type) && IS_SINGLE_WORD(type2)) ||
	     (IS_DOUBLE_WORD(type2, type)))) {
	    return -1;
	}
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	break;

    case(93): /* dup2_x1 */
	GET_STACK_SLOT(3, type3, class3);
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	if(!(IS_SINGLE_WORD(type3) &&
	     ((IS_SINGLE_WORD(type) && IS_SINGLE_WORD(type2)) ||
	      (IS_DOUBLE_WORD(type2, type))))) {
	    return -1;
	}
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	PUSH_STACK(type3, class3);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	break;

    case(94): /* dup2_x2 */
	/* these are really getting obnoxious */
	GET_STACK_SLOT(4, type4, class4);
	GET_STACK_SLOT(3, type3, class3);
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	if(!(((IS_SINGLE_WORD(type3) && IS_SINGLE_WORD(type4)) ||
	      (IS_DOUBLE_WORD(type4, type3))) &&
	     ((IS_SINGLE_WORD(type2) && IS_SINGLE_WORD(type)) ||
	      (IS_DOUBLE_WORD(type2, type))))) {
	    return -1;
	}
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	PUSH_STACK(type4, class4);
	PUSH_STACK(type3, class3);
	PUSH_STACK(type2, class2);
	PUSH_STACK(type, class);
	break;

    case(95): /* swap */
	GET_STACK_SLOT(2, type2, class2);
	GET_STACK_SLOT(1, type, class);
	if(!(IS_SINGLE_WORD(type) && IS_SINGLE_WORD(type2))) {
	    return -1;
	}
	POP_STACK(JNI_ANY_WORD);
	POP_STACK(JNI_ANY_WORD);
	PUSH_STACK(type, class);
	PUSH_STACK(type2, class2);
	break;

    case(96): /* iadd */
    case(97): /* ladd */
    case(98): /* fadd */
    case(99): /* dadd */
    case(100): /* isub */
    case(101): /* lsub */
    case(102): /* fsub */
    case(103): /* dsub */
    case(104): /* imul */
    case(105): /* lmul */
    case(106): /* fmul */
    case(107): /* dmul */
    case(108): /* idiv */
    case(109): /* ldiv */
    case(110): /* fdiv */
    case(111): /* ddiv */
    case(112): /* irem */
    case(113): /* lrem */
    case(114): /* frem */
    case(115): /* drem */
    case(126): /* iand */
    case(127): /* land */
    case(128): /* ior */
    case(129): /* lor */
    case(130): /* ixor */
    case(131): /* lxor */
	/* arithmetic operations that take two values of the same type */
	/* and return a value of that type.                            */
	type = get_type(instruction_code);

	CHECK_STACK_TYPE(2 * type_slots(type), type);
	CHECK_STACK_TYPE(1 * type_slots(type), type);
	POP_STACK(type);
	POP_STACK(type);
	PUSH_STACK(type, 0);
	break;
	
    case(116): /* ineg */
    case(117): /* lneg */
    case(118): /* fneg */
    case(119): /* dneg */
	type = get_type(instruction_code);
	
	CHECK_STACK_TYPE(type_slots(type), type);
	/* don't ask me why I bother to do this */
	POP_STACK(type);
	PUSH_STACK(type, 0);
	break;

    case(120): /* ishl */
    case(121): /* lshl */
    case(122): /* ishr */
    case(123): /* lshr */
    case(124): /* iushr */
    case(125): /* lushr */
	type = get_type(instruction_code);

	/* value followed by an int */
	CHECK_STACK_TYPE(type_slots(type) + 1, type);
	CHECK_STACK_TYPE(1, JNI_INT);

	POP_STACK(JNI_INT);
	POP_STACK(type);
	PUSH_STACK(type, 0);
	break;

    case(132): /* iinc */
	local_index = get_local_index(instruction_code);
	CHECK_LOCAL_TYPE(local_index, JNI_INT);
	break;

    case(133): /* i2l */
    case(134): /* i2f */
    case(135): /* i2d */
    case(136): /* l2i */
    case(137): /* l2f */
    case(138): /* l2d */
    case(139): /* f2i */
    case(140): /* f2l */
    case(141): /* f2d */
    case(142): /* d2i */
    case(143): /* d2l */
    case(144): /* d2f */
    case(145): /* i2b */
    case(146): /* i2c */
    case(147): /* i2s */
	source_type = get_source_type(instruction_code);
	target_type = get_target_type(instruction_code);
	if(source_type == JNI_LONG || source_type == JNI_DOUBLE) {
	    CHECK_STACK_TYPE(2, source_type);
	} else {
	    CHECK_STACK_TYPE(1, source_type);
	}
	POP_STACK(source_type);
	PUSH_STACK(target_type, 0);
	break;

    case(148): /* lcmp */
    case(149): /* fcmpl */
    case(150): /* fcmpg */
    case(151): /* dcmpl */
    case(152): /* dcmpg */
	type = get_type(instruction_code);

	CHECK_STACK_TYPE(2 * type_slots(type), type);
	CHECK_STACK_TYPE(1 * type_slots(type), type);
       	POP_STACK(type);
	POP_STACK(type);
	PUSH_STACK(JNI_INT, 0);
	break;

    case(153): /* ifeq */
    case(154): /* ifne */
    case(155): /* iflt */
    case(156): /* ifge */
    case(157): /* ifgt */
    case(158): /* ifle */
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);
	break;

    case(159): /* if_icmpeq */
    case(160): /* if_icmpne */
    case(161): /* if_icmplt */
    case(162): /* if_icmpge */
    case(163): /* if_icmpgt */
    case(164): /* if_icmple */
	CHECK_STACK_TYPE(2, JNI_INT);
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);
	POP_STACK(JNI_INT);
	break;

    case(165): /* if_acmpeq */
    case(166): /* if_acmpne */
	CHECK_STACK_TYPE(2, 0);
	CHECK_STACK_TYPE(1, 0);
	POP_STACK(0);
	POP_STACK(0);
	break;

    case(167): /* goto */
	/* doesn't touch stack or locals */
	break;

    case(168): /* jsr */
	/* special wacky returnAddress type which is really just an int */
	PUSH_STACK(JNI_RETURN_ADDRESS, 0);
	break;

    case(169): /* ret */
	local_index = get_local_index(instruction_code);
	CHECK_LOCAL_TYPE(local_index, JNI_RETURN_ADDRESS);
	break;

    case(170): /* tableswitch */
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);
	break;

    case(171): /* lookupswitch */
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);	
	break;

    case(172): /* ireturn */
	if(parse_method_descriptor(env, method, method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	/* the return type can be any of the int-like types */
	if(return_type.type != JNI_INT &&
	   return_type.type != JNI_BOOLEAN &&
	   return_type.type != JNI_BYTE &&
	   return_type.type != JNI_CHAR &&
	   return_type.type != JNI_SHORT) {
	    return -1;
	}
	CHECK_STACK_TYPE(1, JNI_INT);
	break;

    case(173): /* lreturn */
	if(parse_method_descriptor(env, method,
				   method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	if(return_type.type != JNI_LONG) {
	    return -1;
	}
	CHECK_STACK_TYPE(2, JNI_LONG);
	break;

    case(174): /* freturn */
	if(parse_method_descriptor(env, method,
				   method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	if(return_type.type != JNI_FLOAT) {
	    return -1;
	}
	CHECK_STACK_TYPE(1, JNI_FLOAT);
	break;

    case(175): /* dreturn */
	if(parse_method_descriptor(env, method,
				   method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	if(return_type.type != JNI_DOUBLE) {
	    return -1;
	}
	CHECK_STACK_TYPE(2, JNI_DOUBLE);
	break;

    case(176): /* areturn */
	CHECK_STACK_TYPE(1, 0);
	GET_STACK_CLASS(1, class);
	/* it is always OK to return null.  otherwise, make sure that   */
	/* the class we're returning is compatible with the return type */
	/* in the descriptor for this method.                           */

	if(parse_method_descriptor(env, method,
				   method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	if(!verifier_castIsOK(WATER_ENV(env), return_type.class, class)) {
	    /* returning bad type */
	    return -1;
	}
	break;

    case(177): /* return */
	if(parse_method_descriptor(env, method,
				   method->descriptor, 0, 0, 0,
				   &return_type, ARGUMENT_NO_EXPAND) < 0) {
	    /* bad descriptor */
	    return -1;
	}
	if(return_type.type != JNI_NONE) {
	    return -1;
	}
	break;

    case(178): /* getstatic */
	pool_index = get_pool_index(instruction_code);
	if(verifier_resolveFieldReference(env, method, pool_index, 0,
					  &field_slot) < 0) {
	    /* bad reference */
	    return -1;
	}

	PUSH_STACK(field_slot.type, field_slot.class);
	break;

    case(179): /* putstatic */
	pool_index = get_pool_index(instruction_code);
	if(verifier_resolveFieldReference(env, method, pool_index, 0,
					  &field_slot) < 0) {
	    /* bad reference */
	    return -1;
	}

	CHECK_STACK_TYPE(type_slots(field_slot.type), field_slot.type);
	if(field_slot.type == 0) {
	    if(!verifier_castIsOK(WATER_ENV(env), field_slot.class, 
				  new_stack_contents[(*new_stack_size)-1].
				  class)) {
		/* trying to put the wrong type */
		return -1;
	    }
	}
	POP_STACK(field_slot.type);

	break;

    case(180): /* getfield */
	pool_index = get_pool_index(instruction_code);
	if(verifier_resolveFieldReference(env, 
					  method, pool_index, &object_class,
					  &field_slot) < 0) {
	    /* bad reference */
	    return -1;
	}

	CHECK_STACK_TYPE(1, 0);
	if(!verifier_castIsOK(WATER_ENV(env), object_class,
			      new_stack_contents[(*new_stack_size)-1].class)) {
	    /* object on the stack has the wrong class */
	    return -1;
	}

	POP_STACK(0);
	PUSH_STACK(field_slot.type, field_slot.class);
	break;

    case(181): /* putfield */
	pool_index = get_pool_index(instruction_code);
	if(verifier_resolveFieldReference(env, method, 
					  pool_index, &object_class,
					  &field_slot) < 0) {
	    /* bad reference */
	    return -1;
	}

	CHECK_STACK_TYPE(1 + type_slots(field_slot.type), 0);
	CHECK_STACK_TYPE(type_slots(field_slot.type), field_slot.type);
	if(!verifier_castIsOK(WATER_ENV(env), object_class,
			      new_stack_contents[(*new_stack_size) - 1 - 
				     type_slots(field_slot.type)].class)) {
	    /* object on the stack has the wrong class */
	    return -1;
	}
	if(field_slot.type == 0) {
	    if(!verifier_castIsOK(WATER_ENV(env), field_slot.class, 
				  new_stack_contents[(*new_stack_size)-1].
				  class)) {
		/* trying to put the wrong type */
		return -1;
	    }
	}
	POP_STACK(field_slot.type);
	POP_STACK(0);
	break;

    case(182): /* invokevirtual */
    case(183): /* invokespecial */
    case(184): /* invokestatic */
    case(185): /* invokeinterface */
	pool_index = get_pool_index(instruction_code);

	/* XXX get rid of this arbitrary limit and figure out the */
	/* real number of arguments instead.                      */
	method_arguments = 
	    (waterSlot *)calloc(50, sizeof(waterSlot));
	if(!method_arguments) {
	    water_throwException(WATER_ENV(env), "java/lang/OutOfMemoryError");
	    return -1;
	}

	if(verifier_resolveMethodReference(env, 
					   method, pool_index,
					   &method_class, 
					   method_arguments,
					   &method_arguments_count,
					   *new_stack_size,
					   &method_return_type) < 0) {
	    /* bad descriptor */
	    free(method_arguments);
	    return -1;
	}

	/* XXX ugh.  method_arguments's memory is leaked if any of the      */
	/* stack-checking macros fail.  needs to be fixed, but verification */
	/* failures should be rare anyway.                                  */

	/* check that the arguments on the stack are right */
	stack_slot = 0;
	for(i = method_arguments_count - 1; i >= 0; i--) {
	    stack_slot += type_slots(method_arguments[i].type);
	    CHECK_STACK_TYPE(stack_slot, method_arguments[i].type);
	    if(method_arguments[i].type == 0) {
		/* if it's a reference, make sure it's castable to the */
		/* argument type.                                      */
		GET_STACK_CLASS(stack_slot, class);
		if(!verifier_castIsOK(WATER_ENV(env), 
				      method_arguments[i].class, class)) {
		    /* bad argument type */
		    free(method_arguments);
		    return -1;
		}
	    }
	}

	/* now, unless it's a static method, check the 'this' pointer */
	if(instruction_code[0] != 184 /* invokestatic */) {
	    stack_slot += type_slots(0);
	    CHECK_STACK_TYPE(stack_slot, 0);
	    GET_STACK_CLASS(stack_slot, class);
	    if(!verifier_castIsOK(WATER_ENV(env), method_class, class)) {
		/* calling method on bad reference */
		free(method_arguments);
		return -1;
	    }
	}

	/* pop the arguments and push the return type */
	for(i = method_arguments_count - 1; i >= 0; i--) {
	    POP_STACK(method_arguments[i].type);
	}
	/* pop 'this' pointer if necessary */
	if(instruction_code[0] != 184 /* invokestatic */) {
	    POP_STACK(0);
	}
	if(method_return_type.type != JNI_NONE) {
	    PUSH_STACK(method_return_type.type, method_return_type.class);
	}
	free(method_arguments);
	break;
	
    case(187): /* new */
	pool_index = get_pool_index(instruction_code);
	class = water_resolveClassByIndex(env, method->clazz, pool_index,
					  CLASS_FETUS);
	if(!class) {
	    /* bad reference */
	    return -1;
	}
	PUSH_STACK(0, class);
	/* XXX we are probably supposed to do something about marking this */
	/* as uninitialized and not letting it do anything until a         */
	/* constructor is called on it, and only letting constructors be   */
	/* called on uninitialized objects.                                */
	break;

    case(188): /* newarray */
	type = get_array_type(instruction_code);
	if(type < 0) {
	    /* bad type */
	    return -1;
	}

	class = water_getPrimitiveClass(env, type | JNI_IS_ARRAY);
	if(!class) {
	    return -1;
	}

	/* the array size */
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);

	PUSH_STACK(0, class);
	break;

    case(189): /* anewarray */
	pool_index = get_pool_index(instruction_code);
	class = water_resolveClassByIndex(env, method->clazz, pool_index,
					  CLASS_FETUS);
	if(!class) {
	    /* bad reference */
	    return -1;
	}

	class = water_getCorrespondingArrayClass(env, class);
	if(!class) {
	    return -1;
	}
	
	CHECK_STACK_TYPE(1, JNI_INT);
	POP_STACK(JNI_INT);
	PUSH_STACK(0, class);
	break;

    case(190): /* arraylength */
	CHECK_STACK_TYPE(1, 0);
	GET_STACK_CLASS(1, class);
	
	/* it must be either null or an array class */
	if(class && !(class->class_type & JNI_IS_ARRAY)) {
	    return -1;
	}
	POP_STACK(0);
	PUSH_STACK(JNI_INT, 0);
	break;

    case(191): /* athrow */
	CHECK_STACK_TYPE(1, 0);

	/* make sure it is a subclass of Throwable */
	if(!verifier_castIsOK(WATER_ENV(env), 
			      WATER_ENV(env)->java_lang_throwable_class,
			      new_stack_contents[(*new_stack_size)-1].class)) {
	    return -1;
	}

	/* it doesn't get popped. */
	break;

    case(192): /* checkcast */
	CHECK_STACK_TYPE(1, 0);

	/* now, I believe what we can do here is set the object's class */
	/* to the class it's being cast to.  if the cast fails, we will */
	/* enter an exception handler, which will clear the contents of */
	/* the stack and replace it with a Throwable object, so this    */
	/* reference will disappear anyway.                             */
	pool_index = get_pool_index(instruction_code);
	class = water_resolveClassByIndex(env, method->clazz, pool_index,
					  CLASS_FETUS);
	if(!class) {
	    /* bad reference */
	    return -1;
	}
	POP_STACK(0);
	/* XXX maybe we should only narrow here, and not widen. */
	PUSH_STACK(0, class);
	break;

    case(193): /* instanceof */
	CHECK_STACK_TYPE(1, 0);
	POP_STACK(0);
	PUSH_STACK(JNI_INT, 0);
	break;

    case(194): /* monitorenter */
    case(195): /* monitorexit */
	/* just has to be a reference on the stack */
	CHECK_STACK_TYPE(1, 0);
	POP_STACK(0);
	break;

    case(196): /* wide */
	local_index = get_local_index(instruction_code);
	if(instruction_code[1] == 132 /* iinc */) {
	    CHECK_LOCAL_TYPE(local_index, JNI_INT);
	} else if(instruction_code[1] == 169 /* ret */) {
	    CHECK_LOCAL_TYPE(local_index, JNI_RETURN_ADDRESS);
	} else if(instruction_code[1] >= 21 && 
		  instruction_code[1] <= 25 /* [ilfda]load */) {
	    type = get_type(instruction_code + 1);
	    CHECK_LOCAL_TYPE(local_index, type);
	    PUSH_STACK(type, new_locals_contents[local_index].class);
	} else if(instruction_code[1] >= 54 && 
		  instruction_code[1] <= 58 /* [ilfda]store */) {
	    type = get_type(instruction_code + 1);
	    CHECK_STACK_TYPE(type_slots(type), type);
	    GET_STACK_CLASS(type_slots(type), class);
	    SET_LOCAL_TYPE(local_index, type, class);
	    POP_STACK(type);
	} else {
	    /* invalid 'wide' instruction */
	    return -1;
	}
	break;

    case(197): /* multianewarray */
	pool_index = get_pool_index(instruction_code);
	class = water_resolveClassByIndex(env, method->clazz, pool_index,
					  CLASS_FETUS);
	if(!class) {
	    /* bad reference */
	    return -1;
	}

	dimensions = get_dimensions(instruction_code);
	/* this is a static constraint that we should have caught already: */
	/* (in is_valid_instruction())                                     */
	assert(dimensions >= 1);

	/* make sure the referenced class is an array class with the same */
	/* number of dimensions as desired.                               */
	if(!(class->class_type & JNI_IS_ARRAY) ||
	   (dimensions != water_arrayDimensions(env, class))) {
	    return -1;
	}

	/* make sure all the dimensions are on the stack */
	for(i = dimensions; i > 0; i--) {
	    CHECK_STACK_TYPE(i, JNI_INT);
	}

	/* pop the dimensions */
	for(i = 0; i < dimensions; i++) {
	    POP_STACK(JNI_INT);
	}

	/* push the new array reference */
	PUSH_STACK(0, class);
	break;

    case(198): /* ifnull */
    case(199): /* ifnonnull */
	CHECK_STACK_TYPE(1, 0);
	POP_STACK(0);
	break;

    case(200): /* goto_w */
	/* no stack interaction */
	break;

    case(201): /* jsr_w */
	/* special wacky returnAddress type which is really just an int */
	PUSH_STACK(JNI_RETURN_ADDRESS, 0);
	break;

    default:
	/* should have already caught invalid instructions */
	assert(0 && "invalid instruction in compute_instruction_effects");
	return -1;
    }

    return 0;
}

#undef PUSH_STACK
#undef SET_LOCAL_TYPE
#undef CHECK_LOCAL_TYPE
#undef CHECK_STACK_SIZE
#undef CHECK_STACK_TYPE
#undef POP_STACK
#undef GET_STACK_CLASS
#undef GET_STACK_TYPE
#undef GET_STACK_SLOT
#undef IS_SINGLE_WORD
#undef IS_DOUBLE_WORD

static int compute_branch_targets(waterMethod *method,
				  waterInstruction *instruction, 
				  waterInstruction *instruction_table,
				  int instruction_count)
{
    jshort offset;
    jint long_offset;
    int target_instruction_index;
    unsigned char *code = method->code->code;
    int lowvalue_pc;
    jint lowvalue, highvalue;
    int npairs_pc;
    jint npairs;
    int i;

    /* this may be called more than once for the same instruction, so */
    /* don't redo the work if we don't have to.                       */
    if(instruction->branch_targets_computed) {
	return 0;
    }

    instruction->num_branch_targets = 0;

    switch(code[instruction->pc]) {
    case(153): /* ifeq */
    case(154): /* ifne */
    case(155): /* iflt */
    case(156): /* ifge */
    case(157): /* ifgt */
    case(158): /* ifle */
    case(159): /* if_icmpeq */
    case(160): /* if_icmpne */
    case(161): /* if_icmplt */
    case(162): /* if_icmpge */
    case(163): /* if_icmpgt */
    case(164): /* if_icmple */
    case(165): /* if_acmpeq */
    case(166): /* if_acmpne */
    case(167): /* goto */
    case(198): /* ifnull */
    case(199): /* ifnonnull */
    case(168): /* jsr */
	/* these are all the same--a single possible branch offset */
	/* right after the opcode.                                 */
	GET_SHORT_NOINC(offset, code, instruction->pc + 1);
	target_instruction_index = 
	    find_instruction(instruction->pc + offset, 
			     instruction_table, instruction_count);
	if(target_instruction_index < 0) {
	    /* target is invalid--verification fails. */
	    return -1;
	}

	instruction->branch_targets = (int *)calloc(1, sizeof(int));
	if(!instruction->branch_targets) {
	    return -1;
	}
	instruction->num_branch_targets = 1;
	instruction->branch_targets[0] = target_instruction_index;
	break;

    case(169): /* ret */
	/* ret has no branch targets */
	break;

    case(170): /* tableswitch */
	lowvalue_pc = ((instruction->pc + 1 + 3) & ~0x3) + 4;

	GET_INT_NOINC(lowvalue, code, lowvalue_pc);
	GET_INT_NOINC(highvalue, code, lowvalue_pc + 4);
	
	/* get the default offset */
	GET_INT_NOINC(long_offset, code, lowvalue_pc - 4);
	target_instruction_index = 
	    find_instruction(instruction->pc + long_offset, 
			     instruction_table, instruction_count);
	if(target_instruction_index < 0) {
	    /* target is invalid--verification fails. */
	    return -1;
	}

	instruction->branch_targets = 
	    (int *)calloc((highvalue - lowvalue + 1) + 1 /* default */, 
			  sizeof(int));
	if(!instruction->branch_targets) {
	    return -1;
	}

	instruction->branch_targets[instruction->num_branch_targets++] =
	    target_instruction_index;

	for(i = 0; i < (highvalue - lowvalue + 1); i++) {
	    GET_INT_NOINC(long_offset, code, lowvalue_pc + 8 + (4 * i));
	    target_instruction_index = 
		find_instruction(instruction->pc + long_offset, 
				 instruction_table, instruction_count);
	    if(target_instruction_index < 0) {
		/* target is invalid--verification fails. */
		return -1;
	    }

	    instruction->branch_targets[instruction->num_branch_targets++] =
		target_instruction_index;    
	}
	break;

    case(171): /* lookupswitch */
	npairs_pc = ((instruction->pc + 1 + 3) & ~0x3) + 4;
	
	GET_INT_NOINC(npairs, code, npairs_pc);
	/* get the default offset */
	GET_INT_NOINC(long_offset, code, npairs_pc - 4);
	target_instruction_index = 
	    find_instruction(instruction->pc + long_offset, 
			     instruction_table, instruction_count);
	if(target_instruction_index < 0) {
	    /* target is invalid--verification fails. */
	    return -1;
	}

	instruction->branch_targets = 
	    (int *)calloc(npairs + 1 /* default */, 
			  sizeof(int));
	if(!instruction->branch_targets) {
	    return -1;
	}

	instruction->branch_targets[instruction->num_branch_targets++] =
	    target_instruction_index;

	for(i = 0; i < npairs; i++) {
	    GET_INT_NOINC(long_offset, code, npairs_pc + 4 + (8 * i) + 4);
	    target_instruction_index = 
		find_instruction(instruction->pc + long_offset, 
				 instruction_table, instruction_count);
	    if(target_instruction_index < 0) {
		/* target is invalid--verification fails. */
		return -1;
	    }

	    instruction->branch_targets[instruction->num_branch_targets++] =
		target_instruction_index;    
	}
	break;

    case(200): /* goto_w */
    case(201): /* jsr_w */
	/* just like a regular goto, but the offset is 4 bytes long. */
	GET_INT_NOINC(long_offset, code, instruction->pc + 1);
	target_instruction_index = 
	    find_instruction(instruction->pc + long_offset, 
			     instruction_table, instruction_count);
	if(target_instruction_index < 0) {
	    /* target is invalid--verification fails. */
	    return -1;
	}

	instruction->branch_targets = (int *)calloc(1, sizeof(int));
	if(!instruction->branch_targets) {
	    return -1;
	}
	instruction->num_branch_targets = 1;
	instruction->branch_targets[0] = target_instruction_index;
	break;

    default:
	/* no other instructions cause branches */
    }

    instruction->branch_targets_computed = 1;
    return 0;
}

static int find_instruction(int pc, 
			    waterInstruction *instruction_table,
			    int instruction_count)
{
    int i;
    
    for(i = 0; i < instruction_count; i++) {
	if(instruction_table[i].pc == pc) {
	    return i;
	}
    }

    /* not found */
    return -1;
}

static int get_local_index(unsigned char *code)
{
    jubyte byte_index;
    jushort short_index;

    switch(code[0]) {
    case(26): /* iload_0 */
    case(30): /* lload_0 */
    case(34): /* fload_0 */
    case(38): /* dload_0 */
    case(42): /* aload_0 */
    case(59): /* istore_0 */
    case(63): /* lstore_0 */
    case(67): /* fstore_0 */
    case(71): /* dstore_0 */
    case(75): /* astore_0 */
	return 0;
	
    case(27): /* iload_1 */
    case(31): /* lload_1 */
    case(35): /* fload_1 */
    case(39): /* dload_1 */
    case(43): /* aload_1 */
    case(60): /* istore_1 */
    case(64): /* lstore_1 */
    case(68): /* fstore_1 */
    case(72): /* dstore_1 */
    case(76): /* astore_1 */
	return 1;
	
    case(28): /* iload_2 */
    case(32): /* lload_2 */
    case(36): /* fload_2 */
    case(40): /* dload_2 */
    case(44): /* aload_2 */
    case(61): /* istore_2 */
    case(65): /* lstore_2 */
    case(69): /* fstore_2 */
    case(73): /* dstore_2 */
    case(77): /* astore_2 */
	return 2;
	
    case(29): /* iload_3 */
    case(33): /* lload_3 */
    case(37): /* fload_3 */
    case(41): /* dload_3 */
    case(45): /* aload_3 */
    case(62): /* istore_3 */
    case(66): /* lstore_3 */
    case(70): /* fstore_3 */
    case(74): /* dstore_3 */
    case(78): /* astore_3 */
	return 3;
	
    case(21): /* iload */
    case(22): /* lload */
    case(23): /* fload */
    case(24): /* dload */
    case(25): /* aload */
    case(54): /* istore */
    case(55): /* lstore */
    case(56): /* fstore */
    case(57): /* dstore */
    case(58): /* astore */
    case(132): /* iinc */
    case(169): /* ret */
	GET_BYTE_NOINC(byte_index, code, 1);
	return byte_index;

    case(196): /* wide */
	GET_SHORT_NOINC(short_index, code, 2);
	return short_index;

    default:
	assert(0);
	return -1;
    }	    

    assert(0);
    return -1;
}

static int type_slots(int type)
{
    switch(type) {
    case 0: /* we use 0 to represent references in the verifier */
    case JNI_BOOLEAN:
    case JNI_BYTE:
    case JNI_CHAR:
    case JNI_SHORT:
    case JNI_INT:
    case JNI_FLOAT:
    case JNI_ANY_WORD:
    case JNI_RETURN_ADDRESS:
	return 1;

    case JNI_LONG:
    case JNI_DOUBLE:
	return 2;

    default:
	assert(0);
	return -1;
    }
}

static int get_type(unsigned char *code)
{
    /* I guess this kind of thing should really be a table instead of a */
    /* switch, but filling in tables gives me a headache.               */
    switch(code[0]) {
    case(2): /* iconst_m1 */
    case(3): /* iconst_0 */
    case(4): /* iconst_1 */
    case(5): /* iconst_2 */
    case(6): /* iconst_3 */
    case(7): /* iconst_4 */
    case(8): /* iconst_5 */
    case(21): /* iload */
    case(26): /* iload_0 */
    case(27): /* iload_1 */
    case(28): /* iload_2 */
    case(29): /* iload_3 */
    case(46): /* iaload */
    case(54): /* istore */
    case(59): /* istore_0 */
    case(60): /* istore_1 */
    case(61): /* istore_2 */
    case(62): /* istore_3 */
    case(79): /* iastore */
    case(96): /* iadd */
    case(100): /* isub */
    case(104): /* imul */
    case(108): /* idiv */
    case(112): /* irem */
    case(126): /* iand */
    case(130): /* ixor */
    case(120): /* ishl */
    case(122): /* ishr */
    case(124): /* iushr */
    case(128): /* ior */
    case(116): /* ineg */
    case(16): /* bipush */
    case(17): /* sipush */
	return JNI_INT;

    case(9): /* lconst_0 */
    case(10): /* lconst_1 */
    case(22): /* lload */
    case(30): /* lload_0 */
    case(31): /* lload_1 */
    case(32): /* lload_2 */
    case(33): /* lload_3 */
    case(47): /* laload */
    case(55): /* lstore */
    case(63): /* lstore_0 */
    case(64): /* lstore_1 */
    case(65): /* lstore_2 */
    case(66): /* lstore_3 */
    case(80): /* lastore */
    case(97): /* ladd */
    case(101): /* lsub */
    case(105): /* lmul */
    case(109): /* ldiv */
    case(113): /* lrem */
    case(127): /* land */
    case(129): /* lor */
    case(131): /* lxor */
    case(117): /* lneg */
    case(121): /* lshl */
    case(123): /* lshr */
    case(125): /* lushr */
    case(148): /* lcmp */
	return JNI_LONG;

    case(11): /* fconst_0 */
    case(12): /* fconst_1 */
    case(13): /* fconst_2 */
    case(23): /* fload */
    case(34): /* fload_0 */
    case(35): /* fload_1 */
    case(36): /* fload_2 */
    case(37): /* fload_3 */
    case(48): /* faload */
    case(56): /* fstore */
    case(67): /* fstore_0 */
    case(68): /* fstore_1 */
    case(69): /* fstore_2 */
    case(70): /* fstore_3 */
    case(81): /* fastore */
    case(98): /* fadd */
    case(102): /* fsub */
    case(106): /* fmul */
    case(110): /* fdiv */
    case(114): /* frem */
    case(118): /* fneg */
    case(149): /* fcmpl */
    case(150): /* fcmpg */
	return JNI_FLOAT;

    case(14): /* dconst_0 */
    case(15): /* dconst_1 */
    case(24): /* dload */
    case(38): /* dload_0 */
    case(39): /* dload_1 */
    case(40): /* dload_2 */
    case(41): /* dload_3 */
    case(49): /* daload */
    case(57): /* dstore */
    case(71): /* dstore_0 */
    case(72): /* dstore_1 */
    case(73): /* dstore_2 */
    case(74): /* dstore_3 */
    case(82): /* dastore */
    case(99): /* dadd */
    case(103): /* dsub */
    case(107): /* dmul */
    case(111): /* ddiv */
    case(115): /* drem */
    case(119): /* dneg */
    case(151): /* dcmpl */
    case(152): /* dcmpg */
	return JNI_DOUBLE;

    case(25): /* aload */
    case(42): /* aload_0 */
    case(43): /* aload_1 */
    case(44): /* aload_2 */
    case(45): /* aload_3 */
    case(50): /* aaload */
    case(58): /* astore */
    case(75): /* astore_0 */
    case(76): /* astore_1 */
    case(77): /* astore_2 */
    case(78): /* astore_3 */
    case(83): /* aastore */
	return 0;

    case(51): /* baload */
    case(84): /* bastore */
	return JNI_BYTE;

    case(52): /* caload */
    case(85): /* castore */
	return JNI_CHAR;

    case(53): /* saload */
    case(86): /* sastore */
	return JNI_SHORT;

    default:
	/* don't recognize this one */
	assert(0);
	return -1;
    }
}

static int get_promoted_type(int type) 
{
    switch(type) {
    case JNI_BYTE:
    case JNI_CHAR:
    case JNI_SHORT:
	return JNI_INT;

    default:
	return type;
    }
}

static int get_pool_index(unsigned char *code)
{
    jubyte byte_index;
    jushort short_index;

    switch(code[0]) {
    case(18): /* ldc */
	GET_BYTE_NOINC(byte_index, code, 1);
	return byte_index;

    case(19): /* ldc_w */
    case(20): /* ldc2_w */
    case(178): /* getstatic */
    case(179): /* putstatic */
    case(180): /* getfield */
    case(181): /* putfield */
    case(187): /* new */
    case(189): /* anewarray */
    case(192): /* checkcast */
    case(197): /* multianewarray */
    case(182): /* invokevirtual */
    case(183): /* invokespecial */
    case(184): /* invokestatic */
    case(185): /* invokeinterface */
	GET_SHORT_NOINC(short_index, code, 1);
	return short_index;
	
    default:
	assert(0);
	return -1;
    }

    assert(0);
    return -1;
}

static int parse_method_descriptor(JNIEnv *env, waterMethod *method,
				   waterUTF8String *descriptor,
				   waterSlot *arguments, 
				   int *arguments_count,
				   int max_arguments,
				   waterSlot *return_type,
				   int do_expansion)
{
    int offset;

    if(arguments_count) {
	*arguments_count = 0;
    }
    /* skip the leading '(' */
    offset = 1;
    while(offset < descriptor->length &&
	  descriptor->contents[offset] != ')') {
	if(arguments) {
	    if(next_argument_type(env, method, descriptor, &offset,
				  arguments, arguments_count, max_arguments,
				  do_expansion) < 0) {
		/* bad descriptor */
		return -1;
	    }
	} else {
	    if(next_argument_type(env, method, descriptor, &offset, 
				  0, 0, 0, do_expansion) < 0) {
		/* bad descriptor */
		return -1;
	    }
	}
    }
    if(offset == descriptor->length) {
	/* we hit the end of the thing without finding the ')' */
	/* so the descriptor is broken.                        */
	return -1;
    }

    /* skip close paren */
    offset++;
    if(next_argument_type(env, method, descriptor, &offset, 
			  return_type, 0, 1, do_expansion) < 0) {
	return -1;
    }

    return 0;
}

static int next_argument_type(JNIEnv *env, waterMethod *method,
			      waterUTF8String *descriptor, int *offset_p,
			      waterSlot *results_array, int *results_offset_p,
			      int max_arguments, int do_expansion)
{
    int name_end, name_length;
    int is_class, in_inner_class;
    char *name;
    int type;
    int results_offset;

    if(*offset_p >= descriptor->length) {
	return -1;
    }

    if(results_offset_p) {
	results_offset = *results_offset_p;
    } else {
	results_offset = 0;
    }

    if(results_array && results_offset >= max_arguments) {
	/* we're off the end of the results array */
	return -1;
    }

    if(descriptor->contents[*offset_p] == 'L' ||
       descriptor->contents[*offset_p] == '[') {
	is_class = (descriptor->contents[*offset_p] == 'L');
	name_end = *offset_p + 1;
	in_inner_class = 0;
	while(1) {
	    if(name_end >= descriptor->length) {
		return -1;
	    }

	    /* for a class we wait for the ';'.  for an array we wait */
	    /* for the primitive type.                                */
	    if(((is_class || in_inner_class) 
		&& (descriptor->contents[name_end] == ';')) ||
	       (!(is_class || in_inner_class) 
		&& (descriptor->contents[name_end] == 'Z' ||
		    descriptor->contents[name_end] == 'B' ||
		    descriptor->contents[name_end] == 'C' ||
		    descriptor->contents[name_end] == 'S' ||
		    descriptor->contents[name_end] == 'I' ||
		    descriptor->contents[name_end] == 'J' ||
		    descriptor->contents[name_end] == 'F' ||
		    descriptor->contents[name_end] == 'D'))) {
		break;
	    }
	    if(!is_class && descriptor->contents[name_end] == 'L') {
		in_inner_class = 1;
	    }

	    name_end++;
	}

	name_length = name_end - *offset_p + 1;
	if(is_class) {
	    (*offset_p)++;
	    name_length -= 2;
	}

	if(results_array) {
	    name = (char *)malloc(name_length + 1);
	    if(!name) {
		return -1;
	    }
	    memcpy(name, &descriptor->contents[*offset_p], name_length);
	    name[name_length] = '\0';
	    
	    results_array[results_offset].type = 0;
	    results_array[results_offset].class = 
		water_findClass(env, method->clazz->class_loader, 
				name, CLASS_FETUS, 0, 0);
	    free(name);
	    if(!results_array[results_offset].class) {
		return -1;
	    }

	    results_offset++;
	}

	(*offset_p) += name_length;
	if(is_class) {
	    (*offset_p)++;
	}
    } else {
	if(results_array) {
	    results_array[results_offset].class = 0;
	}
	switch(descriptor->contents[*offset_p]) {
	case 'Z':    
	case 'B':    
	case 'C':    
	case 'S':    
	case 'I':    
	    if(results_array) { 
		results_array[results_offset].type = JNI_INT;     
		results_offset++;
	    }  
	    break;
	case 'F':    
	    if(results_array) { 
		results_array[results_offset].type = JNI_FLOAT;   
		results_offset++;
	    }  
	    break;
	case 'V':    
	    if(results_array) { 
		results_array[results_offset].type = JNI_NONE;    
		results_offset++;
	    }  
	    break;
	case 'J':
	case 'D':
	    if(descriptor->contents[*offset_p] == 'J') {
		type = JNI_LONG;
	    } else {
		type = JNI_DOUBLE;
	    }
	    if(results_array) {
		if(do_expansion == ARGUMENT_EXPAND) {
		    results_array[results_offset].type = 
			type | JNI_IS_FIRST_HALF;
		    results_array[results_offset+1].type = 
			type | JNI_IS_SECOND_HALF;
		    results_array[results_offset+1].class = 0;
		    results_offset += 2;
		} else {
		    results_array[results_offset].type = type;
		    results_offset++;
		}
	    }
	    break;

	default:
	    return -1;
	}
	(*offset_p)++;
    }

    if(results_offset_p) {
	*results_offset_p = results_offset;
    }

    return 0;
}

static int is_unconditional_branch_or_return(unsigned char *code)
{
    switch(code[0]) {
    case(167): /* goto */
    case(168): /* jsr */
    case(169): /* ret */
    case(170): /* tableswitch */
    case(171): /* lookupswitch */
    case(172): /* ireturn */
    case(173): /* lreturn */
    case(174): /* freturn */
    case(175): /* dreturn */
    case(176): /* areturn */
    case(177): /* return */
    case(191): /* athrow */
    case(200): /* goto_w */
    case(201): /* jsr_w */
	return 1;

    default:
	return 0;
    }
}

static int merge_stacks_and_locals(JNIEnv *env,
				   waterMethod *method,
				   waterInstruction *instruction,
				   waterInstruction *source_instruction,
				   waterSlot *stack_contents,
				   int stack_size,
				   waterSlot *locals_contents,
				   int *changed)
{
    int i, j;
    waterClass *common_parent;
    int which_locals_flags;

    /* we have to 'or' together the jsr_locals_touched. */
    for(i = 0; i < source_instruction->num_jsr_targets_reachable_from; i++) {
	/* first see if the target instruction is even reachable from */
	/* the same subroutine we're in--if not, we're not merging    */
	/* anything.                                                  */
	for(j = 0; 
	    j < instruction->num_jsr_targets_reachable_from; j++) {
	    if(source_instruction->jsr_targets_reachable_from[i] ==
	       instruction->jsr_targets_reachable_from[j]) {
		break;
	    }
	}
	if(j == instruction->num_jsr_targets_reachable_from) {
	    /* target instruction isn't reachable from this subroutine. */
	    break;
	}
	which_locals_flags = j;

	for(j = 0; j < method->code->max_locals; j++) {
	    if(source_instruction->jsr_locals_touched[i][j] == 1) {
		instruction->jsr_locals_touched[which_locals_flags][j] = 1;
	    }
	}
    }

    /* if we've never been to this instruction before, just copy the stack */
    /* and locals in here.                                                 */
    if(!instruction->previously_reached) {
	instruction->stack_size = stack_size;
	memcpy(instruction->stack_contents, stack_contents,
	       sizeof(waterSlot) * stack_size);
	memcpy(instruction->locals_contents, locals_contents,
	       sizeof(waterSlot) * method->code->max_locals);	
	instruction->changed = 1;
	*changed = 1;
    } else {
	/* do the stack */
	if(instruction->stack_size != stack_size) {
	    /* stacks must be the same size. */
	    return -1;
	}

	for(i = 0; i < stack_size; i++) {
	    if(instruction->stack_contents[i].type != stack_contents[i].type) {
		/* bad type on stack. */
		return -1;
	    }

	    if(stack_contents[i].type == 0) {
		/* if one of the classes is 0 (a null reference),  */
		/* then the parent is the one that is not null.    */
		/* if both are null then the merged class is null. */
		if(instruction->stack_contents[i].class == 0 ||
		   stack_contents[i].class == 0) {
		    if(instruction->stack_contents[i].class == 0) {
			common_parent = stack_contents[i].class;
		    } else {
			common_parent = instruction->stack_contents[i].class;
		    }
		} else {
		    common_parent = 
			find_common_parent(env,
					   instruction->stack_contents[i].
					   class, 
					   stack_contents[i].class);
		    if(!common_parent) {
			/* two non-null classes must have a common parent. */
			return -1;
		    }
		}

		if(common_parent != instruction->stack_contents[i].class) {
		    instruction->stack_contents[i].class = common_parent;
		    instruction->changed = 1;
		    *changed = 1;
		}
	    }
	}

	/* do the locals. */
	for(i = 0; i < method->code->max_locals; i++) {
	    if(instruction->locals_contents[i].type != 
	       locals_contents[i].type) {
		/* the local becomes unusable. */
		instruction->locals_contents[i].type = JNI_NONE;
		instruction->changed = 1;
		*changed = 1;
	    } else {
		if(locals_contents[i].type == 0) {
		    if(instruction->locals_contents[i].class == 0 ||
		       locals_contents[i].class == 0) {
			if(instruction->locals_contents[i].class == 0) {
			    common_parent = locals_contents[i].class;
			} else {
			    common_parent = 
				instruction->locals_contents[i].class;
			}
		    } else {		    
			common_parent = 
			    find_common_parent(env, 
					       instruction->locals_contents[i].
					       class, 
					       locals_contents[i].class);
			if(!common_parent) {
			    /* the classes must have a common parent. */
			    return -1;
			}
		    }
		    
		    if(common_parent != 
		       instruction->locals_contents[i].class) {
			instruction->locals_contents[i].class = common_parent;
			instruction->changed = 1;
			*changed = 1;
		    }
		}
	    }
	}
    }

    instruction->previously_reached = 1;

    return 0;
}

static waterClass *find_common_parent(JNIEnv *env,
				      waterClass *class1, waterClass *class2)
{
    /* either class1 is a subclass of class2, or class2 is a subclass */
    /* of class1, or their only common parent is java.lang.Object.    */
    if(water_isSuperclass(env, class1, class2)) {
	return class1;
    } else if(water_isSuperclass(env, class2, class1)) {
	return class2;
    } else {
	return WATER_ENV(env)->java_lang_object_class;
    }
}

static int merge_stacks_exception(JNIEnv *env,
				  waterMethod *method,
				  waterInstruction *instruction,
				  waterInstruction *source_instruction,
				  waterExceptionInfo *exception_info,
				  waterSlot *new_stack_contents,
				  waterSlot *locals_contents,
				  int *changed)
{
    int new_stack_size;

    /* set the stack to have exactly one object, of the exception class. */
    new_stack_size = 1;
    new_stack_contents[0].type = 0;
    if(exception_info->catch_type == 0) {
	/* this handler handles any exception.  so set the type just */
	/* to Throwable.                                             */
	new_stack_contents[0].class = 
	    WATER_ENV(env)->java_lang_throwable_class;
    } else {
	new_stack_contents[0].class = 
	    water_resolveClassByIndex(env, method->clazz,
				      exception_info->catch_type, CLASS_FETUS);
	if(!new_stack_contents[0].class) {
	    return -1;
	}
    }

    return merge_stacks_and_locals(env, method, instruction,
				   source_instruction,
				   new_stack_contents, new_stack_size,
				   locals_contents, changed);
}

static int verifier_resolveFieldReference(JNIEnv *env,
					  waterMethod *method, int pool_index,
					  waterClass **containing_class,
					  waterSlot *field_slot)
{
    waterConstantReference *field_ref;
    waterConstantNameAndType *nameandtype_ref;
    int offset;

    if(pool_index >= method->clazz->constant_pool_count) {
	return -1;
    }
    
    if(method->clazz->constant_pool[pool_index].tag != CONSTANT_FIELDREF_TAG) {
	return -1;
    }
    field_ref = &method->clazz->constant_pool[pool_index].value.reference;

    /* we don't always care about the containing class. */
    if(containing_class) {
	*containing_class = 
	    water_resolveClassByIndex(env, method->clazz, 
				      field_ref->class_index, 
				      CLASS_FETUS);
	if(!*containing_class) {
	    return -1;
	}
    }
    
    if(field_ref->name_and_type_index >= method->clazz->constant_pool_count) {
	return -1;
    }

    if(method->clazz->constant_pool[field_ref->name_and_type_index].tag != 
       CONSTANT_NAMEANDTYPE_TAG) {
	return -1;
    }
    nameandtype_ref = 
	&method->clazz->constant_pool[
        field_ref->name_and_type_index].value.name_and_type;
    
    if(nameandtype_ref->descriptor_index >=
       method->clazz->constant_pool_count) {
	return -1;
    }

    if(method->clazz->constant_pool[
       nameandtype_ref->descriptor_index].tag != 
       CONSTANT_UTF8_TAG) {
	return -1;
    }

    offset = 0;
    return 
	next_argument_type(env, method,
			   &method->clazz->constant_pool[
                           nameandtype_ref->descriptor_index].
                           value.utf8_value, &offset,
			   field_slot, 0, 1, ARGUMENT_NO_EXPAND);
}

static int verifier_resolveMethodReference(JNIEnv *env,
					   waterMethod *method, int pool_index,
					   waterClass **method_class,
					   waterSlot *method_arguments,
					   int *method_arguments_count,
					   int max_arguments,
					   waterSlot *method_return_type)
{
    waterConstantReference *method_ref;
    waterConstantNameAndType *nameandtype_ref;

    if(pool_index >= method->clazz->constant_pool_count) {
	return -1;
    }
    
    if(method->clazz->constant_pool[pool_index].tag != 
           CONSTANT_METHODREF_TAG &&
       method->clazz->constant_pool[pool_index].tag != 
           CONSTANT_INTERFACE_METHODREF_TAG) {
	return -1;
    }
    method_ref = &method->clazz->constant_pool[pool_index].value.reference;
    
    *method_class = 
	water_resolveClassByIndex(env, method->clazz, method_ref->class_index, 
				  CLASS_FETUS);
    if(!*method_class) {
	return -1;
    }
    
    if(method_ref->name_and_type_index >= method->clazz->constant_pool_count) {
	return -1;
    }

    if(method->clazz->constant_pool[method_ref->name_and_type_index].tag != 
       CONSTANT_NAMEANDTYPE_TAG) {
	return -1;
    }
    nameandtype_ref = 
	&method->clazz->constant_pool[method_ref->name_and_type_index].
        value.name_and_type;
    
    if(nameandtype_ref->descriptor_index >=
       method->clazz->constant_pool_count) {
	return -1;
    }

    if(method->clazz->constant_pool[nameandtype_ref->descriptor_index].tag != 
       CONSTANT_UTF8_TAG) {
	return -1;
    }

    return 
	parse_method_descriptor(env, method,
				&method->clazz->constant_pool[
				nameandtype_ref->descriptor_index].
                                value.utf8_value, 
				method_arguments,
				method_arguments_count,
				max_arguments,
				method_return_type,
				ARGUMENT_NO_EXPAND);
}

static int get_source_type(unsigned char *code)
{
    switch(code[0]) {
    case(133): /* i2l */
    case(134): /* i2f */
    case(135): /* i2d */
    case(145): /* i2b */
    case(146): /* i2c */
    case(147): /* i2s */
	return JNI_INT;

    case(136): /* l2i */
    case(137): /* l2f */
    case(138): /* l2d */
	return JNI_LONG;

    case(139): /* f2i */
    case(140): /* f2l */
    case(141): /* f2d */
	return JNI_FLOAT;

    case(142): /* d2i */
    case(143): /* d2l */
    case(144): /* d2f */
	return JNI_DOUBLE;

    default:
	assert(0);
	return -1;
    }

    assert(0);
    return -1;
}

static int get_target_type(unsigned char *code)
{
    switch(code[0]) {
    case(136): /* l2i */
    case(139): /* f2i */
    case(142): /* d2i */
    case(145): /* i2b */
    case(146): /* i2c */
    case(147): /* i2s */
	return JNI_INT;

    case(133): /* i2l */
    case(140): /* f2l */
    case(143): /* d2l */
	return JNI_LONG;

    case(134): /* i2f */
    case(137): /* l2f */
    case(144): /* d2f */
	return JNI_FLOAT;

    case(135): /* i2d */
    case(138): /* l2d */
    case(141): /* f2d */
	return JNI_DOUBLE;

    default:
	assert(0);
	return -1;
    }

    assert(0);
    return -1;
}

static int get_array_type(unsigned char *code)
{
    jubyte byte;

    assert(code[0] == 188 /* newarray */);

    GET_BYTE_NOINC(byte, code, 1);

    return byte;
}

static int get_dimensions(unsigned char *code)
{
    jubyte byte;

    assert(code[0] == 197 /* multianewarray */);

    GET_BYTE_NOINC(byte, code, 3);

    return byte;
}

static int verifier_castIsOK(water_JNIEnv *wenv, 
			     waterClass *target, waterClass *source)
{
    /* just like water_castIsOK, but this time we have the case where */
    /* casting null (which we represent using a null class pointer)   */
    /* to anything is OK.                                             */
    if(source == 0) {
	return 1;
    } else {
	return water_castIsOK(wenv, target, source);
    }
}

static int is_jsr(unsigned char opcode) 
{
    return opcode == 168 /* jsr */ ||
	opcode == 201 /* jsr_w */;
}

static void free_instruction_table(waterInstruction *instruction_table,
				   int instruction_count)
{
    int i;

    /* in addition to the table itself we have to free the branch_targets. */
    for(i = 0; i < instruction_count; i++) {
	if(instruction_table[i].branch_targets) {
	    free(instruction_table[i].branch_targets);
	}
    }

    free(instruction_table);
}
