include Makefile.in
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
		$(MKDIR) -p $(BINDIR) ; \
		for FILE in $(EXECS) $(SHLIB) ; do \
			$(CP) -fv $(SRCDIR)$$FILE $(BINDIR) ; \
		done ; \
	else \
		$(RM) -Rfv $(BINDIR) ; \
	fi ; \
	


.PHONY: all







