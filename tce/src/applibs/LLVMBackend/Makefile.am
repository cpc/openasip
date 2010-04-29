SUBDIRS = passes

noinst_LTLIBRARIES = libtcellvmbackend.la

libtcellvmbackend_la_SOURCES = TCESubtarget.cc  \
    TCETargetMachine.cc TCEMCAsmInfo.cc  LLVMPOMBuilder.cc \
	LLVMBackend.cc LowerMissingInstructions.cc TDGen.cc \
	passes/LinkBitcode.cc passes/MachineDCE.cc passes/LowerIntrinsics.cc


PROJECT_ROOT = $(top_srcdir)
SRC_ROOT = ${PROJECT_ROOT}/src
TOOLS_DIR = ${SRC_ROOT}/tools
MACH_DIR = ${SRC_ROOT}/base/mach
PROG_DIR = ${SRC_ROOT}/base/program
TPEF_DIR  = ${SRC_ROOT}/base/tpef
OSAL_DIR = ${SRC_ROOT}/base/osal
GRAPH_DIR = ${SRC_ROOT}/base/Graph
OSAL_APPLIBS_DIR = ${SRC_ROOT}/applibs/osal
MACH_APPLIBS_DIR = ${SRC_ROOT}/applibs/mach
UMACH_DIR = ${SRC_ROOT}/base/umach
DISASM_DIR = ${SRC_ROOT}/applibs/Disassembler
SCHED_ALGO_DIR = ${SRC_ROOT}/applibs/Scheduler/Algorithms
SCHED_DIR = ${SRC_ROOT}/applibs/Scheduler

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} \
    -I${PROG_DIR} -I${TPEF_DIR} -I${OSAL_DIR} -I${DISASM_DIR} \
    -I${UMACH_DIR} -I${OSAL_APPLIBS_DIR} -I${GRAPH_DIR} \
    -I${MACH_APPLIBS_DIR} -I${SCHED_ALGO_DIR} -I${SCHED_DIR} ${LLVM_CPPFLAGS}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

# AM_CXXFLAGS is put before CXXFLAGS in compile command and -Wall overrides
# -Wno-unused-parameter in that case. So to keep correct ordering
# we move CXXFLAGS to AM_CXXFLAGS with additional -Wno switch 
# and clean old CXXFLAGS variable
AM_CXXFLAGS = ${CXXFLAGS}
CXXFLAGS="-Wno-unused-parameter"

# Static sources for target machine plugins.
include_HEADERS = \
 plugin/TCEDAGToDAGISel.cc \
 plugin/TCEDAGToDAGISel.hh \
 plugin/TCEInstrFormats.td \
 plugin/TCEInstrInfo.cc \
 plugin/TCEInstrInfo.hh \
 plugin/TCEInstrInfo.td \
 plugin/TCEPlugin.hh \
 plugin/TCERegisterInfo.cc \
 plugin/TCERegisterInfo.hh \
 plugin/TCEISelLowering.cc \
 plugin/TCEISelLowering.hh \
 plugin/TCETargetObjectFile.cc \
 plugin/TCETargetObjectFile.hh \
 plugin/TCETargetMachinePlugin.cc \
 plugin/TCECallingConv.td \
 TCETargetMachine.hh \
 TCETargetMachinePlugin.hh \
 TCESubtarget.hh

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da



## headers start
libtcellvmbackend_la_SOURCES += \
	TCETargetMachine.hh LLVMBackend.hh \
	TCEMCAsmInfo.hh TCESubtarget.hh \
	LLVMPOMBuilder.hh TDGen.hh \
	TCETargetMachinePlugin.hh 
## headers end
