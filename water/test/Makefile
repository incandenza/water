JDKDIR=/usr/local/jdk1.1.5
MOZILLADIR=/home/djm/mozilla/mozilla
ifeq ($(BUILD_OPT), 1)
MOZILLAOBJDIR=Linux2_x86_OPT.OBJ
else
MOZILLAOBJDIR=Linux2_x86_DBG.OBJ
endif

CC=gcc
ifdef BUILD_OPT
CFLAGS=-O3 -rdynamic
else
CFLAGS=-g -rdynamic
endif

ifdef BUILD_PROF
CFLAGS+=-pg
endif

JAVAC=$(JDKDIR)/bin/javac

ifeq ($(SUN_BUILD),1)
LIBS=-L$(JDKDIR)/lib/Linux/green_threads -ljava
INCLUDES=-I$(JDKDIR)/include -I$(JDKDIR)/include/genunix
else
LIBS=-L$(MOZILLADIR)/dist/$(MOZILLAOBJDIR)/lib -lwater -lnspr21 ../native-asm.o -lm #-lefence
INCLUDES=-I$(MOZILLADIR)/dist/public/water \
	-I$(MOZILLADIR)/dist/$(MOZILLAOBJDIR)/include
endif

all: Prog.class ProgParent.class ProgInterface.class jnitest water

ifneq ($(SUN_BUILD),1)
jnitest: ../$(MOZILLAOBJDIR)/libwater.a
water: ../$(MOZILLAOBJDIR)/libwater.a
endif

clean:
	rm -f jnitest water core

realclean:
	rm -f Prog.class jnitest *~ *.class #*# core

%.class: %.java
	$(JAVAC) $<

jnitest: jnitest.c
	$(CC) $(CFLAGS) $(INCLUDES) -o jnitest jnitest.c $(LIBS)

water: water.c
	$(CC) $(CFLAGS) $(INCLUDES) -o water water.c $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<
