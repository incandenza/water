#
# water
# a Java virtual machine
# 
# Copyright (C) 1998-2025 Dan McGuirk <mcguirk@gmail.com>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#

.global water_asmCallNative

water_asmCallNative:
	pushl	%ebp
	movl	%esp, %ebp
	# XXX figure out which of these registers we're allowed to clobber
	pushl	%esi
	pushl	%edi
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
			
	movl	0x18(%ebp), %ebx	# get total size of arguments
	movl	%ebx, %ecx		# this will be the count for the copy
	addl	$0x8, %ebx		# stack args size: add room for 'env' 
					# and 'this'
	subl	%ebx, %esp		# make room on the stack

	movl	0x14(%ebp), %esi	# the start of the arguments
	movl	%esp, %edi		# the destination
	addl	$0x8, %edi		# skip 'env' and 'this'

	cld				# copy the arguments
	rep
	movsb	(%esi), (%edi)

	movl	0x8(%ebp), %eax		# copy the env pointer
	movl	%eax, 0x0(%esp)
	movl	0xc(%ebp), %eax		# copy the this pointer
	movl	%eax, 0x4(%esp)

	movl	0x10(%ebp), %eax	# get pointer to the native code
	call	%eax			# call it

	addl	%ebx, %esp		# kill the arguments

	# careful--don't clobber %eax or %edx here, they may contain results.
	movl	0x24(%ebp), %ebx	# check for exception
	movl	(%ebx), %ecx
	andl	$0x1, %ecx
	jnz	finish			# if so, don't mess with the return
					# value.  the exception will be on
					# the stack instead.
	
	movl	0x1c(%ebp), %ecx	# get the return pointer
	movl	0x20(%ebp), %ebx	# and the return type
	cmpl	$0x63, %ebx		# void
	je	finish
	
	cmpl	$0xb, %ebx		# a 64-bit long
	jne	maybefloat
isdword:
	movl	%eax, (%ecx)
	movl	%edx, 4(%ecx)
	jmp	finish
maybefloat:
	cmpl	$0x6, %ebx		# float
	jne	maybedouble
float:
	# XXX dunno how to do float yet
	jmp	finish
maybedouble:
	cmpl	$0x7, %ebx		# double
	jne	isword			# not long, float, or double ==
					# regular 32-bit word of some kind
double:
	# XXX dunno how to do double yet
	jmp	finish
	
isword:	
	movl	%eax, (%ecx)
	jmp	finish
	
finish:	
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%edi
	popl	%esi

	popl	%ebp
	ret
