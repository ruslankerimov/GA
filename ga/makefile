# -*- Mode: makefile -*-
# Copyright (c) 1994-1996 Massachusetts Institute of Technology
# Copyright (c) 1996-2005 Matthew Wall
# -----------------------------------------------------------------------------

include ../makevars
include makefile.sources

INC_DIRS= -I..

.SUFFIXES: .C
.C.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

$(LIB): $(OBJS) 
	$(PRELINK)
	$(AR) $(LIB) $?
	$(RANLIB) $(LIB)
	echo "$(CXX) $(CXXFLAGS)" > BUILD
	@echo $(LIB) is now up-to-date
.C.a:;

clean:
	$(RM) $(LIB) BUILD
	$(RM) *.o *~ .#* *.bak core ii_files ptrepository

install:: $(LIB)
	$(INSTALL) $(LIB) $(LIB_DEST_DIR)
	$(RM) $(HDR_DEST_DIR)/ga; $(MKDIR) $(HDR_DEST_DIR)/ga
	$(CP) $(HDRS) $(TMPL_SRCS) ../VERSION BUILD $(HDR_DEST_DIR)/ga

uninstall:
	$(RM) $(LIB_DEST_DIR)/$(LIB)
	$(RM) $(HDR_DEST_DIR)/ga

depend:
	$(MKDEPEND) $(INC_DIRS) $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
