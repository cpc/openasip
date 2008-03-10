bin_PROGRAMS = tblgen

tblgen_SOURCES = \
AsmWriterEmitter.cpp \
CallingConvEmitter.cpp \
CodeEmitterGen.cpp \
CodeGenTarget.cpp \
DAGISelEmitter.cpp \
FileLexer.cpp \
FileParser.cpp \
InstrInfoEmitter.cpp \
IntrinsicEmitter.cpp \
Record.cpp \
RegisterInfoEmitter.cpp \
SubtargetEmitter.cpp \
TableGenBackend.cpp \
TableGen.cpp

AM_CPPFLAGS = $(LLVM_CPPFLAGS)
AM_LDFLAGS = -L$(LLVM_LIBDIR) -lLLVMSupport -lLLVMSystem -lpthread -ldl -lm

# LLVM headers have lots of unused parameters, do not warn about those.
# Had to define CXXCOMPILE because AM_CXXFLAGS is defined before CXXFLAGS
# in the default CXXCOMPILE command, which might include -Wall which 
# overrides the -Wno-unused-parameter flag.
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -Wno-unused-parameter

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


include_HEADERS = \
  td/TargetCallingConv.td \
  td/TargetSchedule.td \
  td/TargetSelectionDAG.td \
  td/Target.td
