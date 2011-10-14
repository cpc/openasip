SUBDIRS = CL
pkglib_LTLIBRARIES = llvmopencl.la 

llvmopencl_la_SOURCES = AddContext.hh AddContext.cc \
                        Flatten.cc \
                        GenerateWarps.hh GenerateWarps.cc \
                        IndependentPointers.cc \
                        KernelFunctions.hh KernelFunctions.cc \
                        LoopRegions.hh LoopRegions.cc \
                        OpenCL.hh OpenCL.cc \
                        Rematerialize.hh Rematerialize.cc \
                        SplitRegions.hh SplitRegions.cc \
                        ThreadContext.hh
#                        BuildDDG.cc
llvmopencl_la_CPPFLAGS = `@LO_LLVM_CONFIG@ --cppflags`
llvmopencl_la_LDFLAGS = -module -export-dynamic $(LDFLAGS) `@LO_LLVM_CONFIG@ --ldflags`

PROJECT_ROOT = $(top_srcdir)

llvmopencl_la_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h
