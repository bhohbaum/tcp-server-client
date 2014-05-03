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
DIRNAME=$(shell which dirname)

########################################################################################################################
# Compiler & Linker settings
########################################################################################################################
BASE_CFLAGS=-O3 -Wall -fPIC
DEBUG_CFLAGS=-g
SO_LIBFLAGS=--shared -Bdynamic
AR_LIBFLAGS=--static -Bstatic -lc

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
ARLIB=tcp-server-client.a

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
all: execs libs

execs: $(EXECS)

libs: $(SHLIB) $(ARLIB)

$(EXECS) $(SHLIB) $(ARLIB): $(OBJ)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

tcpserver: $(LIBOBJ)
	$(CC) -o $@ $@.o $<

tcpclient: $(LIBOBJ)
	$(CC) -o $@ $@.o $<

$(SHLIB) $(ARLIB): $(OBJ)
	$(CC) -o $(SHLIB) $(SO_LIBFLAGS) $(LIBOBJ)
	$(CC) -o $(ARLIB) $(AR_LIBFLAGS) $(LIBOBJ)
	
.PHONY: clean

clean:
	rm -f $(OBJ) $(EXECS) $(SHLIB) $(ARLIB)




include src/Makefile





########################################################################################################################
# cheat-shit...
########################################################################################################################


#tcp%: tcp%.o
#	$(CC) -o $@ $^


#$(OBJ): $(OBJ)
#	ORIG=$(shell echo $@ | $(SED) -e 's/loc-//g') ; \
#	$(MV) $$ORIG $@

#	BASEDIR=`$(DIRNAME) $(shell pwd)/$0 | $(SED) -e 's/\/.\//\//g'` ; \
#	for MAKEFILE in `$(FIND) $$BASEDIR -name Makefile -type f` ; do \
#		$(CD) `$(DIRNAME) $$BASEDIR` ; \
#		$0 $@ ; \
#	done ; \

#		$(OBJCP) --localize-symbol=$$FUNCNAME $$ORIG $@ ; \
#		$(MV) $@ $$ORIG ; \
		
		
		