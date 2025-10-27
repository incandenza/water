#
# Config stuff for Darwin (macOS)
#

ARCH            := darwin
CPU_ARCH        := x86_64
GFX_ARCH        := x

OS_INCLUDES     = -I/opt/X11/include
G++INCLUDES     = -I/usr/include/c++/v1
LOC_LIB_DIR     = /opt/X11/lib
MOTIF           =
MOTIFLIB        =
OS_LIBS         =

PLATFORM_FLAGS      = -DDARWIN -DMACOSX $(DSO_CFLAGS)
MOVEMAIL_FLAGS     = -DHAVE_STRERROR
PORT_FLAGS         = -DSW_THREADS -DNEED_UINT -DHAVE_LCHOWN -DNTOHL_ENDIAN_H -DHAVE_FILIO_H -DNEED_SYS_TIME_H -DNEED_UINT_T -DHAVE_BSD_FLOCK

OS_CFLAGS       = $(PLATFORM_FLAGS) $(PORT_FLAGS) $(MOVEMAIL_FLAGS)

LOCALE_MAP      = $(DEPTH)/cmd/xfe/intl/linux.lm
EN_LOCALE       = C
DE_LOCALE       = de_DE.ISO8859-1
FR_LOCALE       = fr_FR.ISO8859-1
JP_LOCALE       = ja
SJIS_LOCALE     = ja_JP.SJIS
KR_LOCALE       = ko_KR.EUC
CN_LOCALE       = zh
TW_LOCALE       = zh
I2_LOCALE       = i2

# Overrides for defaults in config.mk
EMACS           = /bin/true
JAVA_PROG       = $(JAVA_BIN)java
PERL            = /usr/bin/perl
PROCESSOR_ARCHITECTURE = _$(CPU_ARCH)
RANLIB          = /usr/bin/ranlib
UNZIP_PROG      = /usr/bin/unzip
ZIP_PROG        = /usr/bin/zip

NEED_XMOS       = 1

DSO_CFLAGS      = -fpic
DSO_LDOPTS      = -shared
DSO_LDFLAGS     =

MKSHLIB         = $(CC) -shared -Wl,-soname -Wl,$(@:$(OBJDIR)/%.so=%.so)

ifdef BUILD_OPT
ifdef MY_OPTIMIZER
OPTIMIZER       = $(MY_OPTIMIZER)
else
OPTIMIZER       = -O3 -funroll-loops
endif
endif
