#!gmake
#
# The contents of this file are subject to the Netscape Public License
# Version 1.0 (the "NPL"); you may not use this file except in
# compliance with the NPL.  You may obtain a copy of the NPL at
# http://www.mozilla.org/NPL/
#
# Software distributed under the NPL is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
# for the specific language governing rights and limitations under the
# NPL.
#
# The Initial Developer of this code under the NPL is Netscape
# Communications Corporation.  Portions created by Netscape are
# Copyright (C) 1998 Netscape Communications Corporation.  All Rights
# Reserved.


DEPTH		= ..
MODULE		= water
LIBRARY_NAME	= $(LITE_PREFIX)water

MOD_DEPTH	= .
DIRS		= native-vmspecific native-jni native-net

REQUIRES	= nspr
EXPORTS		= jni.h jni_types.h

ifdef BUILD_OPT
MY_OPTIMIZER    = -O3 -funroll-loops -DUSE_SWITCH #-DUSE_LOCAL_COPIES
else
MY_OPTIMIZER    = -g
endif

ifdef BUILD_PROF
MY_OPTIMIZER   += -pg
endif

CSRCS		= create.c \
		  classes.c \
		  entry.c \
		  globals.c \
		  miscjni.c \
		  call.c \
		  stack.c \
		  exec.c \
		  objects.c \
		  utf8.c \
		  exception.c \
		  verify.c \
		  water.c

EXTRA_OBJS	= native-asm.o

LOCAL_INCLUDES	= -I.

LDFLAGS	       += -rdynamic -L$(DEPTH)/dist/$(OBJDIR)/lib -lnspr4
PROGRAM		= water$(BIN_SUFFIX)

# these are generated files that we want to go away when
# doing a 'make realclean'.
EXTRA_TRASH	= exec-instr-protos.c \
		  exec-instr-table.c \
		  exec-instr-table-stubs.c \
		  exec-instr-switch.c \
		  exec-instr-macros.c

include $(DEPTH)/config/rules.mk

$(LIBRARY): $(OBJS)

exec-instr-protos.c: exec-instr-pre.c munge-instructions
	./munge-instructions

exec-instr-table.c: exec-instr-pre.c munge-instructions
	./munge-instructions

exec-instr-table-stubs.c: exec-instr-pre.c munge-instructions
	./munge-instructions

exec-instr-macros.c: exec-instr-pre.c munge-instructions
	./munge-instructions

exec-instr-switch.c: exec-instr-pre.c munge-instructions
	./munge-instructions

$(OBJDIR)/native-asm.o: native-asm.S
	as -o $(OBJDIR)/native-asm.o native-asm.S

$(OBJDIR)/exec.o: exec-instr-protos.c
$(OBJDIR)/exec.o: exec-instr-table.c
$(OBJDIR)/exec.o: exec-instr-table-stubs.c
$(OBJDIR)/exec.o: exec-instr-macros.c
$(OBJDIR)/exec.o: exec-instr-switch.c
