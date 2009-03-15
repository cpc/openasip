bin_PROGRAMS = tblgen

tblgen_SOURCES = \
  AsmWriterEmitter.cpp \
  CallingConvEmitter.cpp \
  CodeEmitterGen.cpp \
  CodeGenDAGPatterns.cpp \
  CodeGenInstruction.cpp \
  CodeGenTarget.cpp \
  DAGISelEmitter.cpp \
  InstrEnumEmitter.cpp \
  InstrInfoEmitter.cpp \
  IntrinsicEmitter.cpp \
  LLVMCConfigurationEmitter.cpp \
  Record.cpp \
  RegisterInfoEmitter.cpp \
  SubtargetEmitter.cpp \
  TGLexer.cpp \
  TGParser.cpp \
  TGValueTypes.cpp \
  TableGen.cpp \
  TableGenBackend.cpp \
  FastISelEmitter.cpp 

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

# headers start
tblgen_SOURCES += \
	AsmWriterEmitter.h CodeGenDAGPatterns.h \
	CodeGenRegisters.h FastISelEmitter.h \
	IntrinsicEmitter.h RegisterInfoEmitter.h \
	TGLexer.h CallingConvEmitter.h \
	CodeGenInstruction.h CodeGenTarget.h \
    InstrEnumEmitter.h LLVMCConfigurationEmitter.h \
    SubtargetEmitter.h TGParser.h \
	CodeEmitterGen.h CodeGenIntrinsics.h \
	DAGISelEmitter.h InstrInfoEmitter.h \
    Record.h TableGenBackend.h 
