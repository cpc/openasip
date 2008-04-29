bin_PROGRAMS = tce-bugpoint

tce_bugpoint_SOURCES = BugDriver.cpp bugpoint.cpp CrashDebugger.cpp ExecutionDriver.cpp ExtractFunction.cpp FindBugs.cpp Miscompilation.cpp OptimizerDriver.cpp TestPasses.cpp ToolRunner.cpp

AM_CPPFLAGS = $(LLVM_CPPFLAGS)
#AM_LDFLAGS = -L$(LLVM_LIBDIR) -lLLVMSupport -lLLVMSystem -lpthread -ldl -lm

# These are the original LD flags ripped from the LLVM 2.2 Bugpoint Makefile
tce_bugpoint_LDFLAGS =  -L${LLVM_LIBDIR} \
-lLLVMBitWriter -lLLVMLinker -lLLVMArchive -lLLVMBitReader -lLLVMipo \
-lLLVMInstrumentation -lLLVMScalarOpts -lLLVMTransformUtils -lLLVMipa \
-lLLVMAnalysis -lLLVMTarget -lLLVMAsmParser -lLLVMCore -lLLVMSupport \
-lLLVMSystem -lpthread -ldl -lm

# LLVM headers have lots of unused parameters, do not warn about those.
# Had to define CXXCOMPILE because AM_CXXFLAGS is defined before CXXFLAGS
# in the default CXXCOMPILE command, which might include -Wall which 
# overrides the -Wno-unused-parameter flag.
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -Wno-unused-parameter

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
