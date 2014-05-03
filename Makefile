########################################################################################################################
# External programs
########################################################################################################################
CC=$(shell which gcc)
OBJCP=$(shell which objcopy)
MV=$(shell which mv)
CP=$(shell which cp)
RM=$(shell which rm)
NM=$(shell which nm)
GREP=$(shell which grep)
AWK=$(shell which awk)
SED=$(shell which sed)
FIND=$(shell which find)
MKDIR=$(shell which mkdir)
DIRNAME=$(shell which dirname)

########################################################################################################################
# Compiler & Linker settings
########################################################################################################################
BASE_CFLAGS=-O3 -Wall -fPIC
DEBUG_CFLAGS=-g
SO_LIBFLAGS=--shared -Bdynamic -fPIE

########################################################################################################################
# Files and Filetypes
########################################################################################################################
CMAIN=tcpserver.c tcpclient.c
EXECS=tcpserver tcpclient
HDEPS=netcomcore.h
CDEPS=netcomcore.c
SOURCES=$(CMAIN) $(CDEPS)
HEADERS=$(HDEPS)
MAINOBJ=tcpserver.o tcpclient.o
LIBOBJ=netcomcore.o
OBJ=$(MAINOBJ) $(LIBOBJ)
SHLIB=tcp-server-client.so
BINDIR=bin/
SRCDIR=src/

TARGET=

########################################################################################################################
# Debug switch
########################################################################################################################
ifdef DEBUG
CFLAGS=$(BASE_CFLAGS) $(DEBUG_CFLAGS)
else
CFLAGS=$(BASE_CFLAGS)
endif

########################################################################################################################
# Build targets and build steps
########################################################################################################################
all execs libs clean:
	BASEDIR=$(shell pwd) ; \
	for MAKEFILE in `$(FIND) $$BASEDIR -name Makefile -type f` ; do \
		if [ "`$(DIRNAME) $$MAKEFILE`" != "$$BASEDIR" ] ; then \
			cd $$BASEDIR ; \
			cd `$(DIRNAME) $$MAKEFILE` ; \
			$(MAKE) $@ DEBUG=$(DEBUG) ; \
		fi ; \
	done ; \
	cd $$BASEDIR ; \
	if [ "$@" != "clean" ] ; then \
		$(MKDIR) $(BINDIR) ; \
		for FILE in $(EXECS) $(SHLIB) ; do \
			$(CP) -fv $(SRCDIR)$$FILE $(BINDIR) ; \
		done ; \
	else \
		$(RM) -Rfv $(BINDIR) ; \
	fi ; \
	


.PHONY: all







