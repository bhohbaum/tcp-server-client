include ../Makefile.in
########################################################################################################################
# Build targets and build steps
########################################################################################################################
all: libs execs

execs: $(EXECS)

libs: $(SHLIB)

$(EXECS): $(OBJ) $(SHLIB)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

tcpserver: $(SHLIB)
	$(CC) -o $@ $@.o $(LD_FLAGS)

tcpclient: $(SHLIB)
	$(CC) -o $@ $@.o $(LD_FLAGS)

$(SHLIB): $(OBJ)
	$(CC) -o $(SHLIB) $(SO_LIBFLAGS) $(LIBOBJ)
	
.PHONY: all

clean:
	rm -f $(OBJ) $(EXECS) $(SHLIB)









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
		
		
		